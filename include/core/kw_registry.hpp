//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "core_utils.hpp"

#include "core/kw_core.hpp"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <mutex>
#include <shared_mutex>

namespace KalaWindow::Graphics
{
	class ProcessWindow;
	class VulkanContext;
}

namespace KalaWindow::Core
{
	using std::unordered_map;
	using std::vector;
	using std::string;
	using std::to_string;
	using std::unique_ptr;
	using std::make_unique;
	using std::find;
	using std::remove;
	using std::remove_if;
	using std::is_class_v;
	using std::same_as;
	using std::shared_mutex;
	using std::scoped_lock;
	using std::shared_lock;
	using std::defer_lock;
	using std::exception;

	template<typename T>
	concept HasGetID = requires(const T& t) { { t.GetID() } -> same_as<u32>; };

	//Stores unique_ptrs and non-owning pointers of class T for ID-based lookups,
	//should always be stored as 'static inline KalaWindowRegistry<T> registry'
	template<typename T>
		requires is_class_v<T>
	struct LIB_API KalaWindowRegistry
	{
	friend class Input;
	friend class KalaWindow::Graphics::ProcessWindow;
	friend class KalaWindow::Graphics::VulkanContext;
	public:
		//Lock the registry for further writes to read registry-owned objects
		//during the lifetime of the callback, returns error string on failure.
		//This function is great for preserving objects while you need to read them for multithreaded use.
		//Has no benefit for fully single-threaded programs.
		//Nobody else can write to the registry while this function callback is active
		template<typename F>
		KNODISCARD
		static inline string LockRegistry(F&& callback)
		{
			if (isLocked)
			{
				return "LockRegistry failed! Reason: Nested LockRegistry calls are not allowed.";
			}

			shared_lock lock(registryMutex);
			isLocked = true;

			try
			{
				callback();
			}
			catch (const exception& e)
			{
				isLocked = false;
				return "LockRegistry failed! Reason: " + string(e.what());
			}
			catch (...)
			{
				isLocked = false;
				return "LockRegistry failed! Reason: Unknown error.";
			}

			isLocked = false;

			return "";
		}

		//Get a copy of all created object pointers of this registry.
		//Does not guarantee returned content is still alive once returned
		//if used in a program that has multiple threads accessing this registry,
		//combine this function with LockRegistry for best results
		KNODISCARD
		static inline vector<T*> GetAllContent()
		{
			shared_lock lock(registryMutex, defer_lock);
			if (!isLocked) lock.lock();

			return runtimeContent;
		}

		//Get non-owning value by ID or index, returns error string on failure.
		//Does not guarantee returned content is still alive once returned
		//if used in a program that has multiple threads accessing this registry,
		//combine this function with LockRegistry for best results
		KNODISCARD
		static inline string GetContent(
			u32 targetValue,
			T*& outValue,
			bool getByID = true)
		{
			if (getByID)
			{
				shared_lock lock(registryMutex, defer_lock);
				if (!isLocked) lock.lock();

				auto it = createdContent.find(targetValue);
				if (it == createdContent.end())
				{
					return "GetContent failed because target ID '" + to_string(targetValue) + "' was not found!";
				}

				outValue = it->second.get();
			}
			else
			{
				shared_lock lock(registryMutex, defer_lock);
				if (!isLocked) lock.lock();

				if (targetValue >= runtimeContent.size())
				{
					return "GetContent failed because target index '" + to_string(targetValue) + "' was not found!";
				}

				outValue = runtimeContent[targetValue];
			}

			return "";
		}
	private:
		//Add a new unique ptr and its ID, returns error string on failure
		KNODISCARD
		static inline string AddContent(
			u32 targetID,
			unique_ptr<T> targetContent)
		{
			if (!targetContent)
			{
				return "AddContent failed because target content was invalid!";
			}
			if (targetID == 0)
			{
				return "AddContent failed because target ID was empty!";
			}
			if (isLocked)
			{
				return "AddContent failed because the registry is locked by the current thread!";
			}

			scoped_lock lock(registryMutex);

			if (createdContent.contains(targetID))
			{
				return "AddContent failed because target '" + to_string(targetID) + "' has already been added!";
			}

			T* raw = targetContent.get();
			createdContent[targetID] = std::move(targetContent);
			runtimeContent.push_back(raw);

			return "";
		}

		//Destroy content by ID, returns error string on failure
		KNODISCARD
		static inline string DestroyContent(u32 targetID)
		{
			if (isLocked)
			{
				return "DestroyContent failed because the registry is locked by the current thread!";
			}

			unique_ptr<T> createdObjectCopy{};

			{
				scoped_lock lock(registryMutex);

				auto it = createdContent.find(targetID);
				if (it == createdContent.end())
				{
					return "DestroyContent failed because target ID '" + to_string(targetID) + "' was not found!"; 
				}
				
				T* targetPtr = it->second.get();
				if (!targetPtr)
				{
					KalaWindowCore::ForceClose(
						"KalaWindow registry error",
						"DestroyContent failed because target ID '" + to_string(targetID) + "' is a dangling pointer, it was freed externally!");
				}
				
				auto vecit = find(
					runtimeContent.begin(), 
					runtimeContent.end(), 
					targetPtr);
				if (vecit == runtimeContent.end())
				{
					return
						"DestroyContent failed because target ID '" + to_string(targetID) + "' was not found "
						"in runtime container and was moved to batch add/remove container!";
				}

				createdObjectCopy = std::move(it->second);

				runtimeContent.erase(vecit);
				createdContent.erase(targetID);
			}

			createdObjectCopy.reset();

			return "";
		}
		//Destroy content by non-owning pointer, returns error string on failure
		KNODISCARD
		static inline string DestroyContent(T* targetPtr) requires HasGetID<T>
		{
			if (!targetPtr) return "DestroyContent failed because target pointer was invalid!";

			if (isLocked)
			{
				return "DestroyContent failed because the registry is locked by the current thread!";
			}

			unique_ptr<T> createdObjectCopy{};

			{
				scoped_lock lock(registryMutex);

				auto vecit = find(
					runtimeContent.begin(),
					runtimeContent.end(),
					targetPtr);

				if (vecit == runtimeContent.end())
				{
					return "DestroyContent failed because target pointer was not found!";
				}

				u32 targetID = targetPtr->GetID();

				auto it = createdContent.find(targetID);

				if (it == createdContent.end()
					|| it->second.get() != targetPtr)
				{
					return "DestroyContent failed because target pointer ID did not match its registered content!";
				}

				createdObjectCopy = std::move(it->second);
				
				runtimeContent.erase(vecit);
				createdContent.erase(it);
			}

			createdObjectCopy.reset();

			return "";
		}

		//Remove all created content from this registry, returns error string on failure
		KNODISCARD
		static inline string DestroyAllContent()
		{
			if (isLocked)
			{
				return "DestroyAllContent failed because the registry is locked by the current thread!";
			}

			unordered_map<u32, unique_ptr<T>> createdContentCopy{};
			vector<T*> runtimeContentCopy{};

			{
				scoped_lock lock(registryMutex);

				runtimeContentCopy = std::move(runtimeContent);
				createdContentCopy = std::move(createdContent);
			}

			runtimeContentCopy.clear();
			createdContentCopy.clear();

			return "";
		}

		static inline thread_local bool isLocked{};

		static inline shared_mutex registryMutex{};
		static inline unordered_map<u32, unique_ptr<T>> createdContent{};
		static inline vector<T*> runtimeContent{};
	};
}