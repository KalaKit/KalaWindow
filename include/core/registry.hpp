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
	using std::is_class_v;

	//Stores unique_ptrs and non-owning pointers of class T for ID-based lookups,
	//should always be stored as 'static inline Registry<T>'
	template<typename T>
		requires is_class_v<T>
	struct LIB_API Registry
	{
		//Owner container with ID as key
		static inline unordered_map<u32, unique_ptr<T>> createdContent{};
		//Runtime non-owning pointers container
		static inline vector<T*> runtimeContent{};

		//Returns true if the window owns the ID
		//Requires target class inside createdContent and runtimeContent
		//to have the 'u32 GetWindowID()' function.
		//Should not be used for externally created registries
		//because the Window class does not accept new IDs
		template<typename U = T>
			requires requires(U& u) { u.GetWindowID(); }
		static inline bool IsOwner(
			u32 windowID,
			u32 targetID)
		{
			auto it = createdContent.find(targetID);
			if (it == createdContent.end()) return false;

			return it->second
				&& it->second->GetWindowID() == windowID;
		}

		//Get non-owning value by ID
		static inline T* GetContent(u32 targetID)
		{
			auto it = createdContent.find(targetID);
			return it != createdContent.end()
				? it->second.get()
				: nullptr;
		}

		//Add a new unique ptr and its ID to the containers
		static inline bool AddContent(
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
		static inline bool RemoveContent(u32 targetID)
		{
			runtimeContent.erase(
				remove_if(runtimeContent.begin(), runtimeContent.end(),
					[&](T* p)
					{
						return p && p->GetID() == targetID;
					}),
				runtimeContent.end());

			createdContent.erase(targetID);

			return true;
		}
		//Remove content by non-owning pointer from containers
		static inline bool RemoveContent(T* targetPtr)
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

			runtimeContent.erase(remove(
				runtimeContent.begin(),
				runtimeContent.end(),
				targetPtr),
				runtimeContent.end());

			for (auto it = createdContent.begin(); it != createdContent.end(); ++it)
			{
				if (it->second.get() == targetPtr)
				{
					createdContent.erase(it);
					break;
				}
			}

			return true;
		}

		//Get all content as non-owning pointers by window ID from containers.
		//Requires target class inside createdContent and runtimeContent
		//to have the 'u32 GetWindowID()' function.
		//Should not be used for externally created registries
		//because the Window class does not accept new IDs
		template<typename U = T>
			requires requires(U& u) { u.GetWindowID(); }
		static inline vector<T*> GetAllWindowContent(u32 windowID)
		{
			vector<T*> out{};

			for (const auto& v : runtimeContent)
			{
				if (v->GetWindowID() == windowID) out.push_back(v);
			}

			return out;
		}

		//Remove all content by window ID from containers.
		//Requires target class inside createdContent and runtimeContent
		//to have the 'u32 GetWindowID()' function.
		//Should not be used for externally created registries
		//because the Window class does not accept new IDs
		template<typename U = T>
			requires requires(U& u) { u.GetWindowID(); }
		static inline void RemoveAllWindowContent(u32 windowID)
		{
			runtimeContent.erase(remove_if(
				runtimeContent.begin(),
				runtimeContent.end(),
				[&](T* c)
				{
					return c && c->GetWindowID() == windowID;
				}), runtimeContent.end());

			for (auto it = createdContent.begin(); it != createdContent.end();)
			{
				if (it->second->GetWindowID() == windowID)
				{
					it = createdContent.erase(it);
				}
				else ++it;
			}
		}

		//Clear all content from containers
		static inline void RemoveAllContent()
		{
			createdContent.clear();
			runtimeContent.clear();
		}
	};
}