// string_utils.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Requirements: 
//   - You must name each Registry struct as 'registry' so hierarchy can access its siblings, 
//     this is a limit of C++ templates so there is no way around it
//
// Provides:
//   - parent-child hierarchy management
//   - fast lookup through recursive traversal across parents, children and siblings
//   - ownership and access for handling classes and structs at runtime
//------------------------------------------------------------------------------

#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>

namespace KalaHeaders
{
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;
	using std::make_unique;
	using std::find;
	using std::remove;
	using std::remove_if;
	using std::is_class_v;
	
	using u32 = uint32_t;
	
	//Stores local non-owning pointers per T instance inside the Registry struct
	template<typename T>
		requires is_class_v<T>
	struct Hierarchy
	{
		T* thisObject{};
		T* parent{};
		vector<T*> children{};
		
		//Returns the top-most parent of this target
		inline T* GetRoot() { return parent ? T::registry.hierarchy[parent].GetRoot() : this; }

		//Returns true if target target is connected
		//to current target as a child, parent or sibling.
		//Set recursive to true if you want deep target search
		inline bool HasTarget(
			T* targetObject,
			bool recursive = false)
		{
			if (!thisObject
				|| !targetObject)
			{
				return false;
			}

			if (thisObject == targetObject) return true;

			//check descendants
			for (auto* c : children)
			{
				if (c == targetObject) return true;

				if (recursive
					&& T::registry.hierarchy[c].HasTarget(targetObject, true))
				{
					return true;
				}
			}

			//check ancestors
			if (parent)
			{
				if (parent == targetObject) return true;

				if (recursive
					&& T::registry.hierarchy[parent].HasTarget(targetObject, true))
				{
					return true;
				}
			}

			return false;
		}
		
		inline bool IsParent(
			T* targetObject,
			bool recursive = false)
		{
			if (!thisObject
				|| !targetObject
				|| thisObject == targetObject)
			{
				return false;
			}

			if (!parent) return false;

			if (parent == targetObject) return true;

			if (recursive
				&& T::registry.hierarchy[parent].IsParent(targetObject, true))
			{
				return true;
			}

			return false;
		}
		inline T* GetParent() { return parent; }
		inline bool SetParent(T* targetObject)
		{
			if (!thisObject
				|| !targetObject
				|| targetObject == thisObject
				|| HasTarget(targetObject, true)
				|| T::registry.hierarchy[targetObject].HasTarget(thisObject, true)
				|| (parent
				&& (parent == targetObject
				|| T::registry.hierarchy[parent].HasTarget(thisObject, true))))
			{
				return false;
			}
			
			//remove existing parent if it exists
			if (parent) RemoveParent();

			//set this target parent
			parent = targetObject;
			//add this as new child to parent
			T::registry.hierarchy[parent].children.push_back(thisObject);

			return true;
		}
		inline bool RemoveParent()
		{
			//skip if parent never even existed
			if (!thisObject
				|| !parent)
			{
				return false;
			}

			vector<T*>& parentChildren = T::registry.hierarchy[parent].children;

			parentChildren.erase(remove(
				parentChildren.begin(),
				parentChildren.end(),
				thisObject),
				parentChildren.end());

			parent = nullptr;

			return true;
		}
		
		inline bool IsChild(
			T* targetObject,
			bool recursive = false)
		{
			if (!thisObject
				|| !targetObject
				|| thisObject == targetObject)
			{
				return false;
			}

			for (auto* c : children)
			{
				if (c == targetObject) return true;

				if (recursive
					&& T::registry.hierarchy[c].IsChild(targetObject, true))
				{
					return true;
				}
			}

			return false;
		}
		inline bool AddChild(T* targetObject)
		{
			if (!thisObject
				|| !targetObject
				|| targetObject == thisObject
				|| HasTarget(targetObject, true)
				|| T::registry.hierarchy[targetObject].HasTarget(thisObject, true))
			{
				return false;
			}

			children.push_back(targetObject);
			T::registry.hierarchy[targetObject].parent = thisObject;

			return true;
		}
		inline bool RemoveChild(
			T* targetObject,
			bool isDestructive = false)
		{
			if (!thisObject
				|| !targetObject
				|| targetObject == thisObject
				|| (parent
				&& targetObject == parent))
			{
				return false;
			}

			if (T::registry.hierarchy[targetObject].parent)
			{
				T::registry.hierarchy[targetObject].parent = nullptr;
			}

			if (isDestructive) T::registry.RemoveContent(targetObject, true);

			children.erase(remove(
				children.begin(),
				children.end(),
				targetObject),
				children.end());

			return true;
		}

		inline const vector<T*>& GetAllChildren() 
		{
			static const vector<T*> empty{};
			return thisObject ? children : empty;
		}
		inline void RemoveAllChildren(bool isDestructive = false)
		{
			if (!thisObject) return;

			for (auto* c : children)
			{
				T::registry.hierarchy[c].parent = nullptr;
				
				if (isDestructive) T::registry.RemoveContent(c, true);
			}				
			
			children.clear();
		}
	};

	//Stores unique_ptrs and non-owning pointers of class T for ID-based lookups,
	//should always be stored as 'static inline Registry<T> registry'
	template<typename T>
		requires is_class_v<T>
	struct Registry
	{
		//Owner registry with ID as key
		static inline unordered_map<u32, unique_ptr<T>> createdContent{};
		//Runtime non-owning pointers
		static inline vector<T*> runtimeContent{};
		//Hierarchy content for storing parent-child relations per instance of this class
		static inline unordered_map<T*, Hierarchy<T>> hierarchy{};

		//Get non-owning value by ID
		static inline T* GetContent(u32 targetID)
		{
			auto it = createdContent.find(targetID);
			return it != createdContent.end()
				? it->second.get()
				: nullptr;
		}

		//Add a new unique ptr and its ID
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
			
			//add hierarchy node
			hierarchy[raw] = Hierarchy<T>{};
			hierarchy[raw].thisObject = raw;

			return true;
		}

		//Remove content by ID
		static inline bool RemoveContent(u32 targetID)
		{
			T* targetPtr{};
			
			auto it = createdContent.find(targetID);
			if (it != createdContent.end()) targetPtr = it->second.get();
			
			//erase from hierarchy
			if (targetPtr)
			{
				hierarchy.erase(targetPtr);
				
				for (auto& [obj, node] : hierarchy)
				{
					node.children.erase(remove(
						node.children.begin(),
						node.children.end(),
						targetPtr),
						node.children.end());
						
					if (node.parent == targetPtr) node.parent = nullptr;
				}	
			}
			
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
		//Remove content by non-owning pointer
		static inline bool RemoveContent(
			T* targetPtr,
			bool removedViaHierarchy = false)
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
			
			if (!removedViaHierarchy)
			{
				//erase from hierarchy
				hierarchy.erase(targetPtr);
				
				for (auto& [obj, node] : hierarchy)
				{
					node.children.erase(remove(
						node.children.begin(),
						node.children.end(),
						targetPtr),
						node.children.end());
						
					if (node.parent == targetPtr) node.parent = nullptr;
				}	
			}

			runtimeContent.erase(remove(
				runtimeContent.begin(),
				runtimeContent.end(),
				targetPtr),
				runtimeContent.end());

			if (createdContent.contains(targetPtr->GetID()))
			{
				createdContent.erase(targetPtr->GetID());
			}

			return true;
		}

		static inline void RemoveAllContent()
		{
			hierarchy.clear();
			createdContent.clear();
			runtimeContent.clear();
		}
	};
}