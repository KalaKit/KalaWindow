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
#include <sstream>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/log_utils.hpp"

#include "core/glm_global.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl_texture.hpp"

namespace KalaWindow::UI
{
	using std::string;
	using std::vector;
	using std::clamp;
	using std::function;
	using std::array;
	using std::ostringstream;

	using KalaHeaders::Log;
	using KalaHeaders::LogType;

	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
	using KalaWindow::Graphics::OpenGL::OpenGL_Texture;

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
		POS_WORLD, //get/set world position
		POS_LOCAL, //get/set position relative to parent

		//get/set position relative to anchor,
		//only works if anchor point is set to ANCHOR_CUSTOM
		POS_ANCHOR,

		POS_COMBINED //get position affected by anchor and parent
	};
	enum class RotTarget
	{
		ROT_WORLD,   //get/set world rotation
		ROT_LOCAL,   //get/set rotation relative to parent

		ROT_COMBINED //get rotation affected by parent
	};
	enum class SizeTarget
	{
		SIZE_WORLD,   //get/set world size
		SIZE_LOCAL,   //get/set size relative to parent

		SIZE_COMBINED //get size affected by parent
	};

	enum class AnchorTarget
	{
		ANCHOR_NONE,         //disable anchoring
		ANCHOR_TOP_LEFT,     //lock anchor position to top left corner of world pos
		ANCHOR_TOP_RIGHT,    //lock anchor position to top right corner of world pos
		ANCHOR_BOTTOM_LEFT,  //lock anchor position to bottom left corner of world pos
		ANCHOR_BOTTOM_RIGHT, //lock anchor position to bottom right corner of world pos
		ANCHOR_CENTER,       //lock anchor position to center of world pos
		ANCHOR_CUSTOM        //for using as alternative of local pos for any parent or non-parent widget
	};

	enum class ActionTarget
	{
		ACTION_PRESSED,  //pressed mouse button while hovering over widget
		ACTION_RELEASED, //released mouse button while hovering over widget
		ACTION_HELD,     //held mouse button while hovering over widget
		ACTION_HOVERED,  //hovered cursor over widget
		ACTION_DRAGGED,  //held mouse button and moved mouse while hovering over widget
		ACTION_SCROLLED  //moved scrollwheel while hovering over widget
	};

	struct Widget_Transform
	{
		//helps avoid calling heavy hitter functions every frame
		bool isDirty = true;
		//if true, then the UI is scaled by base height
		bool canScale = false;

		AnchorTarget anchorTarget = AnchorTarget::ANCHOR_TOP_LEFT;

		vec2 worldPos{};      //world position unaffected by other values
		vec2 localPos{};      //parent offset position, unused if anchor is valid
		vec2 anchorPos{};     //anchor offset position
		vec2 combinedPos{};   //final position relative to parent and anchor

		f32 worldRot{};       //world euler rotation in degrees, unaffected by other values
		f32 localRot{};       //parent offset euler rotation
		f32 combinedRot{};    //final rotation relative to parent

		vec2 worldSize{};     //world size unaffected by other values
		vec2 localSize{};     //parent offset size
		vec2 combinedSize{};  //final size relative to parent

		vec2 viewportSize{};

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

		array<vec2, 4> corners{};
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

	class LIB_API Widget
	{
	public:
		//Returns all hit widgets at mouse position sorted by highest Z first
		static vector<Widget*> HitWidgets(u32 windowID);

		//
		// CORE
		//

		inline bool IsInitialized() const { return isInitialized; }

		//Core render function for all widget systems, must be overridden per inherited widget
		virtual bool Render(const mat3& projection) = 0;

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

		inline void SetDirtyState(bool newValue) { transform.isDirty = newValue; }
		inline bool IsDirty() const { return transform.isDirty; }

		//If true, then the UI is scaled by base height
		inline void SetScaleState(bool newValue) { transform.canScale = newValue; }
		//If true, then the UI is scaled by base height
		inline bool CanScale() const { return transform.canScale; }

		//Sets the current viewport size used for controlling how large UI appears relative to this height
		inline void SetViewportSize(vec2 newValue)
		{
			vec2 clamped = clamp(newValue, vec2(1.0f), vec2(10000.0f));
			transform.viewportSize = clamped;
		}
		//Returns the current viewport size used for position and size normalization
		inline vec2 GetViewportSize() const { return transform.viewportSize; }

		inline void SetAnchorTarget(AnchorTarget newValue)
		{ 
			switch (newValue)
			{
			case AnchorTarget::ANCHOR_NONE:
			case AnchorTarget::ANCHOR_CENTER:
			{
				transform.anchorPos = vec2(0.0f);
				break;
			}
			case AnchorTarget::ANCHOR_TOP_LEFT:
			{
				transform.anchorPos = vec2(
					0.5f * transform.worldSize.x,
					0.5f * transform.worldSize.y);
				break;
			}
			case AnchorTarget::ANCHOR_TOP_RIGHT:
			{
				transform.anchorPos = vec2(
					-0.5f * transform.worldSize.x,
					0.5f * transform.worldSize.y);
				break;
			}
			case AnchorTarget::ANCHOR_BOTTOM_LEFT:
			{
				transform.anchorPos = vec2(
					0.5f * transform.worldSize.x,
					-0.5f * transform.worldSize.y);
				break;
			}
			case AnchorTarget::ANCHOR_BOTTOM_RIGHT:
			{
				transform.anchorPos = vec2(
					-0.5f * transform.worldSize.x,
					-0.5f * transform.worldSize.y);
				break;
			}
			}

			transform.anchorTarget = newValue;

			transform.isDirty = true;
			UpdateTransform();
		}

		inline void SetPos(
			const vec2 newPos,
			PosTarget posTarget)
		{
			//cannot set combined pos
			if (posTarget == PosTarget::POS_COMBINED) return;

			//cannot set anchor pos if anchor point is not custom
			if (transform.anchorTarget != AnchorTarget::ANCHOR_CUSTOM
				&& posTarget == PosTarget::POS_ANCHOR)
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
			case PosTarget::POS_ANCHOR: transform.anchorPos = clampedPos; break;
			}

			transform.isDirty = true;
			UpdateTransform();
		}
		inline const vec2 GetPos(PosTarget posTarget) const 
		{ 
			static const vec2 empty{};

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:      return transform.worldPos; break;
			case PosTarget::POS_LOCAL:      return transform.localPos; break;
			case PosTarget::POS_ANCHOR:     return transform.anchorPos; break;
			case PosTarget::POS_COMBINED:   return transform.combinedPos; break;
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

			transform.isDirty = true;
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

			transform.isDirty = true;
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

			transform.isDirty = true;
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

		inline const array<vec2, 4>& GetCorners() 
		{ 
			UpdateCorners();
			return transform.corners; 
		}

		//Called automatically when any pos, rot or size type is updated
		inline void UpdateTransform()
		{
			if (!transform.isDirty) return;

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

			vec2 localOffset{};
			bool useUnparentedLocalOffset = false;
			
			if (transform.anchorTarget == AnchorTarget::ANCHOR_TOP_LEFT
				|| transform.anchorTarget == AnchorTarget::ANCHOR_TOP_RIGHT
				|| transform.anchorTarget == AnchorTarget::ANCHOR_BOTTOM_RIGHT
				|| transform.anchorTarget == AnchorTarget::ANCHOR_BOTTOM_LEFT
				|| transform.anchorTarget == AnchorTarget::ANCHOR_CENTER

				//custom anchor pos
				|| (transform.anchorTarget == AnchorTarget::ANCHOR_CUSTOM
				&& (transform.anchorPos.x != 0.0f
				|| transform.anchorPos.y != 0.0f)))
			{
				localOffset = transform.anchorPos;
				useUnparentedLocalOffset = true;
			}
			else localOffset = transform.localPos;

			if (parent)
			{
				f32 rads = radians(parent->transform.combinedRot);
				mat2 rotMat =
				{
					{ cos(rads), sin(rads) },
					{ -sin(rads), cos(rads) }
				};

				vec2 rotOffset = rotMat * localOffset;
				transform.combinedPos = 
					parent->transform.combinedPos 
					+ transform.worldPos
					+ rotOffset;
			}
			else
			{
				transform.combinedPos = transform.worldPos;
				if (useUnparentedLocalOffset) transform.combinedPos += localOffset;
			}

			ostringstream oss{};

			oss << "\nlooking at widget '" + name + "' in UpdateTransform()\n"
				<< "    world pos:  " << transform.worldPos.x << ", " << transform.worldPos.y << "\n"
				<< "    world rot:  " << transform.worldRot << "\n"
				<< "    world size: " << transform.worldSize.x << ", " << transform.worldSize.y << "\n"

				<< "===========================================\n"

				<< "    local pos:  " << transform.localPos.x << ", " << transform.localPos.y << "\n"
				<< "    local rot:  " << transform.localRot << "\n"
				<< "    local size: " << transform.localSize.x << ", " << transform.localSize.y << "\n"
				
				<< "===========================================\n"
				
				<< "    anchored pos:  " << transform.anchorPos.x << ", " << transform.anchorPos.y << "\n"
				<< "    combined pos:  " << transform.combinedPos.x << ", " << transform.combinedPos.y << "\n"
				<< "    combined rot:  " << transform.combinedRot << "\n"
				<< "    combined size: " << transform.combinedSize.x << ", " << transform.combinedSize.y << "\n";

			Log::Print(
				oss.str(),
				"WIDGET",
				LogType::LOG_DEBUG);

			UpdateCorners();

			transform.isDirty = false;
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

		//Skip hit testing if true
		inline void SetInteractableState(bool newValue) { isInteractable = newValue; }
		//Skip hit testing if true
		inline bool IsInteractable() const { return isInteractable; }

		inline bool IsMouseInsideImage(const vec2 mouse)
		{
			UpdateCorners();

			auto PointInTriangle = [](
				const vec2& p,
				const vec2& a,
				const vec2& b,
				const vec2& c)
				{
					auto cross2D = [](vec2 u, vec2 v)
						{
							return u.x * v.y - u.y * v.x;
						};

					f32 c1 = cross2D(b - a, p - a);
					f32 c2 = cross2D(c - b, p - b);
					f32 c3 = cross2D(a - c, p - c);

					return (
						c1 >= 0
						&& c2 >= 0
						&& c3 >= 0)
						|| (c1 <= 0
						&& c2 <= 0
						&& c3 <= 0);
				};

			return
				PointInTriangle(mouse,
					transform.corners[0],
					transform.corners[1],
					transform.corners[2])
				|| PointInTriangle(mouse,
					transform.corners[0],
					transform.corners[2],
					transform.corners[3]);
		};

		//If the cursor is over this widget and this widget is not
		//covered entirely or partially by another widget then this returns true
		bool IsHovered() const;

		inline void SetAction(
			const function<void()>& newValue,
			ActionTarget actionTarget)
		{
			//skip if function is invalid
			if (!newValue) return;

			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:  function_mouse_pressed = newValue; break;
			case ActionTarget::ACTION_RELEASED: function_mouse_released = newValue; break;
			case ActionTarget::ACTION_HELD:     function_mouse_held = newValue; break;
			case ActionTarget::ACTION_HOVERED:  function_mouse_hovered = newValue; break;
			case ActionTarget::ACTION_DRAGGED:  function_mouse_dragged = newValue; break;
			case ActionTarget::ACTION_SCROLLED: function_mouse_scrolled = newValue; break;
			}
		}

		inline void RunAction(ActionTarget actionTarget) const
		{
			switch (actionTarget)
			{
			case ActionTarget::ACTION_PRESSED:  if (function_mouse_pressed) function_mouse_pressed(); break;
			case ActionTarget::ACTION_RELEASED: if (function_mouse_released) function_mouse_released(); break;
			case ActionTarget::ACTION_HELD:     if (function_mouse_held) function_mouse_held(); break;
			case ActionTarget::ACTION_HOVERED:  if (function_mouse_hovered) function_mouse_hovered(); break;
			case ActionTarget::ACTION_DRAGGED:  if (function_mouse_dragged) function_mouse_dragged(); break;
			case ActionTarget::ACTION_SCROLLED: if (function_mouse_scrolled) function_mouse_scrolled(); break;
			}
		}

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

			transform.isDirty = true;
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

			transform.isDirty = true;
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

			targetWidget->transform.isDirty = true;
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

			targetWidget->transform.isDirty = true;
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

				c->transform.isDirty = true;
				c->UpdateTransform();

				c->parent = nullptr;
			}
			children.clear(); 
		}

		//Do not destroy manually, erase from registry instead
		virtual ~Widget() = 0;
	protected:
		//used for ui scaling, affects UI size
		static inline f32 baseHeight = 1080.0f;

		inline void UpdateCorners()
		{
			f32 r = radians(transform.combinedRot);
			f32 c = cos(r);
			f32 s = sin(r);

			mat2 rot = { { c, -s }, { s, c } };

			vec2 basePos = parent
				? transform.worldPos + transform.localPos
				: transform.worldPos;

			f32 w = transform.combinedSize.x;
			f32 h = transform.combinedSize.y;

			array<vec2, 4> local =
			{
				vec2(0.0f, 0.0f), //top-left
				vec2(w,    0.0f), //top-right
				vec2(w,    h),    //bottom-right
				vec2(0.0f, h)     //bottom-left
			};

			for (size_t i = 0; i < 4; ++i)
			{
				transform.corners[i] = basePos + rot * local[i];
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
		function<void()> function_mouse_pressed{};
		function<void()> function_mouse_released{};
		function<void()> function_mouse_held{};
		function<void()> function_mouse_hovered{};
		function<void()> function_mouse_dragged{};
		function<void()> function_mouse_scrolled{};

		Widget* parent{};
		vector<Widget*> children{};

		Widget_Transform transform{};
		Widget_Render render{};

		static void Create2DQuad(
			u32& vaoOut,
			u32& vboOut,
			u32& eboOut);
	};
}