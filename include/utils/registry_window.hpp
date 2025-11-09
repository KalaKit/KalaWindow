//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "KalaHeaders/core_utils.hpp"
		
namespace KalaWindow::Utils
{
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;
	using std::remove_if;
	
	using u32 = uint32_t;
	
	//Returns true if the window owns the ID
	//Requires target class inside createdContent and runtimeContent
	//to have the 'u32 GetWindowID()' function.
	//Should not be used for externally created registries
	//because the Window class does not accept new IDs
	template<typename T>
		requires requires(T& t) { t.GetWindowID(); }
	inline bool LIB_API IsOwner(
		const unordered_map<u32, unique_ptr<T>>& createdContent,
		u32 windowID,
		u32 targetID)
	{
		auto it = createdContent.find(targetID);
		if (it == createdContent.end()) return false;

		return it->second
			&& it->second->GetWindowID() == windowID;
	}
		
	//Get all content as non-owning pointers by window ID from containers.
	//Requires target class inside createdContent and runtimeContent
	//to have the 'u32 GetWindowID()' function.
	//Should not be used for externally created registries
	//because the Window class does not accept new IDs
	template<typename T>
		requires requires(T& t) { t.GetWindowID(); }
	inline vector<T*> LIB_API GetAllWindowContent(
		const vector<T*>& runtimeContent,
		u32 windowID)
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
	template<typename T>
		requires requires(T& t) { t.GetWindowID(); }
	inline void LIB_API RemoveAllWindowContent(
		unordered_map<u32, unique_ptr<T>>& createdContent,
		vector <T*>& runtimeContent,
		u32 windowID)
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
}