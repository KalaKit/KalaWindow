//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

#include "graphics/opengl/opengl_shader.hpp"

namespace KalaWindow::UI
{
	constexpr u16 MAX_Z_ORDER = 1024u;

	using std::string;
	using std::vector;
	using std::clamp;

	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;

	class LIB_API Widget
	{
	public:
		inline bool IsInitialized() const { return isInitialized; }

		//Core render function for all widget systems, must be overridden per inherited widget.
		//Pass mat4(1.0f) to view and pass 2D projection as ortho(0.0f, windowWidth, windowHeight, 0.0f)
		//if you want 2D UI, otherwise this widget element is drawn in 3D space
		virtual bool Render(
			const mat4& view,
			const mat4& projection) = 0;

		inline u32 GetID() const { return ID; }

		//Swapping a window ID at runtime delinks this widget from its parent and its children
		void SetWindowID(u32 newID);
		inline u32 GetWindowID() const { return windowID; }

		inline void SetZOrder(u16 newZOrder)
		{
			u16 clamped = clamp(newZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = clamped;
		}
		inline u16 GetZOrder() const { return zOrder; }

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

		inline void SetPos(const vec3& newPos)
		{
			float clampedX = clamp(newPos.x, -10000.0f, 10000.0f);
			float clampedY = clamp(newPos.y, -10000.0f, 10000.0f);
			float clampedZ = clamp(newPos.z, -10000.0f, 10000.0f);

			pos = vec3(clampedX, clampedY, clampedZ);
		}
		inline const vec3& GetPos() const { return pos; }

		inline void SetRotVec(const vec3& newRot)
		{
			vec3 clamped(
				clamp(newRot.x, -359.99f, 359.99f),
				clamp(newRot.y, -359.99f, 359.99f),
				clamp(newRot.z, -359.99f, 359.99f));

			rotVec = vec3(clamped.x, clamped.y, clamped.z);

			quat qx = angleAxis(radians(clamped.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(clamped.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(clamped.z), vec3(0, 0, 1));

			rotQuat = qz * qy * qx;
		}
		inline const vec3& GetRotVec() const { return rotVec; }

		inline void SetRotQuat(const quat& newRot)
		{
			vec3 eulerDeg = degrees(eulerAngles(newRot));

			vec3 clamped(
				clamp(eulerDeg.x, -359.99f, 359.99f),
				clamp(eulerDeg.y, -359.99f, 359.99f),
				clamp(eulerDeg.z, -359.99f, 359.99f));

			rotVec = clamped;

			quat qx = angleAxis(radians(clamped.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(clamped.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(clamped.z), vec3(0, 0, 1));

			rotQuat = qz * qy * qx;
		}
		inline const quat& GetRotQuat() const { return rotQuat; }

		inline void SetSize(const vec3& newScale)
		{
			float clampedX = clamp(newScale.x, 0.01f, 1000.0f);
			float clampedY = clamp(newScale.y, 0.01f, 1000.0f);
			float clampedZ = clamp(newScale.z, 0.01f, 1000.0f);

			size = vec3(clampedX, clampedY, clampedZ);
		}
		inline const vec3& GetSize() const { return size; };

		inline mat4 GetWorldMatrix() const
		{
			mat4 m(1.0f);

			m = translate(m, pos);
			m *= mat4_cast(rotQuat);
			m = scale(m, size);

			return m;
		}

		inline u32 GetVAO() const { return VAO; }
		inline u32 GetVBO() const { return VBO; }
		inline u32 GetEBO() const { return EBO; }

		inline void SetShader(OpenGL_Shader* newShader)
		{
			shader = newShader;
		}
		inline const OpenGL_Shader* GetShader() const { return shader; }

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

		inline Widget* GetParent() 
		{ 
			//skip if parent never even existed
			if (!parent) return nullptr;

			return parent; 
		}

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

		inline void RemoveAllChildren()
		{
			//skip if this has no children
			if (children.empty()) return;

			children.clear();
		}

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

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Widget();
	protected:
		bool isInitialized{};

		string name{};

		u32 ID{};
		u32 windowID{};

		u16 zOrder{};

		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		vec3 pos{};
		vec3 rotVec{};
		quat rotQuat{};
		vec3 size{};

		OpenGL_Shader* shader{};

		Widget* parent{};
		vector<Widget*> children{};
	};
}