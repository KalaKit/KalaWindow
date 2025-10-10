//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "graphics/opengl/opengl_shader.hpp"

namespace KalaWindow::UI
{
	using std::string;
	using std::vector;

	using KalaHeaders::Log;
	using KalaHeaders::LogType;

	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;

	class LIB_API WidgetManager
	{
	public:
		//Initializes the global widget system reused across all windows.
		//Also initializes the quad and text shaders
		static void Initialize();

		static inline OpenGL_Shader* GetQuadShader()
		{
			return shader_quad;
		}
		static inline OpenGL_Shader* GetTextShader()
		{
			return shader_text;
		}
	private:
		static inline OpenGL_Shader* shader_quad{};
		static inline OpenGL_Shader* shader_text{};
	};

	class LIB_API Widget
	{
	public:
		static Widget* Initialize(const string& name);
		inline bool IsInitialized() const { return isInitialized; }

		inline void SetName(const string& newName)
		{
			//skip if name is empty
			if (newName.empty()) return;
			//skip if name is too long
			if (newName.length() > 50) return;
			//skip if name is already same
			if (newName == name) return;

			name = newName;
		}
		inline const string& GetName() { return name; }

		inline u32 GetID() const { return ID; }

		inline void SetParent(Widget* newParent) 
		{
			//skip if parent is nullptr
			if (!newParent) return;
			//skip if parent is this
			if (newParent == this) return;
			//skip if parent is not initialized
			if (!newParent->isInitialized) return;
			//skip if existing parent is already the same
			if (parent
				&& parent->ID == newParent->ID)
			{
				return;
			}

			bool alreadyExists{};

			//skip if one of the children of this widget already has the same ID
			for (const auto& c : children)
			{
				if (c->ID == newParent->ID)
				{
					alreadyExists = true;
					break;
				}
			}

			if (alreadyExists) return;

			//set this widget parent
			parent = newParent;

			//skip adding this as new child to parent if it already has this as child
			for (const auto& c : parent->children)
			{
				if (c->ID == ID)
				{
					alreadyExists = true;
					break;
				}
			}

			//this is still fine because it just means parent already has this child
			if (alreadyExists) return;

			//add this as new child to parent
			parent->children.push_back(this);
		}

		inline void RemoveParent()
		{
			//skip if parent never even existed
			if (!parent) return;

			vector<Widget*>& parentChildren = parent->children;

			parentChildren.erase(remove(
				parentChildren.begin(),
				parentChildren.end(),
				this),
				parentChildren.end());

			parent = nullptr;
		}

		inline Widget* GetParent() { return parent; }

		//Returns true if this has the selected child
		inline bool HasChildByWidget(Widget* child)
		{
			//skip if child is nullptr
			if (!child) return false;
			//skip if this and child are the same
			if (this == child) return false;
			//skip if child is not initialized
			if (!child->IsInitialized()) return false;

			return find(
				children.begin(),
				children.end(),
				child) 
				!= children.end();
		}
		//Returns true if this has a child with the selected ID
		inline bool HasChildByID(u32 childID)
		{
			//skip if this has no children
			if (children.empty()) return false;

			for (const auto& c : children)
			{
				if (c->ID == childID) return true;
			}

			return false;
		}
		//Returns true if this has a child at the selected index
		inline bool HasChildByIndex(u32 childIndex)
		{
			//skip if this is not initialized
			if (!isInitialized) return false;
			//skip if this has no children
			if (children.empty()) return false;
			//skip if child index is above children size
			if (childIndex >= children.size()) return false;

			return true;
		}

		inline void AddChild(Widget* newChild)
		{
			//skip if child is nullptr
			if (!newChild) return;
			//skip if child is this
			if (newChild == this) return;
			//skip if child is not initialized
			if (!newChild->isInitialized) return;
			//skip if this already has selected child
			if (HasChildByWidget(newChild)) return;

			//add new child
			children.push_back(newChild);
			//set this as new child parent
			newChild->parent = this;
		}

		inline void RemoveChildByWidget(Widget* child)
		{
			//skip if child is nullptr
			if (!child) return;
			//skip if child is this
			if (child == this) return;
			//skip if child is not initialized
			if (!child->isInitialized) return;

			if (child->parent) child->parent = nullptr;

			children.erase(remove(
				children.begin(),
				children.end(),
				child),
				children.end());
		}
		inline void RemoveChildByID(u32 childID)
		{
			Widget* child = GetChildByID(childID);

			if (!child) return;

			child->parent = nullptr;

			children.erase(remove(
				children.begin(),
				children.end(),
				child),
				children.end());
		}
		inline void RemoveChildByIndex(u32 childIndex)
		{
			Widget* child = GetChildByIndex(childIndex);

			if (!child) return;

			child->parent = nullptr;

			children.erase(remove(
				children.begin(),
				children.end(),
				child),
				children.end());
		}

		//Get child by widget ID
		inline Widget* GetChildByID(u32 childID)
		{
			//skip if this has no children
			if (children.empty()) return nullptr;

			for (const auto& c : children)
			{
				if (c->ID == childID) return c;
			}

			return nullptr;
		}
		//Get child by child index of this widgets children vector
		inline Widget* GetChildByIndex(u32 childIndex)
		{
			//skip if this is not initialized
			if (!isInitialized) return{};
			//skip if this has no children
			if (children.empty()) return nullptr;
			//skip if child index is above children size
			if (childIndex >= children.size()) return nullptr;

			return children[childIndex];
		}

		inline const vector<Widget*>& GetAllChildren() 
		{ 
			static const vector<Widget*> empty{};

			//skip if this is not initialized
			if (!isInitialized) return empty;
			//skip if this has no children
			if (children.empty()) return empty;

			return children; 
		}
	private:
		bool isInitialized{};

		string name{};
		u32 ID{};

		Widget* parent{};
		vector<Widget*> children{};
	};
}