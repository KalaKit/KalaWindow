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

	template<typename T>
	concept HasGetWindowID = requires(const T& t) { { t.GetWindowID() } -> same_as<u32>; };

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
					"in runtime container ""and was moved to batch add/remove container!";
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
					"in runtime container ""and was moved to batch add/remove container!";
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

		//Move this this content from live containers
		//to batch add containers for batch removal,
		//does not destroy moved content, must call DestroyBatchAddContent,
		//use DestroyContent for destroying a single object
		KNODISCARD
		static inline string PushBatchAddContent()
		{
			return "";
		}
		//Return this batch add content back to live containers
		KNODISCARD
		static inline string PullBatchAddContent()
		{
			return "";
		}

		//Return all batch add content back to live containers
		static inline void ClearBatchAddContent()
		{
			runtimeContent.insert(
				runtimeContent.end(),
				runtimeBatchAddContent.begin(),
				runtimeBatchAddContent.end());
				
			runtimeBatchAddContent.clear();
		}
		//Remove all batched add content from this registry
		static inline void DestroyBatchAddContent()
		{
			runtimeBatchAddContent.clear();
			createdContent.clear();
		}

		//Move this this content from live containers
		//to batch remove containers for batch removal,
		//does not destroy moved content, must call DestroyBatchRemoveContent,
		//use DestroyContent for destroying a single object
		KNODISCARD
		static inline string PushBatchRemoveContent()
		{
			return "";
		}
		//Return this batch remove content back to live containers
		KNODISCARD
		static inline string PullBatchRemoveContent()
		{
			return "";
		}

		//Return all batch remove content back to live containers
		static inline void ClearBatchRemoveContent()
		{
			runtimeContent.insert(
				runtimeContent.end(),
				runtimeBatchRemoveContent.begin(),
				runtimeBatchRemoveContent.end());

			runtimeBatchRemoveContent.clear();
		}
		//Remove all batched remove content from this registry
		static inline void DestroyBatchRemoveContent()
		{
			runtimeBatchRemoveContent.clear();
			createdContent.clear();
		}
		
		//
		// WINDOW-RELATED ACTIONS
		//
			
		//Get all content as non-owning pointers by window ID from containers.
		//Requires target class inside createdContent and runtimeContent
		//to have the 'u32 GetWindowID()' function.
		//Should not be used for externally created registries
		//because the Window class does not accept new IDs,
		//returns error string on failure
		KNODISCARD
		static inline string GetAllWindowContent(
			u32 windowID,
			vector<T*>& outContent) requires HasGetWindowID<T>
		{
			auto it = createdContent.find(windowID);
			if (it == createdContent.end())
			{
				return "GetAllWindowContent failed because window ID '" + to_string(windowID) + "' was not found!"; 
			}
			
			T* targetPtr = it->second.get();
			if (!targetPtr)
			{
				KalaWindowCore::ForceClose(
					"KalaWindow registry error",
					"GetAllWindowContent failed because window ID '" + to_string(windowID) + "' is a dangling pointer, it was freed externally!");
			}
			
			auto vecit = find(
				runtimeContent.begin(), 
				runtimeContent.end(), 
				targetPtr);
			if (vecit == runtimeContent.end())
			{
				return
					"GetAllWindowContent failed because window ID '" + to_string(windowID) + "' was not found "
					"in runtime container and was moved to batch add/remove container!";
			}

			vector<T*> out{};

			for (const auto& v : runtimeContent)
			{
				if (v->GetWindowID() == windowID) out.push_back(v);
			}
				
			outContent = std::move(out);
			return "";
		}

		//Remove all content by window ID from containers.
		//Requires target class inside createdContent and runtimeContent
		//to have the 'u32 GetWindowID()' function.
		//Should not be used for externally created registries
		//because the Window class does not accept new IDs,
		//returns error string on failure
		KNODISCARD
		static inline string DestroyAllWindowContent(u32 windowID) requires HasGetWindowID<T>
		{
			auto it = createdContent.find(windowID);
			if (it == createdContent.end())
			{
				return "DestroyAllWindowContent failed because window ID '" + to_string(windowID) + "' was not found!"; 
			}
			
			T* targetPtr = it->second.get();
			if (!targetPtr)
			{
				KalaWindowCore::ForceClose(
					"KalaWindow registry error",
					"DestroyAllWindowContent failed because window ID '" + to_string(windowID) + "' is a dangling pointer, it was freed externally!");
			}
			
			auto vecit = find(
				runtimeContent.begin(), 
				runtimeContent.end(), 
				targetPtr);
			if (vecit == runtimeContent.end())
			{
				return
					"DestroyAllWindowContent failed because window ID '" + to_string(windowID) + "' was not found "
					"in runtime container and was moved to batch add/remove container!";
			}

			runtimeContent.erase(remove_if(
				runtimeContent.begin(),
				runtimeContent.end(),
				[&](T* c)
				{
					return c && c->GetWindowID() == windowID;
				}), runtimeContent.end());
					
			for (auto it = createdContent.begin(); it != createdContent.end();)
			{
				if (it->second->GetWindowID() == windowID) it = createdContent.erase(it);
				else ++it;
			}

			return "";
		}
	private:
		static inline unordered_map<u32, unique_ptr<T>> createdContent{};

		static inline vector<T*> runtimeContent{};
		static inline vector<T*> runtimeBatchAddContent{};
		static inline vector<T*> runtimeBatchRemoveContent{};
	};
}