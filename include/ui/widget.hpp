//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <array>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "core/input.hpp"

namespace KalaWindow::UI
{
	using std::string;
	using std::vector;
	using std::clamp;
	using std::function;
	using std::array;

	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
	using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
	using KalaWindow::Core::MouseButton;

	enum class HitTarget
	{
		//uses the widgets own size and vertices to calculate hit testing
		HIT_QUAD,

		//uses the attached texture as the hit test,
		//defaults to quad if no texture is attached
		HIT_TEXTURE
	};
	enum class AnchorPoint
	{
		ANCHOR_TOP_LEFT,
		ANCHOR_TOP_RIGHT,
		ANCHOR_BOTTOM_LEFT,
		ANCHOR_BOTTOM_RIGHT,
		ANCHOR_CENTER
	};

	constexpr u16 MAX_Z_ORDER = 1024u;

	class LIB_API Widget
	{
	public:
		//Returns all hit widgets in 2D or 3D space sorted by highest Z first.
		//If all three values are empty then hit test is dealt in 2D space by mouse cursor position.
		//  - origin: where ray starts
		//  - target: where ray ends
		//  - distance: how far ray should cast
		static vector<Widget*> HitWidgets(
			u32 windowID,
			const vec3& origin = vec3(0),
			const vec3& target = vec3(0),
			float distance = 0.0f);

		//
		// CORE
		//

		inline bool IsInitialized() const { return isInitialized; }

		//Toggle between 2D and 3D state, sets to 2D if true.
		//Delinks parent and children that don't match the new coordinate state
		inline void Set2DState(bool newValue)
		{ 
			if (parent
				&& parent->is2D != newValue)
			{
				RemoveParent();
			}

			function<void(Widget*)> RemoveInvalidChildren;

			RemoveInvalidChildren = [&](Widget* w)
				{
					vector<Widget*> copy = w->children;
					for (Widget* c : copy)
					{
						RemoveInvalidChildren(c);
						if (c->is2D != newValue) w->RemoveChildByWidget(c);
					}
				};

			RemoveInvalidChildren(this);

			is2D = newValue;
		}
		inline bool Is2D() const { return is2D; }

		//Core render function for all widget systems, must be overridden per inherited widget.
		//Pass mat4(1.0f) to view and pass 2D projection as ortho(0.0f, windowWidth, windowHeight, 0.0f)
		//if you want 2D UI, otherwise this widget element is drawn in 3D space
		virtual bool Render(
			const mat4& view,
			const mat4& projection) = 0;

		//No children render past this widget size if true
		inline void SetClippingState(bool newValue) { isClipping = newValue; }
		//No children render past this widget size if true
		inline bool IsClipping() const { return isClipping; }

		inline u32 GetID() const { return ID; }

		inline u32 GetWindowID() const { return windowID; }

		inline void SetName(const string& newName)
		{
			if (!newName.empty()
				&& newName.length() <= 50
				&& newName != name)
			{
				name = newName;
			}
		}
		inline const string& GetName() const { return name; }

		//
		// Z ORDER
		//

		//Makes this widget Z order 1 unit higher than target widget
		inline void MoveAbove(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized
				|| targetWidget->is2D != is2D)
			{
				return;
			}

			u16 targetZOrder = targetWidget->zOrder;

			//skip if target z order already is max
			if (targetZOrder == MAX_Z_ORDER) return;

			zOrder = ++targetZOrder;
		}
		//Makes this widget Z order 1 unit lower than target widget
		inline void MoveBelow(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized
				|| targetWidget->is2D != is2D)
			{
				return;
			}

			u16 targetZOrder = targetWidget->zOrder;

			//skip if target z order already is 0
			if (targetZOrder == 0) return;

			zOrder = --targetZOrder;
		}

		inline void SetZOrder(u16 newZOrder)
		{
			u16 clamped = clamp(newZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = clamped;
		}
		inline u16 GetZOrder() const { return zOrder; }

		//
		// TRANSFORM
		//

		inline void SetPos(const vec3& newPos)
		{
			float clampedX = clamp(newPos.x, -10000.0f, 10000.0f);
			float clampedY = clamp(newPos.y, -10000.0f, 10000.0f);
			float clampedZ = clamp(newPos.z, -10000.0f, 10000.0f);

			pos = vec3(clampedX, clampedY, clampedZ);
		}
		//Get true position, unaffected by anchor
		inline const vec3& GetPos() const { return pos; }

		inline void SetAnchor(AnchorPoint newValue) { anchorPoint = newValue; }
		//Get position affected by anchor target if anchored to one of the corners
		inline const vec3& GetAnchorPos() const { return anchorPos; }

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

			UpdateAABB();
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

			UpdateAABB();
		}
		inline const quat& GetRotQuat() const { return rotQuat; }

		inline void SetSize(const vec3& newValue)
		{
			float clampedX = clamp(newValue.x, 0.01f, 10000.0f);
			float clampedY = clamp(newValue.y, 0.01f, 10000.0f);
			float clampedZ = clamp(newValue.z, 0.01f, 10000.0f);

			size = vec3(clampedX, clampedY, clampedZ);

			UpdateAABB();
		}
		inline const vec3& GetSize() const { return size; };

		inline void SetVerticeValue(u8 index, const vec3& newValue)
		{
			if (index > 3) return;

			float clampedX = clamp(newValue.x, -10.000f, 10.000f);
			float clampedY = clamp(newValue.y, -10.000f, 10.000f);
			float clampedZ = clamp(newValue.z, -10.000f, 10.000f);

			vertices[index] = vec3(clampedX, clampedY, clampedZ);

			UpdateAABB();
		}
		inline const vec3& GetVerticeValue(u8 index)
		{
			static const vec3 empty{};

			return index <= 3 ? vertices[index] : empty;
		}

		inline void SetVertices(const array<vec3, 4>& newValue)
		{
			array<vec3, 4> clamped = newValue;

			for (auto& v : clamped) 
			{
				float clampedX = clamp(v.x, -10.000f, 10.000f);
				float clampedY = clamp(v.y, -10.000f, 10.000f);
				float clampedZ = clamp(v.z, -10.000f, 10.000f);

				v = vec3(clampedX, clampedY, clampedZ);
			}

			vertices = clamped;

			UpdateAABB();
		}
		inline const array<vec3, 4>& GetVertices() const { return vertices; };
		inline const array<u8, 6>& GetIndices() const { return indices; }

		//Called automatically when rotation, size or vertices are updated
		inline void UpdateAABB()
		{
			vec3 boxMin = vertices[0];
			vec3 boxMax = vertices[0];

			for (int i = 0; i < 4; ++i)
			{
				boxMin.x = min(boxMin.x, vertices[i].x);
				boxMin.y = min(boxMin.y, vertices[i].y);
				boxMin.z = min(boxMin.z, vertices[i].z);

				boxMax.x = max(boxMax.x, vertices[i].x);
				boxMax.y = max(boxMax.y, vertices[i].y);
				boxMax.z = max(boxMax.z, vertices[i].z);
			}

			aabb = { boxMin, boxMax };
		}
		inline const array<vec3, 2>& GetAABB() const { return aabb; }

		inline mat4 GetWorldMatrix() const
		{
			mat4 m(1.0f);

			m = translate(m, pos);
			m *= mat4_cast(rotQuat);
			m = scale(m, size);

			return m;
		}

		//
		// INTERACTION
		//

		//Skip hit testing if true
		inline void SetInteractableState(bool newValue) { isInteractable = newValue; }
		//Skip hit testing if true
		inline bool IsInteractable() const { return isInteractable; }

		//If the cursor is over this widget and this widget is not
		//covered entirely or partially by another widget then this returns true.
		//If all three values are empty then hit test is dealt in 2D space by mouse cursor position.
		//  - origin: where ray starts
		//  - target: where ray ends
		//  - distance: how far ray should cast
		bool IsHovered(
			const vec3& origin = vec3(0),
			const vec3& target = vec3(0),
			float distance = 0.0f) const;

		inline void SetMousePressedFunction(const function<void()>& newValue)
		{
			if (newValue) function_mouse_pressed = newValue;
		}
		inline void RunMousePressedFunction() 
		{ 
			if (function_mouse_pressed) function_mouse_pressed();
		}

		inline void SetMouseReleasedFunction(const function<void()>& newValue)
		{
			if (newValue) function_mouse_released = newValue;
		}
		inline void RunMouseReleasedFunction()
		{
			if (function_mouse_released) function_mouse_released();
		}

		inline void SetMouseHeldFunction(const function<void()>& newValue)
		{
			if (newValue) function_mouse_held = newValue;
		}
		inline void RunMouseHeldFunction()
		{
			if (function_mouse_held) function_mouse_held();
		}

		inline void SetMouseHoveredFunction(const function<void()>& newValue)
		{
			if (newValue) function_mouse_hovered = newValue;
		}
		inline void RunMouseHoveredFunction()
		{
			if (function_mouse_hovered) function_mouse_hovered();
		}

		inline void SetMouseDraggedFunction(const function<void()>& newValue)
		{
			if (newValue) function_mouse_dragged = newValue;
		}
		inline void RunMouseDraggedFunction()
		{
			if (function_mouse_dragged) function_mouse_dragged();
		}

		inline void SetMouseScrolledFunction(const function<void()>& newValue)
		{
			if (newValue) function_mouse_scrolled = newValue;
		}
		inline void RunMouseScrolledFunction()
		{
			if (function_mouse_scrolled) function_mouse_scrolled();
		}

		//
		// GRAPHICS
		//

		inline void SetNormalizedColor(const vec3& newValue)
		{
			float clampX = clamp(newValue.x, 0.0f, 1.0f);
			float clampY = clamp(newValue.y, 0.0f, 1.0f);
			float clampZ = clamp(newValue.z, 0.0f, 1.0f);

			color = vec3(clampX, clampY, clampZ);
		}
		inline void SetRGBColor(const vec3& newValue)
		{
			int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
			int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
			int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

			float normalizedX = static_cast<float>(clampX) / 255;
			float normalizedY = static_cast<float>(clampY) / 255;
			float normalizedZ = static_cast<float>(clampZ) / 255;

			color = vec3(normalizedX, normalizedY, normalizedZ);
		}

		inline const vec3& GetNormalizedColor() const { return color; }
		inline vec3 GetRGBColor() const
		{
			int rgbX = static_cast<int>(color.x * 255);
			int rgbY = static_cast<int>(color.y * 255);
			int rgbZ = static_cast<int>(color.z * 255);

			return vec3(rgbX, rgbY, rgbZ);
		}

		inline void SetOpacity(float newValue)
		{
			float clamped = clamp(newValue, 0.0f, 1.0f);
			opacity = clamped;
		}
		inline float GetOpacity() const { return opacity; }

		inline u32 GetVAO() const { return VAO; }
		inline u32 GetVBO() const { return VBO; }
		inline u32 GetEBO() const { return EBO; }

		inline void SetShader(OpenGL_Shader* newShader)
		{
			if (newShader
				&& shader != newShader)
			{
				shader = newShader;
			}
		}
		inline void ClearShader() { shader = nullptr; }
		inline const OpenGL_Shader* GetShader() const { return shader; }

		inline void SetTexture(OpenGL_Texture* newTexture)
		{
			if (newTexture
				&& texture != newTexture)
			{
				texture = newTexture;
			}
		}
		inline void ClearTexture() { texture = nullptr; }
		inline const OpenGL_Texture* GetTexture() const { return texture; }

		//
		// PARENT-CHILD HIERARCHY
		//

		//Returns the top-most widget of this widget
		inline Widget* GetRoot() { return parent ? parent->GetRoot() : this; }

		inline bool HasParent() { return parent; }

		//Assigning a new parent clears this from old parent children
		inline void SetParent(Widget* newParent) 
		{
			if (!newParent
				|| newParent == this
				|| !newParent->isInitialized
				|| newParent->is2D != is2D
				|| (parent
				&& (parent == newParent
				|| parent->HasChildByWidget(this, true))))
			{
				return;
			}

			//set this widget parent
			parent = newParent;
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

		//Returns true if this has the selected child with optional recursive calls
		inline bool HasChildByWidget(
			const Widget* child,
			bool recursive = false)
		{
			if (!child
				|| child == this
				|| !child->isInitialized
				|| child->is2D != is2D)
			{
				return false;
			}

			if (!recursive)
			{
				return find(
					children.begin(),
					children.end(),
					child) != children.end();
			}

			for (const auto& c : children)
			{
				if (!c || !c->isInitialized)
				{
					continue;
				}

				if (c == child) return true;

				if (c->HasChildByWidget(child, true)) return true;
			}

			return false;
		}
		//Returns true if this has a child with the selected ID with optional recursive calls
		inline bool HasChildByID(
			u32 childID,
			bool recursive = false)
		{
			//skip if this has no children
			if (children.empty()) return false;

			for (const auto& c : children)
			{
				if (!c || !c->isInitialized) continue;

				if (c->ID == childID) return true;

				if (recursive
					&& c->HasChildByID(childID, true))
				{
					return true;
				}
			}

			return false;
		}
		//Returns true if this has a child at the selected index, does not accept recursive calls
		inline bool HasChildByIndex(u32 childIndex)
		{
			return !children.empty()
				&& childIndex < children.size()
				&& children[childIndex]
				&& children[childIndex]->isInitialized
				&& children[childIndex]->is2D == is2D;
		}

		inline void AddChild(Widget* newChild)
		{
			if (!newChild
				|| newChild == this
				|| !newChild->isInitialized
				|| HasChildByWidget(newChild)
				|| newChild->is2D != is2D)
			{
				return;
			}

			//add new child
			children.push_back(newChild);
			//set this as new child parent
			newChild->parent = this;
		}

		inline void RemoveChildByWidget(Widget* child)
		{
			if (!child
				|| child == this
				|| !child->isInitialized
				|| !HasChildByWidget(child)
				|| child->is2D != is2D)
			{
				return;
			}

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
			if (children.empty()) return;

			for (auto& c : children) { c->RemoveParent(); }
			children.clear(); 
		}

		inline Widget* GetChildByID(u32 childID)
		{
			//skip if this has no children
			if (children.empty()
				|| !HasChildByID(childID))
			{
				return nullptr;
			}

			for (const auto& c : children)
			{
				if (c->ID == childID) return c;
			}

			return nullptr;
		}
		inline Widget* GetChildByIndex(u32 childIndex)
		{
			if (children.empty()
				|| childIndex >= children.size())
			{
				return nullptr;
			}

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
		bool is2D = true;

		bool isClipping{};
		bool isHovered{};

		string name = "NO_NAME_ADDED";

		u32 ID{};
		u32 windowID{};

		vec3 pos{};
		vec3 anchorPos{};
		vec3 rotVec{};
		quat rotQuat{};
		vec3 size{};

		array<vec3, 4> vertices{};
		const array<u8, 6> indices =
		{
			0, 1, 2,
			2, 3, 0
		};

		array<vec3, 2> aabb{};

		HitTarget hitTarget{};
		AnchorPoint anchorPoint{};

		u16 zOrder{};

		bool isInteractable = true;
		function<void()> function_mouse_pressed{};
		function<void()> function_mouse_released{};
		function<void()> function_mouse_held{};
		function<void()> function_mouse_hovered{};
		function<void()> function_mouse_dragged{};
		function<void()> function_mouse_scrolled{};

		vec3 color = vec3(1.0f);
		float opacity = 1.0f;

		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		OpenGL_Shader* shader{};
		OpenGL_Texture* texture{};

		Widget* parent{};
		vector<Widget*> children{};
	};
}