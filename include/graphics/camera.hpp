//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "core/registry.hpp"

namespace KalaWindow::Graphics
{
	using std::string;

	using KalaHeaders::kvec2;
	using KalaHeaders::kvec3;
	using KalaHeaders::kmat4;
	using KalaHeaders::kquat;
	using KalaHeaders::cross;
	using KalaHeaders::radians;
	using KalaHeaders::normalize;
	using KalaHeaders::lookat;
	using KalaHeaders::toeuler3;
	using KalaHeaders::angleaxis;
	using KalaHeaders::wrap;

	using KalaWindow::Core::Registry;

	class LIB_API Camera
	{
	public:
		static inline Registry<Camera> registry{};

		static Camera* Initialize(
			const string& cameraName,
			u32 windowID,
			f32 fov,
			f32 speed,
			const kvec3& pos = kvec3(0),
			const kvec3& rot = kvec3(0, 0, 90.0f));

		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

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

		//Handle camera rotation based off of mouse movement
		void UpdateCameraRotation(kvec2 delta)
		{
			rotVec.y += delta.x * sensitivity; //yaw
			rotVec.x -= delta.y * sensitivity; //pitch

			//prevent absurdly high positive or negative numbers
			rotVec.y = wrap(rotVec.y);

			//clamp to avoid flipping
			rotVec.x = clamp(rotVec.x, -89.99f, 89.99f);

			//calculate front from euler

			front.x = cos(radians(rotVec.y)) * cos(radians(rotVec.x));
			front.y = sin(radians(rotVec.x));
			front.z = sin(radians(rotVec.y)) * cos(radians(rotVec.x));
			front = normalize(front);

			right = normalize(cross(front, kvec3(0.0f, 1.0f, 0.0f)));

			//update kquat after euler stuff is done

			kquat qx = angleaxis(radians(rotVec.x), kvec3(1, 0, 0));
			kquat qy = angleaxis(radians(rotVec.y), kvec3(0, 1, 0));
			kquat qz = angleaxis(radians(rotVec.z), kvec3(0, 0, 1));

			rotquat = normalize(qz * qy * qx);
		}

		inline void SetFOV(f32 newFOV)
		{
			fov = clamp(newFOV, 70.0f, 110.0f);
		}
		inline f32 GetFOV() const { return fov; }

		inline void SetNearClip(f32 newNearClip)
		{
			nearClip = clamp(newNearClip, 0.001f, farClip - 0.1f);
		}
		inline f32 GetNearClip() const { return nearClip; }

		inline void SetFarClip(f32 newFarClip)
		{
			farClip = clamp(newFarClip, nearClip + 0.1f, 1000.0f);
		}
		inline f32 GetFarClip() const { return farClip; }

		//Called inside resize callback to ensure camera aspect ratio always stays valid
		inline void SetAspectRatio(f32 size)
		{
			aspectRatio = clamp(size, 0.001f, 10.0f);
		}
		inline f32 GetAspectRatio() const { return aspectRatio; }

		inline void SetSpeed(f32 newSpeed)
		{
			speed = clamp(newSpeed, 0.01f, 25.0f);
		}
		inline f32 GetSpeed() const { return speed; }

		inline void SetSensitivity(f32 newSens)
		{
			sensitivity = clamp(newSens, 0.001f, 10.0f);
		}
		inline f32 GetSensitivity() const { return sensitivity; }

		kmat4 GetViewMatrix() const
		{
			return lookat(pos, pos + front, up);
		};

		inline const kvec3& GetUp() const { return up; }

		inline void SetFront(const kvec3& newFront) { front = newFront; }
		inline const kvec3& GetFront() const { return front; }

		inline void SetRight(const kvec3& newRight) { right = newRight; }
		inline const kvec3& GetRight() const { return right; }

		inline void SetPos(const kvec3& newPos)
		{
			pos =
			{
				clamp(newPos.x, -10000.0f, 10000.0f),
				clamp(newPos.y, -10000.0f, 10000.0f),
				clamp(newPos.z, -10000.0f, 10000.0f)
			};
		}
		inline const kvec3& GetPos() const { return pos; }

		inline void SetRotVec(const kvec3& newRot)
		{
			kvec3 clamped = newRot;
			clamped.x = wrap(clamped.x);
			clamped.y = wrap(clamped.y);
			clamped.z = wrap(clamped.z);

			rotVec = clamped;

			kquat qx = angleaxis(radians(clamped.x), kvec3(1, 0, 0));
			kquat qy = angleaxis(radians(clamped.y), kvec3(0, 1, 0));
			kquat qz = angleaxis(radians(clamped.z), kvec3(0, 0, 1));

			rotquat = qz * qy * qx;
		}
		inline const kvec3& GetRotVec() const { return rotVec; }

		inline void Setrotquat(const kquat& newRot)
		{
			kvec3 eulerDeg = degrees(toeuler3(newRot));

			kvec3 clamped = eulerDeg;
			clamped.x = wrap(clamped.x);
			clamped.y = wrap(clamped.y);
			clamped.z = wrap(clamped.z);

			rotVec = clamped;

			kquat qx = angleaxis(radians(clamped.x), kvec3(1, 0, 0));
			kquat qy = angleaxis(radians(clamped.y), kvec3(0, 1, 0));
			kquat qz = angleaxis(radians(clamped.z), kvec3(0, 0, 1));

			rotquat = qz * qy * qx;
		}
		inline const kquat& Getrotquat() const { return rotquat; }

		//Safely wraps within allowed bounds
		inline void AddRot(const kvec3& deltaRot)
		{
			rotVec =
			{
				wrap(rotVec.x + deltaRot.x),
				wrap(rotVec.y + deltaRot.y),
				wrap(rotVec.z + deltaRot.z),
			};

			kquat qx = angleaxis(radians(rotVec.x), kvec3(1, 0, 0));
			kquat qy = angleaxis(radians(rotVec.y), kvec3(0, 1, 0));
			kquat qz = angleaxis(radians(rotVec.z), kvec3(0, 0, 1));

			rotquat = qz * qy * qx;
		}

		~Camera();
	private:
		bool isInitialized{};

		string name{};

		u32 ID{};
		u32 windowID{};

		f32 fov{};
		f32 speed{};

		f32 aspectRatio{};

		f32 nearClip = 0.01f;
		f32 farClip = 512.0;
		f32 sensitivity = 0.1f;

		kvec3 up = kvec3(0.0f, 1.0f, 0.0f);
		kvec3 front = kvec3(0.0f, 0.0f, -1.0f);
		kvec3 right = kvec3(1.0f, 0.0f, 0.0f);

		kvec3 pos{};
		kvec3 rotVec{};
		kquat rotquat{};
	};
}