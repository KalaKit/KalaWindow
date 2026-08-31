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

	template<typename T>
	concept HasGetID = requires(const T& t) { { t.GetID() } -> same_as<u32>; };

	//Stores unique_ptrs and non-owning pointers of class T for ID-based lookups,
	//should always be stored as 'static inline KalaWindowRegistry<T> registry'
	template<typename T>
		requires is_class_v<T>
	struct LIB_API KalaWindowRegistry
	{
		//Get a runtime iteration safe list of all
		//created object pointers of this registry
		KNODISCARD
		static inline const vector<T*>& GetAllContent() { return runtimeContent; }

		//Get non-owning value by ID or index, returns error string on failure
		KNODISCARD
		static inline string GetContent(
			u32 targetValue,
			T*& outValue,
			bool getByID = true)
		{
			if (getByID)
			{
				auto it = createdContent.find(targetValue);
				if (it == createdContent.end())
				{
					return "GetContent failed because target ID '" + to_string(targetValue) + "' was not found!";
				}

				outValue = it->second.get();
			}
			else
			{
				if (targetValue >= runtimeContent.size())
				{
					return "GetContent failed because target index '" + to_string(targetValue) + "' was not found!";
				}

				outValue = runtimeContent[targetValue];
			}

			return "";
		}

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

			runtimeContent.erase(vecit);
			createdContent.erase(targetID);

			return "";
		}
		//Destroy content by non-owning pointer, returns error string on failure
		KNODISCARD
		static inline string DestroyContent(T* targetPtr) requires HasGetID<T>
		{
			if (!targetPtr) return "DestroyContent failed because target pointer was invalid!";

			u32 targetID = targetPtr->GetID();

			auto it = createdContent.find(targetID);
			if (it == createdContent.end())
			{
				return "DestroyContent failed because target pointer ID '" + to_string(targetID) + "' was not found!"; 
			}

			auto vecit = find(
				runtimeContent.begin(), 
				runtimeContent.end(), 
				targetPtr);
			if (vecit == runtimeContent.end())
			{
				return
					"DestroyContent failed because target pointer ID '" + to_string(targetID) + "' was not found "
					"in runtime container and was moved to batch add/remove container!";
			}

			runtimeContent.erase(vecit);
			createdContent.erase(targetID);

			return "";
		}

		//Remove all created content from this registry
		static inline void DestroyAllContent()
		{
			runtimeContent.clear();
			createdContent.clear();
		}
	private:
		static inline unordered_map<u32, unique_ptr<T>> createdContent{};
		static inline vector<T*> runtimeContent{};
	};
}