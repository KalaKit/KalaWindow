//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

constexpr f32 MAX_POS = 10000.0f;
constexpr f32 MIN_POS = -10000.0f;

constexpr f32 MAX_SIZE = 10000.0f;
constexpr f32 MIN_SIZE = 0.01f;

namespace KalaWindow::Utils
{
	using KalaHeaders::vec2;
	using KalaHeaders::vec3;
	using KalaHeaders::vec4;
	using KalaHeaders::mat2;
	using KalaHeaders::mat3;
	using KalaHeaders::mat4;
	using KalaHeaders::quat;
	using KalaHeaders::radians;
	using KalaHeaders::wrap;
	using KalaHeaders::kclamp;
	using KalaHeaders::rotate;
	using KalaHeaders::toeuler2;
	using KalaHeaders::toeuler3;
	using KalaHeaders::toquat;
	using KalaHeaders::normalize;

	enum class PosTarget
	{
		POS_WORLD,    //position in world space
		POS_LOCAL,    //position relative to parent

		POS_COMBINED, //final position after combining world and local position
	};
	enum class RotTarget
	{
		ROT_WORLD,   //rotation in world space
		ROT_LOCAL,   //rotation relative to parent

		ROT_COMBINED //final rotation after combining world and local rotation
	};
	enum class SizeTarget
	{
		SIZE_WORLD,   //size in world space
		SIZE_LOCAL,   //size relative to parent

		SIZE_COMBINED //final position after combining world and local position
	};

	class LIB_API Transform2D
	{
	public:
		//Incrementally moves over time
		inline void AddPos(
			const vec2 pos_delta,
			PosTarget posTarget,
			const Transform2D& parent = Transform2D{})
		{
			//cannot set combined pos
			if (posTarget == PosTarget::POS_COMBINED) return;

			vec2 base = (posTarget == PosTarget::POS_WORLD)
				? pos_world
				: pos_local;

			vec2 pos_clamped = kclamp(
				base + pos_delta,
				vec2(MIN_POS),
				vec2(MAX_POS));

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:  pos_world = pos_clamped; break;
			case PosTarget::POS_LOCAL:  pos_local = pos_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Snaps to given position
		inline void SetPos(
			const vec2 pos_new,
			PosTarget posTarget,
			const Transform2D& parent = Transform2D{})
		{
			//cannot set combined pos
			if (posTarget == PosTarget::POS_COMBINED) return;

			vec2 pos_clamped = kclamp(
				pos_new, 
				vec2(MIN_POS), 
				vec2(MAX_POS));

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:  pos_world = pos_clamped; break;
			case PosTarget::POS_LOCAL:  pos_local = pos_clamped; break;
			}

			UpdateTransform(parent);
		}
		inline const vec2 GetPos(PosTarget posTarget) const
		{
			static const vec2 empty{};

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:    return pos_world; break;
			case PosTarget::POS_LOCAL:    return pos_local; break;
			case PosTarget::POS_COMBINED: return pos_combined; break;
			}

			return empty;
		}

		//Takes in rotation in euler (degrees) and incrementally rotates over time
		inline void AddRot(
			float rot_delta,
			RotTarget rotTarget,
			const Transform2D& parent = Transform2D{})
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			f32 base = (rotTarget == RotTarget::ROT_WORLD)
				? rot_world
				: rot_local;

			f32 rot_clamped = wrap(base + rot_delta);

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: rot_world = rot_clamped; break;
			case RotTarget::ROT_LOCAL: rot_local = rot_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Takes in rotation in euler (degrees) and snaps to given rotation
		inline void SetRot(
			const f32 rot_new,
			RotTarget rotTarget,
			const Transform2D& parent = Transform2D{})
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			f32 rot_clamped = wrap(rot_new);

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: rot_world = rot_clamped; break;
			case RotTarget::ROT_LOCAL: rot_local = rot_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Returns rotation in euler (degrees)
		inline const f32 GetRot(RotTarget rotTarget) const
		{
			static const f32 empty{};

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:    return rot_world; break;
			case RotTarget::ROT_LOCAL:    return rot_local; break;
			case RotTarget::ROT_COMBINED: return rot_combined; break;
			}

			return empty;
		}

		//Incrementally scales over time
		inline void AddSize(
			const vec2 size_delta,
			SizeTarget sizeTarget,
			const Transform2D& parent = Transform2D{})
		{
			//cannot set combined size
			if (sizeTarget == SizeTarget::SIZE_COMBINED) return;

			vec2 base = (sizeTarget == SizeTarget::SIZE_WORLD)
				? size_world
				: size_local;

			vec2 size_clamped = kclamp(
				base + size_delta,
				vec2(MIN_SIZE),
				vec2(MAX_SIZE));

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD: size_world = size_clamped; break;
			case SizeTarget::SIZE_LOCAL: size_local = size_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Snaps to given size
		inline void SetSize(
			const vec2 size_new,
			SizeTarget sizeTarget,
			const Transform2D& parent = Transform2D{})
		{
			//cannot set combined size
			if (sizeTarget == SizeTarget::SIZE_COMBINED) return;

			vec2 size_clamped = kclamp(
				size_new, 
				vec2(MIN_SIZE), 
				vec2(MAX_SIZE));

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD: size_world = size_clamped; break;
			case SizeTarget::SIZE_LOCAL: size_local = size_clamped; break;
			}

			UpdateTransform(parent);
		}
		inline const vec2 GetSize(SizeTarget sizeTarget) const
		{
			static const vec2 empty{};

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD:      return size_world; break;
			case SizeTarget::SIZE_LOCAL:      return size_local; break;
			case SizeTarget::SIZE_COMBINED:   return size_combined; break;
			}

			return empty;
		};
	private:
		//Updates combined pos, rot and size relative to local and optional parent values
		inline void UpdateTransform(const Transform2D& parent)
		{
			if (parent.pos_combined != 0.0f)
			{
				rot_combined = parent.rot_combined + rot_world + rot_local;
				size_combined = parent.size_combined * size_world * size_local;

				f32 rads = radians(parent.rot_combined);
				mat3 rot_mat =
				{
					cos(rads), -sin(rads), 0.0f,
					sin(rads),  cos(rads), 0.0f,
					0.0f,       0.0f,      1.0f
				};

				vec3 rot_offset = vec3(rot_mat * vec3(pos_local, 1.0f));
				pos_combined =
					parent.pos_combined
					+ pos_world
					+ vec2(rot_offset.x, rot_offset.y);
			}
			else
			{
				rot_combined = rot_world;
				size_combined = size_world;
				pos_combined = pos_world;
			}
		}

		static inline vec2 pos_world{};
		static inline vec2 pos_local{};
		static inline vec2 pos_combined{};

		static inline f32 rot_world{};
		static inline f32 rot_local{};
		static inline f32 rot_combined{};

		static inline vec2 size_world{};
		static inline vec2 size_local{};
		static inline vec2 size_combined{};
	};

	class LIB_API Transform3D
	{
	public:
		//Incrementally moves over time
		inline void AddPos(
			const vec3& pos_delta,
			PosTarget posTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined pos
			if (posTarget == PosTarget::POS_COMBINED) return;

			vec3 base = (posTarget == PosTarget::POS_WORLD)
				? pos_world 
				: pos_local;

			vec3 pos_clamped = kclamp(
				base + pos_delta,
				vec3(MIN_POS),
				vec3(MAX_POS));

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:  pos_world = pos_clamped; break;
			case PosTarget::POS_LOCAL:  pos_local = pos_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Snaps to given position
		inline void SetPos(
			const vec3& pos_new,
			PosTarget posTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined pos
			if (posTarget == PosTarget::POS_COMBINED) return;

			vec3 pos_clamped = kclamp(
				pos_new, 
				vec3(MIN_POS), 
				vec3(MAX_POS));

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:  pos_world = pos_clamped; break;
			case PosTarget::POS_LOCAL:  pos_local = pos_clamped; break;
			}

			UpdateTransform(parent);
		}
		inline const vec3 GetPos(PosTarget posTarget) const
		{
			static const vec3 empty{};

			switch (posTarget)
			{
			case PosTarget::POS_WORLD:    return pos_world; break;
			case PosTarget::POS_LOCAL:    return pos_local; break;
			case PosTarget::POS_COMBINED: return pos_combined; break;
			}

			return empty;
		}

		//Takes in rotation in euler (degrees) and incrementally rotates over time
		inline void AddRot(
			const vec3& rot_delta,
			RotTarget rotTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			vec3 current{};

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: current = toeuler3(rot_world); break;
			case RotTarget::ROT_LOCAL: current = toeuler3(rot_local); break;
			}

			vec3 rot_clamped = vec3(
				wrap(current.x + rot_delta.x),
				wrap(current.y + rot_delta.y),
				wrap(current.z + rot_delta.z));

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: rot_world = toquat(rot_clamped); break;
			case RotTarget::ROT_LOCAL: rot_local = toquat(rot_clamped); break;
			}

			UpdateTransform(parent);
		}
		//Takes in rotation in euler (degrees) and snaps to given rotation
		inline void SetRot(
			const vec3& rot_new,
			RotTarget rotTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			vec3 rot_clamped = vec3(
				wrap(rot_new.x), 
				wrap(rot_new.y), 
				wrap(rot_new.z));

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: rot_world = toquat(rot_clamped); break;
			case RotTarget::ROT_LOCAL: rot_local = toquat(rot_clamped); break;
			}

			UpdateTransform(parent);
		}
		//Takes in rotation in quaternion and snaps to given rotation
		inline void SetRot(
			const quat& rot_new,
			RotTarget rotTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined vec rot
			if (rotTarget == RotTarget::ROT_COMBINED) return;

			quat rot_clamped = normalize(rot_new);

			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD: rot_world = rot_clamped; break;
			case RotTarget::ROT_LOCAL: rot_local = rot_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Returns rotation in euler (degrees)
		inline const vec3 GetRotEuler(RotTarget rotTarget) const
		{
			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:    return toeuler3(rot_world); break;
			case RotTarget::ROT_LOCAL:    return toeuler3(rot_local); break;
			case RotTarget::ROT_COMBINED: return toeuler3(rot_combined); break;
			default:                      return{};
			}
		}
		//Returns quaternion rotation
		inline const quat GetRotQuat(RotTarget rotTarget) const
		{
			switch (rotTarget)
			{
			case RotTarget::ROT_WORLD:    return rot_world; break;
			case RotTarget::ROT_LOCAL:    return rot_local; break;
			case RotTarget::ROT_COMBINED: return rot_combined; break;
			default:                      return{};
			}
		}

		//Incrementally scales over time
		inline void AddSize(
			const vec3& size_delta,
			SizeTarget sizeTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined size
			if (sizeTarget == SizeTarget::SIZE_COMBINED) return;

			vec3 base = (sizeTarget == SizeTarget::SIZE_WORLD)
				? size_world
				: size_local;

			vec3 size_clamped = kclamp(
				base + size_delta,
				vec3(MIN_SIZE),
				vec3(MAX_SIZE));

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD:  size_world = size_clamped; break;
			case SizeTarget::SIZE_LOCAL:  size_local = size_clamped; break;
			}

			UpdateTransform(parent);
		}
		//Snaps to given scale
		inline void SetSize(
			const vec3& size_new,
			SizeTarget sizeTarget,
			const Transform3D& parent = Transform3D{})
		{
			//cannot set combined size
			if (sizeTarget == SizeTarget::SIZE_COMBINED) return;

			vec3 size_clamped = kclamp(
				size_new, 
				vec3(MIN_SIZE), 
				vec3(MAX_SIZE));

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD: size_world = size_clamped; break;
			case SizeTarget::SIZE_LOCAL: size_local = size_clamped; break;
			}

			UpdateTransform(parent);
		}
		inline const vec3 GetSize(SizeTarget sizeTarget) const
		{
			static const vec3 empty{};

			switch (sizeTarget)
			{
			case SizeTarget::SIZE_WORLD:      return size_world; break;
			case SizeTarget::SIZE_LOCAL:      return size_local; break;
			case SizeTarget::SIZE_COMBINED:   return size_combined; break;
			}

			return empty;
		};
	private:
		//Updates combined pos, rot and size relative to local and optional parent values
		inline void UpdateTransform(const Transform3D& parent)
		{
			if (parent.pos_combined != 0.0f)
			{
				rot_combined = parent.rot_combined * rot_world * rot_local;
				size_combined = parent.size_combined * size_world * size_local;

				mat4 rot_mat = rotate(mat4(1.0f), parent.rot_combined);

				vec4 rot_offset = rot_mat * vec4(pos_local, 1.0f);
				pos_combined =
					parent.pos_combined
					+ pos_world
					+ vec3(
						rot_offset.x,
						rot_offset.y,
						rot_offset.z);
			}
			else
			{
				rot_combined = rot_world;
				size_combined = size_world;
				pos_combined = pos_world;
			}
		}

		static inline vec3 pos_world{};
		static inline vec3 pos_local{};
		static inline vec3 pos_combined{};

		static inline quat rot_world{};
		static inline quat rot_local{};
		static inline quat rot_combined{};

		static inline vec3 size_world{};
		static inline vec3 size_local{};
		static inline vec3 size_combined{};
	};
}