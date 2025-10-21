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
#include "core/input.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl_texture.hpp"

namespace KalaWindow::UI
{
	using std::string;
	using std::vector;
	using std::clamp;
	using std::function;
	using std::array;

	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
	using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
	using KalaWindow::Core::Input;
	using KalaWindow::Core::Key;
	using KalaWindow::Core::MouseButton;

	constexpr u16 MAX_Z_ORDER = 1024;

	enum class HitTarget
	{
		//uses the widgets own size and vertices to calculate hit testing
		HIT_QUAD,

		//uses the attached texture as the hit test,
		//defaults to quad if no texture is attached
		HIT_TEXTURE
	};

	enum class PosTarget
	{
		POS_WORLD,    //position relative to window center
		POS_LOCAL,    //position added on top of world position if the widget has a parent

		POS_COMBINED, //final position after combining world and local position
		POS_ORIGINAL  //original position before moving to window center
	};
	enum class RotTarget
	{
		ROT_WORLD,   //rotation of this widget
		ROT_LOCAL,   //rotation added on top of world rotation if the widget has a parent

		ROT_COMBINED //final rotation after combining world and local rotation
	};
	enum class SizeTarget
	{
		SIZE_WORLD,   //size of this widget
		SIZE_LOCAL,   //size added on top of world position if the widget has a parent

		SIZE_COMBINED //final position after combining world and local position
	};

	enum class ActionTarget
	{
		ACTION_PRESSED,  //pressed key or mouse button
		ACTION_RELEASED, //released key or mouse button
		ACTION_HELD,     //held key or mouse button
		ACTION_HOVERED,  //hovered cursor over widget
		ACTION_DRAGGED,  //held mouse button and moved mouse
		ACTION_SCROLLED  //used scrollwheel
	};

	struct Widget_Transform
	{
		vec2 worldPos{};     //position relative to window center
		vec2 localPos{};     //position added on top of world position if the widget has a parent
		vec2 combinedPos{};  //final position after combining world and local position
		vec2 originalPos{};  //original position before moving to window center

		f32 worldRot{};      //rotation of this widget
		f32 localRot{};      //rotation added on top of world rotation if the widget has a parent
		f32 combinedRot{};   //final rotation after combining world and local rotation

		vec2 worldSize{};    //size of this widget
		vec2 localSize{};    //size added on top of world position if the widget has a parent
		vec2 combinedSize{}; //final position after combining world and local position

		vec2 lastViewportSize{};    //Last frame viewport size, if it doesn't match current one then 'UpdateTransform()' is called
		vec2 viewportSize{};        //the size of the target window or viewport to render this widget inside of
		float baseHeight = 1080.0f; //used for affecting the Y position of the AABB relative to the viewport size

		const array<vec2, 4> vertices = 
		{
			vec2(-0.5f,  0.5f), //top-left
			vec2(0.5f,  0.5f),  //top-right
			vec2(0.5f, -0.5f),  //bottom-right
			vec2(-0.5f, -0.5f)  //bottom-left
		};
		const array<u8, 6> indices =
		{
			0, 1, 2,
			2, 3, 0
		};

		array<vec2, 2> aabb{};
	};

	struct Widget_Render
	{
		bool canUpdate = true;

		//no children render past this widget size if true
		bool isClipping{};

		vec3 color = vec3(1.0f);
		f32 opacity = 1.0f;

		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		OpenGL_Shader* shader{};
		OpenGL_Texture* texture{};
	};

	struct Widget_Event
	{
		function<void()> function_button_pressed{};
		Key keyPressed{};
		MouseButton mousePressed{};

		function<void()> function_button_released{};
		Key keyReleased{};
		MouseButton mouseReleased{};

		function<void()> function_button_held{};
		Key keyHeld{};
		MouseButton mouseHeld{};

		function<void()> function_mouse_dragged{};
		MouseButton mouseDragged{};

		function<void()> function_mouse_hovered{};
		function<void()> function_mouse_scrolled{};
	};

	class LIB_API Widget
	{
	public:
		//Returns all hit widgets at mouse position sorted by highest Z first
		static vector<Widget*> HitWidgets(u32 windowID);

		//
		// CORE
		//

		inline bool IsInitialized() const { return isInitialized; }

		//Render the widget. Pass viewport size so that the widget can be
		//positioned to the window center and offset from that with world pos
		virtual bool Render(
			const mat3& projection,
			const vec2 viewportSize) = 0;

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

		//Skips rendering if set to false without needing to
		//encapsulate the render function in its own render toggle
		inline void SetUpdateState(bool newValue) { render.canUpdate = newValue; }
		//Skips rendering if set to false without needing to
		//encapsulate the render function in its own render toggle
		inline bool CanUpdate() const { return render.canUpdate; }

		//No children render past this widget size if true
		inline void SetClippingState(bool newValue) { render.isClipping = newValue; }
		//No children render past this widget size if true
		inline bool IsClipping() const { return render.isClipping; }

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
		// TRANSFORM
		//

		inline void SetBaseHeight(f32 newValue)
		{
			f32 clamped = clamp(newValue, 1.0f, 4320.0f);
			transform.baseHeight = clamped;
		}
		inline f32 GetBaseHeight() const { return transform.baseHeight; }

		inline void SetPos(
			const vec2 newPos,
			PosTarget posTarget)
		{
			//cannot set combined or original pos
			if (posTarget == PosTarget::POS_COMBINED
				|| posTarget == PosTarget::POS_ORIGINAL)
			{
				return;
			}

			f32 clampedX = clamp(newPos.x, -10000.0f, 10000.0f);
			f32 clampedY = clamp(newPos.y, -10000.0f, 10000.0f);

			vec2 clampedPos = vec2(clampedX, clampedY);

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:  transform.worldPos  = clampedPos; break;
			case PosTarget::POS_LOCAL:  transform.localPos  = clampedPos; break;
			}

			UpdateTransform();
		}
		inline const vec2 GetPos(PosTarget posTarget) const 
		{ 
			static const vec2 empty{};

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:    return transform.worldPos; break;
			case PosTarget::POS_LOCAL:    return transform.localPos; break;
			case PosTarget::POS_COMBINED: return transform.combinedPos; break;
			case PosTarget::POS_ORIGINAL: return transform.originalPos; break;
			}

			return empty;
		}

		//Safely wraps within allowed bounds
		inline void AddRot(f32 deltaRot)
		{
			f32 angle = transform.worldRot + deltaRot;
			angle = fmodf(angle, 360.0f);
			if (angle < 0.0f) angle += 360.0f;
			transform.worldRot = angle;

			UpdateTransform();
		}

		inline void SetRot(
			const f32 newRot,
			RotTarget rotTarget)
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			f32 clamped = clamp(newRot, 0.0f, 359.99f);

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: transform.worldRot = clamped; break;
			case RotTarget::ROT_LOCAL: transform.localRot = clamped; break;
			}

			UpdateTransform();
		}
		inline const f32 GetRot(RotTarget rotTarget) const
		{
			static const f32 empty{};

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:    return transform.worldRot; break;
			case RotTarget::ROT_LOCAL:    return transform.localRot; break;
			case RotTarget::ROT_COMBINED: return transform.combinedRot; break;
			}

			return empty;
		}

		inline void SetSize(
			const vec2 newSize,
			SizeTarget sizetarget)
		{
			//cannot set combined size
			if (sizetarget == SizeTarget::SIZE_COMBINED) return;

			f32 clampedX = clamp(newSize.x, 0.01f, 10000.0f);
			f32 clampedY = clamp(newSize.y, 0.01f, 10000.0f);

			vec2 clampedSize = vec2(clampedX, clampedY);

			switch (sizetarget)
			{
			case SizeTarget::SIZE_WORLD: transform.worldSize = clampedSize; break;
			case SizeTarget::SIZE_LOCAL: transform.localSize = clampedSize; break;
			}

			UpdateTransform();
		}
		inline const vec2 GetSize(SizeTarget sizeTarget) const 
		{ 
			static const vec2 empty{};

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD:      return transform.worldSize; break;
			case SizeTarget::SIZE_LOCAL:      return transform.localSize; break;
			case SizeTarget::SIZE_COMBINED:   return transform.combinedSize; break;
			}

			return empty;
		};

		inline const array<vec2, 4>& GetVertices() const { return transform.vertices; };
		inline const array<u8, 6>& GetIndices() const { return transform.indices; }

		inline const array<vec2, 2>& GetAABB() 
		{ 
			UpdateAABB();
			return transform.aabb; 
		}

		//Called automatically when any pos, rot or size type is updated
		inline void UpdateTransform()
		{
			//
			// ROTATION
			//

			transform.combinedRot = parent
				? parent->transform.combinedRot + transform.worldRot + transform.localRot
			    : transform.worldRot;

			//
			// SIZE
			//

			transform.combinedSize = parent
				? transform.worldSize + transform.localSize
			    : transform.worldSize;

			//
			// POSITION
			//

			if (parent)
			{
				f32 rads = radians(parent->transform.combinedRot);
				mat2 rotMat =
				{
					{ cos(rads), sin(rads) },
					{ -sin(rads), cos(rads) }
				};

				vec2 rotOffset = rotMat * transform.localPos;
				transform.combinedPos = 
					parent->transform.combinedPos
					+ transform.originalPos
					+ transform.worldPos
					+ rotOffset;
			}
			else transform.combinedPos = transform.originalPos + transform.worldPos;

			UpdateAABB();
		}

		//
		// Z ORDER
		//

		//Makes this widget Z order 1 unit higher than target widget
		inline void MoveAbove(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized)
			{
				return;
			}

			u16 targetZOrder = targetWidget->zOrder;

			u16 newZOrder = clamp(++targetZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = newZOrder;
		}
		//Makes this widget Z order 1 unit lower than target widget
		inline void MoveBelow(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized)
			{
				return;
			}

			u16 targetZOrder = targetWidget->zOrder;

			//skip if target z order already is 0
			if (targetZOrder == 0) return;

			u16 newZOrder = clamp(--targetZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = newZOrder;
		}

		inline void SetZOrder(u16 newZOrder)
		{
			u16 clamped = clamp(newZOrder, static_cast<u16>(0), MAX_Z_ORDER);

			zOrder = clamped;
		}
		inline u16 GetZOrder() const { return zOrder; }

		//
		// INTERACTION
		//

		//Skip hit testing and event polling if true
		inline void SetInteractableState(bool newValue) { isInteractable = newValue; }
		//Skip hit testing and event polling if true
		inline bool IsInteractable() const { return isInteractable; }

		//If the cursor is over this widget and this widget is not
		//covered entirely or partially by another widget then this returns true
		bool IsHovered() const;

		//Accepts mouse buttons for pressed, released, held and dragged events.
		//Use 'SetMouseHoverEvent()' and 'SetMouseScrollEvent()' to assign those events
		inline void SetMouseEvent(
			const function<void()>& newValue,
			MouseButton mouseButton,
			ActionTarget actionTarget)
		{
			if (!newValue
				|| mouseButton == MouseButton::MouseButtonCount)
			{
				return;
			}

			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:
			{
				event.keyPressed = Key::Unknown;
				event.mousePressed = mouseButton;

				event.function_button_pressed = newValue;
				break;
			}
			case ActionTarget::ACTION_RELEASED:
			{
				event.keyReleased = Key::Unknown;
				event.mouseReleased = mouseButton;

				event.function_button_released = newValue;
				break;
			}
			case ActionTarget::ACTION_HELD:
			{
				event.keyHeld = Key::Unknown;
				event.mouseHeld = mouseButton;

				event.function_button_held = newValue;
				break;
			}
			case ActionTarget::ACTION_DRAGGED:
			{
				event.mouseDragged = mouseButton;

				event.function_mouse_dragged = newValue;
				break;
			}
			}
		}
		//Assigns mouse hovered event
		inline void SetMouseHoverEvent(const function<void()>& newValue)
		{
			if (!newValue) return;
			event.function_mouse_hovered = newValue;
		}
		//Assigns mouse scrolled event
		inline void SetMouseScrollEvent(const function<void()>& newValue)
		{
			if (!newValue) return;
			event.function_mouse_scrolled = newValue;
		}
		//Returns which mouse button is attached to what event, ignores hovered and scrolled events
		inline MouseButton GetMouseEventButton(ActionTarget actionTarget) const
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:  return event.mousePressed;
			case ActionTarget::ACTION_RELEASED: return event.mouseReleased;
			case ActionTarget::ACTION_HELD:     return event.mouseHeld;
			case ActionTarget::ACTION_DRAGGED:  return event.mouseDragged;
			}

			return MouseButton::Unknown;
		}

		//Accepts keyboard keys for pressed, released and held events, ignores all other events
		inline void SetKeyEvent(
			const function<void()>& newValue,
			Key key,
			ActionTarget actionTarget)
		{
			if (!newValue
				|| key == Key::KeyCount)
			{
				return;
			}

			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED: 	event.function_button_pressed = newValue; break;
			case ActionTarget::ACTION_RELEASED: event.function_button_released = newValue; break;
			case ActionTarget::ACTION_HELD:     event.function_button_held = newValue; break;
			}
		}
		//Returns which key is attached to what key event, ignores dragged, hovered and scrolled events
		inline Key GetKeyEventButton(ActionTarget actionTarget) const
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:  return event.keyPressed;
			case ActionTarget::ACTION_RELEASED: return event.keyReleased;
			case ActionTarget::ACTION_HELD:     return event.keyHeld;
			}

			return Key::Unknown;
		}

		//Clears target event function and its buttons
		inline void ClearEvent(ActionTarget actionTarget)
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:
			{
				event.keyPressed = Key::Unknown;
				event.mousePressed = MouseButton::Unknown;
				event.function_button_pressed = nullptr;

				break;
			}
			case ActionTarget::ACTION_RELEASED:
			{
				event.keyReleased = Key::Unknown;
				event.mouseReleased = MouseButton::Unknown;
				event.function_button_released = nullptr;

				break;
			}
			case ActionTarget::ACTION_HELD:
			{
				event.keyHeld = Key::Unknown;
				event.mouseHeld = MouseButton::Unknown;
				event.function_button_held = nullptr;

				break;
			}
			case ActionTarget::ACTION_DRAGGED:
			{
				event.mouseDragged = MouseButton::Unknown;
				event.function_mouse_dragged = nullptr;

				break;
			}
			case ActionTarget::ACTION_HOVERED:  event.function_mouse_hovered = nullptr; break;
			case ActionTarget::ACTION_SCROLLED: event.function_mouse_scrolled = nullptr; break;
			}
		}
		//Removes all event functions and resets their attached buttons
		inline void ClearAllEvents()
		{
			event.keyPressed = Key::Unknown;
			event.mousePressed = MouseButton::Unknown;
			event.function_button_pressed = nullptr;

			event.keyReleased = Key::Unknown;
			event.mouseReleased = MouseButton::Unknown;
			event.function_button_released = nullptr;

			event.keyHeld = Key::Unknown;
			event.mouseHeld = MouseButton::Unknown;
			event.function_button_held = nullptr;

			event.mouseDragged = MouseButton::Unknown;
			event.function_mouse_dragged = nullptr;

			event.function_mouse_hovered = nullptr;

			event.function_mouse_scrolled = nullptr;
		}

		//Poll the events that have attached functions once this frame,
		//skipped internally if 'isInteractable' is false
		void PollEvents(Input* input);

		//
		// GRAPHICS
		//

		inline void SetNormalizedColor(const vec3& newValue)
		{
			f32 clampX = clamp(newValue.x, 0.0f, 1.0f);
			f32 clampY = clamp(newValue.y, 0.0f, 1.0f);
			f32 clampZ = clamp(newValue.z, 0.0f, 1.0f);

			render.color = vec3(clampX, clampY, clampZ);
		}
		inline void SetRGBColor(const vec3& newValue)
		{
			int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
			int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
			int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

			f32 normalizedX = static_cast<f32>(clampX) / 255;
			f32 normalizedY = static_cast<f32>(clampY) / 255;
			f32 normalizedZ = static_cast<f32>(clampZ) / 255;

			render.color = vec3(normalizedX, normalizedY, normalizedZ);
		}

		inline const vec3& GetNormalizedColor() const { return render.color; }
		inline vec3 GetRGBColor() const
		{
			int rgbX = static_cast<int>(render.color.x * 255);
			int rgbY = static_cast<int>(render.color.y * 255);
			int rgbZ = static_cast<int>(render.color.z * 255);

			return vec3(rgbX, rgbY, rgbZ);
		}

		inline void SetOpacity(f32 newValue)
		{
			f32 clamped = clamp(newValue, 0.0f, 1.0f);
			render.opacity = clamped;
		}
		inline f32 GetOpacity() const { return render.opacity; }

		inline u32 GetVAO() const { return render.VAO; }
		inline u32 GetVBO() const { return render.VBO; }
		inline u32 GetEBO() const { return render.EBO; }

		inline const OpenGL_Shader* GetShader() const { return render.shader; }

		inline void SetTexture(OpenGL_Texture* newTexture)
		{
			if (newTexture
				&& render.texture != newTexture)
			{
				render.texture = newTexture;
			}
		}
		inline void ClearTexture() { render.texture = nullptr; }
		inline const OpenGL_Texture* GetTexture() const { return render.texture; }

		//
		// PARENT-CHILD HIERARCHY
		//

		//Returns the top-most widget of this widget
		inline Widget* GetRoot() { return parent ? parent->GetRoot() : this; }

		//Returns true if target widget is connected
		//to current widget as a child, parent or sibling.
		//Set recursive to true if you want deep widget search
		inline bool HasWidget(
			Widget* targetWidget, 
			bool recursive = false)
		{ 
			if (!targetWidget) return false;

			if (this == targetWidget) return true;

			//check descendants
			for (auto* c : children)
			{
				if (c == targetWidget) return true;

				if (recursive
					&& c->HasWidget(targetWidget, true))
				{
					return true;
				}
			}

			//check ancestors
			if (parent)
			{
				if (parent == targetWidget) return true;

				if (recursive 
					&& parent->HasWidget(targetWidget, true))
				{
					return true;
				}
			}

			return false;
		}

		inline bool IsParent(
			Widget* targetWidget,
			bool recursive = false)
		{
			if (!targetWidget
				|| this == targetWidget)
			{
				return false;
			}

			if (!parent) return false;

			if (parent == targetWidget) return true;

			if (recursive
				&& parent->IsParent(targetWidget, true))
			{
				return true;
			}

			return false;
		}
		inline Widget* GetParent() { return parent; }
		inline bool SetParent(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| HasWidget(targetWidget, true)
				|| targetWidget->HasWidget(this, true)
				|| (parent
				&& (parent == targetWidget
				|| parent->HasWidget(this, true))))
			{
				return false;
			}

			transform.localPos = vec3(0);
			transform.localRot = 0.0f;
			transform.localSize = vec3(0);

			UpdateTransform();

			//set this widget parent
			parent = targetWidget;
			//add this as new child to parent
			parent->children.push_back(this);

			return true;
		}
		inline bool RemoveParent()
		{
			//skip if parent never even existed
			if (!parent) return false;

			vector<Widget*>& parentChildren = parent->children;

			parentChildren.erase(remove(
				parentChildren.begin(),
				parentChildren.end(),
				this),
				parentChildren.end());

			transform.localPos = vec3(0);
			transform.localRot = 0.0f;
			transform.localSize = vec3(0);

			UpdateTransform();

			parent = nullptr;

			return true;
		}

		inline bool IsChild(
			Widget* targetWidget,
			bool recursive = false)
		{
			if (!targetWidget
				|| this == targetWidget)
			{
				return false;
			}

			for (auto* c : children)
			{
				if (c == targetWidget) return true;

				if (recursive
					&& c->IsChild(targetWidget, true))
				{
					return true;
				}
			}

			return false;
		}
		inline bool AddChild(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| HasWidget(targetWidget, true)
				|| targetWidget->HasWidget(this, true))
			{
				return false;
			}

			targetWidget->transform.localPos = vec3(0);
			targetWidget->transform.localRot = 0.0f;
			targetWidget->transform.localSize = vec3(0);

			targetWidget->UpdateTransform();

			children.push_back(targetWidget);
			targetWidget->parent = this;

			return true;
		}
		inline bool RemoveChild(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| targetWidget == parent)
			{
				return false;
			}

			targetWidget->transform.localPos = vec3(0);
			targetWidget->transform.localRot = 0.0f;
			targetWidget->transform.localSize = vec3(0);

			targetWidget->UpdateTransform();

			if (targetWidget->parent) targetWidget->parent = nullptr;

			children.erase(remove(
				children.begin(),
				children.end(),
				targetWidget),
				children.end());

			return true;
		}

		inline const vector<Widget*> GetAllChildren() { return children; }
		inline void RemoveAllChildren() 
		{ 
			for (auto* c : children)
			{
				c->transform.localPos = vec3(0);
				c->transform.localRot = 0.0f;
				c->transform.localSize = vec3(0);

				c->UpdateTransform();

				c->parent = nullptr;
			}
			children.clear(); 
		}

		//Do not destroy manually, erase from registry instead
		virtual ~Widget() = 0;
	protected:
		inline void UpdateAABB()
		{
			vec2 size = transform.combinedSize;
			float normalizedHeight = transform.viewportSize.y / transform.baseHeight;

			vec2 offset = vec2(0.0f, -(size.y * 0.7f * normalizedHeight));

			vec2 half = transform.combinedSize * 0.5f;

			transform.aabb[0] = transform.combinedPos - half + offset; //min
			transform.aabb[1] = transform.combinedPos + half + offset; //max
		}

		//Ensures the widget is centered
		inline void UpdateOriginalPosition()
		{
			vec2 center = transform.viewportSize * 0.5f;
			vec2 correctPos = vec2(center.x * 1.0f, center.y * 1.5f);

			if (transform.originalPos != correctPos) transform.originalPos = correctPos;

			if (transform.lastViewportSize != transform.viewportSize)
			{
				UpdateTransform();
				transform.lastViewportSize = transform.viewportSize;
			}
		}

		bool isInitialized{};

		string name = "NO_NAME_ADDED";

		u32 ID{};
		u32 windowID{};

		bool isHovered{};

		HitTarget hitTarget{};

		u16 zOrder{};

		bool isInteractable = true;

		Widget* parent{};
		vector<Widget*> children{};

		Widget_Transform transform{};
		Widget_Render render{};
		Widget_Event event{};

		static void Create2DQuad(
			u32& vaoOut,
			u32& vboOut,
			u32& eboOut);
	};
}