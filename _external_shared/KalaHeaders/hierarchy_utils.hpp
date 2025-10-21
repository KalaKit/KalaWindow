//------------------------------------------------------------------------------
// hierarchy_utils.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
// 
// You must name each Hierarchy struct as 'hierarchy',
// this is a limit of C++ templates so there is no way around it.
//
// Provides:
//   - parent-child hierarchy management
//   - fast lookup through recursive traversal across parents, children and siblings
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <vector>

namespace KalaHeaders
{
	using std::is_class_v;
	using std::same_as;
	using std::vector;

	template<typename T>
		requires is_class_v<T>
	struct Hierarchy
	{
		T* parent{};
		vector<T*> children{};

		//Returns the top-most parent of this target
		inline T* GetRoot() { return parent ? parent->hierarchy.GetRoot() : this; }

		//Returns true if target target is connected
		//to current target as a child, parent or sibling.
		//Set recursive to true if you want deep target search
		inline bool HasTarget(
			T* thisObject,
			T* targetObject,
			bool recursive = false)
		{
			if (!targetObject) return false;

			if (thisObject == targetObject) return true;

			//check descendants
			for (auto* c : children)
			{
				if (c == targetObject) return true;

				if (recursive
					&& c->hierarchy.HasTarget(c, targetObject, true))
				{
					return true;
				}
			}

			//check ancestors
			if (parent)
			{
				if (parent == targetObject) return true;

				if (recursive
					&& parent->hierarchy.HasTarget(parent, targetObject, true))
				{
					return true;
				}
			}

			return false;
		}
		inline bool IsParent(
			T* thisObject,
			T* targetObject,
			bool recursive = false)
		{
			if (!targetObject
				|| thisObject == targetObject)
			{
				return false;
			}

			if (!parent) return false;

			if (parent == targetObject) return true;

			if (recursive
				&& parent->hierarchy.IsParent(targetObject, true))
			{
				return true;
			}

			return false;
		}
		inline T* GetParent() { return parent; }
		inline bool SetParent(
			T* thisObject,
			T* targetObject)
		{
			if (!targetObject
				|| targetObject == thisObject
				|| HasTarget(thisObject, targetObject, true)
				|| targetObject->hierarchy.HasTarget(targetObject, thisObject, true)
				|| (parent
				&& (parent == targetObject
				|| parent->hierarchy.HasTarget(parent, thisObject, true))))
			{
				return false;
			}

			//set this target parent
			parent = targetObject;
			//add this as new child to parent
			parent->hierarchy.children.push_back(thisObject);

			return true;
		}
		inline bool RemoveParent(T* thisObject)
		{
			//skip if parent never even existed
			if (!parent) return false;

			vector<T*>& parentChildren = parent->hierarchy.children;

			parentChildren.erase(remove(
				parentChildren.begin(),
				parentChildren.end(),
				thisObject),
				parentChildren.end());

			parent = nullptr;

			return true;
		}
		inline bool IsChild(
			T* thisObject,
			T* targetObject,
			bool recursive = false)
		{
			if (!targetObject
				|| thisObject == targetObject)
			{
				return false;
			}

			for (auto* c : children)
			{
				if (c == targetObject) return true;

				if (recursive
					&& c->hierarchy.IsChild(targetObject, true))
				{
					return true;
				}
			}

			return false;
		}
		inline bool AddChild(
			T* thisObject,
			T* targetObject)
		{
			if (!targetObject
				|| targetObject == thisObject
				|| HasTarget(thisObject, targetObject, true)
				|| targetObject->hierarchy.HasTarget(targetObject, thisObject, true))
			{
				return false;
			}

			children.push_back(targetObject);
			targetObject->hierarchy.parent = thisObject;

			return true;
		}
		inline bool RemoveChild(
			T* thisObject,
			T* targetObject)
		{
			if (!targetObject
				|| targetObject == thisObject
				|| (parent
				&& targetObject == parent))
			{
				return false;
			}

			if (targetObject->hierarchy.parent) targetObject->hierarchy.parent = nullptr;

			children.erase(remove(
				children.begin(),
				children.end(),
				targetObject),
				children.end());

			return true;
		}

		inline const vector<T*>& GetAllChildren() { return children; }
		inline void RemoveAllChildren()
		{
			for (auto* c : children) c->hierarchy.parent = nullptr;
			children.clear();
		}
	};
}