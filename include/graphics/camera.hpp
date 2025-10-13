//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::Graphics
{
	using std::string;

	class LIB_API Camera
	{
	public:
		static Camera* Initialize(
			const string& cameraName,
			u32 windowID,
			f32 fov,
			f32 speed,
			const vec3& pos = vec3(0),
			const vec3& rot = vec3(0, 0, 90.0f));

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
		void UpdateCameraRotation(vec2 delta)
		{
			rotVec.y += delta.x * sensitivity; //yaw
			rotVec.x -= delta.y * sensitivity; //pitch

			//prevent absurdly high positive or negative numbers
			rotVec.y = fmod(rotVec.y, 360.0f);

			//clamp to avoid flipping
			rotVec.x = clamp(rotVec.x, -89.99f, 89.99f);

			//calculate front from euler

			front.x = cos(radians(rotVec.y)) * cos(radians(rotVec.x));
			front.y = sin(radians(rotVec.x));
			front.z = sin(radians(rotVec.y)) * cos(radians(rotVec.x));
			front = normalize(front);

			right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));

			//update quat after euler stuff is done

			quat qx = angleAxis(radians(rotVec.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(rotVec.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(rotVec.z), vec3(0, 0, 1));

			rotQuat = normalize(qz * qy * qx);
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

		mat4 GetViewMatrix() const
		{
			return lookAt(pos, pos + front, up);
		};

		inline const vec3& GetUp() const { return up; }

		inline void SetFront(const vec3& newFront) { front = newFront; }
		inline const vec3& GetFront() const { return front; }

		inline void SetRight(const vec3& newRight) { right = newRight; }
		inline const vec3& GetRight() const { return right; }

		inline void SetPos(const vec3& newPos)
		{
			pos =
			{
				clamp(newPos.x, -10000.0f, 10000.0f),
				clamp(newPos.y, -10000.0f, 10000.0f),
				clamp(newPos.z, -10000.0f, 10000.0f)
			};
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

		//Safely wraps within allowed bounds
		inline void AddRot(const vec3& deltaRot)
		{
			auto WrapAngle = [](f32 angle)
				{
					angle = fmodf(angle, 360.0f);
					if (angle < 0.0f) angle += 360.0f;
					return angle;
				};

			rotVec =
			{
				WrapAngle(rotVec.x + deltaRot.x),
				WrapAngle(rotVec.y + deltaRot.y),
				WrapAngle(rotVec.z + deltaRot.z),
			};

			quat qx = angleAxis(radians(rotVec.x), vec3(1, 0, 0));
			quat qy = angleAxis(radians(rotVec.y), vec3(0, 1, 0));
			quat qz = angleAxis(radians(rotVec.z), vec3(0, 0, 1));

			rotQuat = qz * qy * qx;
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

		vec3 up = vec3(0.0f, 1.0f, 0.0f);
		vec3 front = vec3(0.0f, 0.0f, -1.0f);
		vec3 right = vec3(1.0f, 0.0f, 0.0f);

		vec3 pos{};
		vec3 rotVec{};
		quat rotQuat{};
	};
}