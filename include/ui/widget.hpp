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
#include "core/registry.hpp"

#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/opengl/opengl_texture.hpp"

namespace KalaWindow::UI
{
	using std::string;
	using std::vector;
	using std::clamp;
	using std::function;
	using std::array;

	using KalaWindow::Core::Registry;

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

		POS_COMBINED //get total position
	};
	enum class RotTarget
	{
		ROT_WORLD,   //get/set world rotation
		ROT_LOCAL,   //get/set rotation relative to parent
		ROT_COMBINED //get total rotation
	};
	enum class SizeTarget
	{
		SIZE_WORLD,   //get/set world size
		SIZE_LOCAL,   //get/set size relative to parent
		SIZE_COMBINED //get total size
	};

	enum class AnchorPoint
	{
		ANCHOR_NONE,         //disable anchoring
		ANCHOR_TOP_LEFT,     //lock local position to top left corner of parent
		ANCHOR_TOP_RIGHT,    //lock local position to top right corner of parent
		ANCHOR_BOTTOM_LEFT,  //lock local position to bottom left corner of parent
		ANCHOR_BOTTOM_RIGHT, //lock local position to bottom right corner of parent
		ANCHOR_CENTER,       //lock local position to center of parent
		ANCHOR_CUSTOM        //unlock POS_ANCHOR for custom anchor positioning
	};

	enum class TargetAction
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
		//if true, then aabb updates every time pos, rot or size is changed
		bool updateAABB{};

		AnchorPoint anchorPoint{};

		vec3 pos{};             //world position
		vec3 localPos{};        //parent offset position
		vec3 anchorPos{};       //anchor offset position
		vec3 combinedPos{};     //parent pos + (parent quat * local pos)

		vec3 rotVec{};          //euler rotation
		vec3 localRotVec{};     //parent offset euler rotation
		vec3 combinedRotVec{};  //ToEuler(combined rot quat)

		quat rotQuat{};         //quaternion rotation
		quat localRotQuat{};    //parent offset quaternion rotation
		quat combinedRotQuat{}; //parent combined rot quat * local rot quat

		vec3 size = vec3(32.0f, 32.0f, 0.0f); //world size
		vec3 localSize = vec3(1.0f);          //parent offset size
		vec3 combinedSize = size;             //parent combined size * local size

		array<vec3, 4> vertices = 
		{
			vec3(-0.5f, -0.5f, 0.0f), //bottom-left
			vec3(0.5f, -0.5f, 0.0f),  //bottom-right
			vec3(0.5f,  0.5f, 0.0f),  //top-right
			vec3(-0.5f,  0.5f, 0.0f)  //top-left
		};
		const array<u8, 6> indices =
		{
			0, 1, 2,
			2, 3, 0
		};

		array<vec3, 2> aabb{};
	};

	struct Widget_Render
	{
		bool canUpdate = true;
		bool is2D = true;

		//no children render past this widget size if true
		bool isClipping{};

		vec3 color = vec3(1.0f);
		float opacity = 1.0f;

		u32 VAO{};
		u32 VBO{};
		u32 EBO{};

		OpenGL_Shader* shader{};
		OpenGL_Texture* texture{};
	};

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

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

		//Toggle between 2D and 3D state, sets to 2D if true.
		//Delinks parent and children that don't match the new coordinate state
		inline void Set2DState(bool newValue)
		{ 
			if (parent
				&& parent->render.is2D != newValue)
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
						if (c->render.is2D != newValue) w->RemoveChild(c);
					}
				};

			RemoveInvalidChildren(this);

			render.is2D = newValue;
		}
		inline bool Is2D() const { return render.is2D; }

		//Core render function for all widget systems, must be overridden per inherited widget.
		//Pass mat4(1.0f) to view and pass 2D projection as ortho(0.0f, windowWidth, windowHeight, 0.0f)
		//if you want 2D UI, otherwise this widget element is drawn in 3D space
		virtual bool Render(
			const mat4& view,
			const mat4& projection) = 0;

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

		//Toggle whether you want AABB to update each time any pos, rot or size value is updated
		inline void SetUpdateAABBState(bool newValue) { transform.updateAABB = newValue; }
		//if true, then aabb updates every time pos, rot or size is changed
		inline bool IsUpdateAABBEnabled() const { return transform.updateAABB; }

		inline void SetAnchorType(
			AnchorPoint newValue)
		{ 
			switch (newValue)
			{
			case AnchorPoint::ANCHOR_NONE:
			case AnchorPoint::ANCHOR_CENTER:
			{
				transform.anchorPos = vec3(0.0f);
				break;
			}
			case AnchorPoint::ANCHOR_TOP_LEFT:
			{
				transform.anchorPos = vec3(
					-0.5f * transform.size.x,
					0.5f * transform.size.y,
					0.0f);
				break;
			}
			case AnchorPoint::ANCHOR_TOP_RIGHT:
			{
				transform.anchorPos = vec3(
					0.5f * transform.size.x,
					0.5f * transform.size.y,
					0.0f);
				break;
			}
			case AnchorPoint::ANCHOR_BOTTOM_LEFT:
			{
				transform.anchorPos = vec3(
					-0.5f * transform.size.x,
					-0.5f * transform.size.y,
					0.0f);
				break;
			}
			case AnchorPoint::ANCHOR_BOTTOM_RIGHT:
			{
				transform.anchorPos = vec3(
					0.5f * transform.size.x,
					-0.5f * transform.size.y,
					0.0f);
				break;
			}
			case AnchorPoint::ANCHOR_CUSTOM: break;
			}

			transform.anchorPoint = newValue;

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();
		}

		inline void SetPos(
			const vec3& newPos,
			PosTarget posTarget)
		{
			//cannot set combined pos
			if (posTarget == PosTarget::POS_COMBINED) return;

			//cannot set anchor pos if anchor point is not custom
			if (transform.anchorPoint != AnchorPoint::ANCHOR_CUSTOM
				&& posTarget == PosTarget::POS_ANCHOR)
			{
				return;
			}

			float clampedX = clamp(newPos.x, -10000.0f, 10000.0f);
			float clampedY = clamp(newPos.y, -10000.0f, 10000.0f);
			float clampedZ = clamp(newPos.z, -10000.0f, 10000.0f);

			vec3 clampedPos = vec3(clampedX, clampedY, clampedZ);

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:    transform.pos = clampedPos; break;
			case PosTarget::POS_LOCAL:    transform.localPos = clampedPos; break;
			case PosTarget::POS_ANCHOR:   transform.anchorPos = clampedPos; break;
			case PosTarget::POS_COMBINED: transform.combinedPos = clampedPos; break;
			}

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();
		}
		inline const vec3& GetPos(PosTarget posTarget) const 
		{ 
			static const vec3 empty{};

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:    return transform.pos; break;
			case PosTarget::POS_LOCAL:    return transform.localPos; break;
			case PosTarget::POS_ANCHOR:   return transform.anchorPos; break;
			case PosTarget::POS_COMBINED: return transform.combinedPos; break;
			}

			return empty;
		}

		//Safely wraps within allowed bounds
		inline void AddRot(const vec3& deltaRot)
		{
			auto WrapAngle = [](f32 angle)
				{
					angle = fmodf(angle, 360.0f);
					if (angle < 0.0f) angle += 360.0f;
					return angle;
				};

			transform.rotVec =
			{
				WrapAngle(transform.rotVec.x + deltaRot.x),
				WrapAngle(transform.rotVec.y + deltaRot.y),
				WrapAngle(transform.rotVec.z + deltaRot.z),
			};

			quat qx = angleAxis(radians(transform.rotVec.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(transform.rotVec.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(transform.rotVec.z), vec3(0, 0, 1));

			transform.rotQuat = qz * qy * qx;

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();
		}

		inline void SetRotVec(
			const vec3& newRot,
			RotTarget rotTarget)
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			vec3 clamped(
				clamp(newRot.x, -359.99f, 359.99f),
				clamp(newRot.y, -359.99f, 359.99f),
				clamp(newRot.z, -359.99f, 359.99f));

			vec3 clampedVec = vec3(clamped.x, clamped.y, clamped.z);

			quat qx = angleAxis(radians(clamped.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(clamped.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(clamped.z), vec3(0, 0, 1));

			quat clampedQuat = qz * qy * qx;

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:
			{
				transform.rotVec = clampedVec;
				transform.rotQuat = clampedQuat;
				break;
			}
			case RotTarget::ROT_LOCAL:
			{
				transform.localRotVec = clampedVec;
				transform.localRotQuat = clampedQuat;
				break;
			}
			}

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();
		}
		inline const vec3& GetRotVec(RotTarget rotTarget) const
		{
			static const vec3 empty{};

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:    return transform.rotVec; break;
			case RotTarget::ROT_LOCAL:    return transform.localRotVec; break;
			case RotTarget::ROT_COMBINED: return transform.combinedRotVec; break;
			}

			return empty;
		}

		inline void SetRotQuat(
			const quat& newRot,
			RotTarget rotTarget)
		{
			//cannot set combined quat rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			vec3 eulerDeg = degrees(eulerAngles(newRot));

			vec3 clamped(
				clamp(eulerDeg.x, -359.99f, 359.99f),
				clamp(eulerDeg.y, -359.99f, 359.99f),
				clamp(eulerDeg.z, -359.99f, 359.99f));

			vec3 clampedVec = vec3(clamped.x, clamped.y, clamped.z);

			quat qx = angleAxis(radians(clamped.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(clamped.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(clamped.z), vec3(0, 0, 1));

			quat clampedQuat = qz * qy * qx;

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:
			{
				transform.rotVec = clampedVec;
				transform.rotQuat = clampedQuat;
				break;
			}
			case RotTarget::ROT_LOCAL:
			{
				transform.localRotVec = clampedVec;
				transform.localRotQuat = clampedQuat;
				break;
			}
			}

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();
		}
		inline const quat& GetRotQuat(RotTarget rotTarget) const 
		{ 
			static const quat empty{};

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:    return transform.rotQuat; break;
			case RotTarget::ROT_LOCAL:    return transform.localRotQuat; break;
			case RotTarget::ROT_COMBINED: return transform.combinedRotQuat; break;
			}

			return empty;
		}

		inline void SetSize(
			const vec3& newSize,
			SizeTarget sizetarget)
		{
			//cannot set combined size
			if (sizetarget == SizeTarget::SIZE_COMBINED) return;

			float clampedX = clamp(newSize.x, 0.01f, 10000.0f);
			float clampedY = clamp(newSize.y, 0.01f, 10000.0f);
			float clampedZ = clamp(newSize.z, 0.01f, 10000.0f);

			vec3 clampedSize = vec3(clampedX, clampedY, clampedZ);

			switch (sizetarget)
			{
			case SizeTarget::SIZE_WORLD: transform.size = clampedSize; break;
			case SizeTarget::SIZE_LOCAL: transform.localSize = clampedSize; break;
			}

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();
		}
		inline const vec3& GetSize(SizeTarget sizeTarget) const 
		{ 
			static const vec3 empty{};

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD:    return transform.size; break;
			case SizeTarget::SIZE_LOCAL:    return transform.localSize; break;
			case SizeTarget::SIZE_COMBINED: return transform.combinedSize; break;
			}

			return empty;
		};

		inline void SetVerticeValue(u8 index, const vec3& newValue)
		{
			if (index > 3) return;

			float clampedX = clamp(newValue.x, -10000.0f, 10000.0f);
			float clampedY = clamp(newValue.y, -10000.0f, 10000.0f);
			float clampedZ = clamp(newValue.z, -10000.0f, 10000.0f);

			transform.vertices[index] = vec3(clampedX, clampedY, clampedZ);

			if (transform.updateAABB) UpdateAABB();
		}
		inline const vec3& GetVerticeValue(u8 index)
		{
			static const vec3 empty{};

			return index <= 3 ? transform.vertices[index] : empty;
		}

		inline void SetVertices(const array<vec3, 4>& newValue)
		{
			array<vec3, 4> clamped = newValue;

			for (auto& v : clamped)
			{
				float clampedX = clamp(v.x, -10000.0f, 10000.0f);
				float clampedY = clamp(v.y, -10000.0f, 10000.0f);
				float clampedZ = clamp(v.z, -10000.0f, 10000.0f);

				v = vec3(clampedX, clampedY, clampedZ);
			}

			transform.vertices = clamped;

			if (transform.updateAABB) UpdateAABB();
		}
		inline const array<vec3, 4>& GetVertices() const { return transform.vertices; };
		inline const array<u8, 6>& GetIndices() const { return transform.indices; }

		//Called automatically when rotation, size or vertices are updated
		inline void UpdateAABB()
		{
			mat4 world = GetWorldMatrix();

			vec3 boxMin(FLT_MAX);
			vec3 boxMax(-FLT_MAX);

			for (const auto& v : transform.vertices)
			{
				vec3 worldV = vec3(world * vec4(v, 1.0f));
				boxMin = min(boxMin, worldV);
				boxMax = max(boxMax, worldV);
			}

			transform.aabb = { boxMin, boxMax };
		}
		inline const array<vec3, 2>& GetAABB() const { return transform.aabb; }

		//Called automatically when any pos, rot or size type is updated
		inline void UpdateTransform()
		{
			//
			// ROTATION
			//

			if (parent) transform.combinedRotQuat = parent->transform.combinedRotQuat * transform.localRotQuat;
			else transform.combinedRotQuat = transform.rotQuat;
			transform.combinedRotVec = degrees(eulerAngles(transform.combinedRotQuat));

			//
			// SIZE
			//

			if (parent) transform.combinedSize = parent->transform.combinedSize * transform.localSize;
			else transform.combinedSize = transform.size;

			//
			// POSITION
			//

			vec3 localOffset = (
				transform.anchorPoint == AnchorPoint::ANCHOR_CUSTOM
				&& (transform.anchorPos.x != 0.0f
					|| transform.anchorPos.y != 0.0f
					|| transform.anchorPos.z != 0.0f))
				? transform.anchorPos
				: transform.localPos;

			if (parent)
			{
				vec3 rotatedOffset =
					parent->transform.combinedRotQuat
					* (parent->transform.combinedSize * localOffset);

				transform.combinedPos = parent->transform.combinedPos + rotatedOffset;
			}
			else transform.combinedPos = transform.pos;
		}

		inline mat4 GetWorldMatrix() const
		{
			mat4 m(1.0f);

			m = translate(m, transform.combinedPos);
			m *= mat4_cast(transform.combinedRotQuat);
			m = scale(m, transform.combinedSize);

			return m;
		}

		//
		// Z ORDER
		//

		//Makes this widget Z order 1 unit higher than target widget
		inline void MoveAbove(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| !targetWidget->isInitialized
				|| targetWidget->render.is2D != render.is2D)
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
				|| !targetWidget->isInitialized
				|| targetWidget->render.is2D != render.is2D)
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

		inline void SetAction(
			const function<void()>& newValue,
			TargetAction targetAction)
		{
			//skip if function is invalid
			if (!newValue) return;

			switch (targetAction)
			{
			case TargetAction::ACTION_PRESSED:  function_mouse_pressed = newValue; break;
			case TargetAction::ACTION_RELEASED: function_mouse_released = newValue; break;
			case TargetAction::ACTION_HELD:     function_mouse_held = newValue; break;
			case TargetAction::ACTION_HOVERED:  function_mouse_hovered = newValue; break;
			case TargetAction::ACTION_DRAGGED:  function_mouse_dragged = newValue; break;
			case TargetAction::ACTION_SCROLLED: function_mouse_scrolled = newValue; break;
			}
		}

		inline void RunAction(TargetAction targetAction) const
		{
			switch (targetAction)
			{
			case TargetAction::ACTION_PRESSED:  if (function_mouse_pressed) function_mouse_pressed(); break;
			case TargetAction::ACTION_RELEASED: if (function_mouse_released) function_mouse_released(); break;
			case TargetAction::ACTION_HELD:     if (function_mouse_held) function_mouse_held(); break;
			case TargetAction::ACTION_HOVERED:  if (function_mouse_hovered) function_mouse_hovered(); break;
			case TargetAction::ACTION_DRAGGED:  if (function_mouse_dragged) function_mouse_dragged(); break;
			case TargetAction::ACTION_SCROLLED: if (function_mouse_scrolled) function_mouse_scrolled(); break;
			}
		}

		//
		// GRAPHICS
		//

		inline void SetNormalizedColor(const vec3& newValue)
		{
			float clampX = clamp(newValue.x, 0.0f, 1.0f);
			float clampY = clamp(newValue.y, 0.0f, 1.0f);
			float clampZ = clamp(newValue.z, 0.0f, 1.0f);

			render.color = vec3(clampX, clampY, clampZ);
		}
		inline void SetRGBColor(const vec3& newValue)
		{
			int clampX = clamp(static_cast<int>(newValue.x), 0, 255);
			int clampY = clamp(static_cast<int>(newValue.y), 0, 255);
			int clampZ = clamp(static_cast<int>(newValue.z), 0, 255);

			float normalizedX = static_cast<float>(clampX) / 255;
			float normalizedY = static_cast<float>(clampY) / 255;
			float normalizedZ = static_cast<float>(clampZ) / 255;

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

		inline void SetOpacity(float newValue)
		{
			float clamped = clamp(newValue, 0.0f, 1.0f);
			render.opacity = clamped;
		}
		inline float GetOpacity() const { return render.opacity; }

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
				|| targetWidget->render.is2D != render.is2D
				|| HasWidget(targetWidget, true)
				|| targetWidget->HasWidget(this, true)
				|| (parent
				&& (parent == targetWidget
				|| parent->HasWidget(this, true))))
			{
				return false;
			}

			transform.localPos = vec3(0);
			transform.localRotVec = vec3(0);
			transform.localRotQuat = quat(1, 0, 0, 0);
			transform.localSize = vec3(0);

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();

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
			transform.localRotVec = vec3(0);
			transform.localRotQuat = quat(1, 0, 0, 0);
			transform.localSize = vec3(0);

			UpdateTransform();
			if (transform.updateAABB) UpdateAABB();

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
		inline void AddChild(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| targetWidget->render.is2D != render.is2D
				|| HasWidget(targetWidget, true)
				|| targetWidget->HasWidget(this, true))
			{
				return;
			}

			targetWidget->transform.localPos = vec3(0);
			targetWidget->transform.localRotVec = vec3(0);
			targetWidget->transform.localRotQuat = quat(1, 0, 0, 0);
			targetWidget->transform.localSize = vec3(0);

			targetWidget->UpdateTransform();
			if (targetWidget->transform.updateAABB) targetWidget->UpdateAABB();

			children.push_back(targetWidget);
			targetWidget->parent = this;
		}
		inline void RemoveChild(Widget* targetWidget)
		{
			if (!targetWidget
				|| targetWidget == this
				|| targetWidget == parent)
			{
				return;
			}

			targetWidget->transform.localPos = vec3(0);
			targetWidget->transform.localRotVec = vec3(0);
			targetWidget->transform.localRotQuat = quat(1, 0, 0, 0);
			targetWidget->transform.localSize = vec3(0);

			targetWidget->UpdateTransform();
			if (targetWidget->transform.updateAABB) targetWidget->UpdateAABB();

			if (targetWidget->parent) targetWidget->parent = nullptr;

			children.erase(remove(
				children.begin(),
				children.end(),
				targetWidget),
				children.end());
		}

		inline const vector<Widget*> GetAllChildren() { return children; }
		inline void RemoveAllChildren() 
		{ 
			for (auto* c : children)
			{
				c->transform.localPos = vec3(0);
				c->transform.localRotVec = vec3(0);
				c->transform.localRotQuat = quat(1, 0, 0, 0);
				c->transform.localSize = vec3(0);

				c->UpdateTransform();
				if (c->transform.updateAABB) c->UpdateAABB();

				c->parent = nullptr;
			}
			children.clear(); 
		}

		//Do not destroy manually, erase from containers.hpp instead
		virtual ~Widget();
	protected:
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