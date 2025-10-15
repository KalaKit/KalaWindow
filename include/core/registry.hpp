//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Core
{
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;
	using std::find;
	using std::remove;
	using std::remove_if;

	template<typename T>
	struct LIB_API Registry
	{
		static inline unordered_map<u32, unique_ptr<T>> createdContent{};
		static inline vector<T*> runtimeContent{};

		//Returns true if the window owns the ID
		static bool IsOwner(
			u32 windowID,
			u32 targetID)
		{
			auto it = createdContent.find(targetID);
			if (it == createdContent.end()) return false;

			return it->second
				&& it->second->GetWindowID() == windowID;
		}

		//Get non-owning value by ID
		static T* GetContent(u32 targetID)
		{
			auto it = createdContent.find(targetID);
			return it != createdContent.end()
				? it->second.get()
				: nullptr;
		}

		//Add a new unique ptr and its ID to the containers
		static bool AddContent(
			u32 targetID,
			unique_ptr<T> targetContent)
		{
			if (!targetContent
				|| targetID == 0
				|| createdContent.contains(targetID))
			{
				return false;
			}

			T* raw = targetContent.get();
			createdContent[targetID] = move(targetContent);
			runtimeContent.push_back(raw);

			return true;
		}

		//Remove content by ID from containers
		static bool RemoveContent(u32 targetID)
		{
			createdContent.erase(targetID);

			runtimeContent.erase(
				remove_if(runtimeContent.begin(), runtimeContent.end(),
					[&](T* p)
					{
						return p && p->GetID() == targetID;
					}));

			return true;
		}
		//Remove content by non-owning pointer from containers
		static bool RemoveContent(T* targetPtr)
		{
			if (!targetPtr) return false;

			//skip early if target ptr wasnt even found from runtime content vector
			if (find(runtimeContent.begin(),
				runtimeContent.end(),
				targetPtr)
				== runtimeContent.end())
			{
				return false;
			}

			for (auto it = createdContent.begin(); it != createdContent.end(); ++it)
			{
				if (it->second.get() == targetPtr)
				{
					createdContent.erase(it);
					break;
				}
			}

			runtimeContent.erase(remove(
				runtimeContent.begin(),
				runtimeContent.end(),
				targetPtr),
				runtimeContent.end());

			return true;
		}

		//Remove all content by window ID from containers
		static void RemoveAllWindowContent(u32 windowID)
		{
			for (auto it = createdContent.begin(); it != createdContent.end();)
			{
				if (it->second->GetWindowID() == windowID)
				{
					it = createdContent.erase(it);
				}
				else ++it;
			}

			runtimeContent.erase(remove_if(
				runtimeContent.begin(),
				runtimeContent.end(),
				[&](T* c)
				{
					return c && c->GetWindowID() == windowID;
				}), runtimeContent.end());
		}

		//Clear all content from containers
		static void RemoveAllContent()
		{
			createdContent.clear();
			runtimeContent.clear();
		}
	};
}