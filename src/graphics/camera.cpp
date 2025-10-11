//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/containers.hpp"
#include "graphics/camera.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::GetValueByID;
using KalaWindow::Core::globalID;
using KalaWindow::Core::WindowContent;
using KalaWindow::Core::windowContent;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;

using std::to_string;
using std::unique_ptr;
using std::make_unique;

namespace KalaWindow::Graphics
{
	Camera* Camera::Initialize(
		const string& cameraName,
		u32 windowID,
		f32 fov,
		f32 speed,
		const vec3& pos,
		const vec3& rot)
	{
		Window* window = GetValueByID<Window>(windowID);
		if (!window)
		{
			Log::Print(
				"Cannot create camera '" + cameraName + "' because its target window is invalid!",
				"CAMERA",
				LogType::LOG_ERROR);

			return nullptr;
		}

		WindowContent& content = windowContent[window];

		OpenGL_Context* context = content.glContext.get();
		if (!context
			|| !context->IsInitialized())
		{
			Log::Print(
				"Cannot create camera '" + cameraName + "' because the target window '" + window->GetTitle() + "' OpenGL context is invalid!",
				"CAMERA",
				LogType::LOG_ERROR);

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<Camera> newCam = make_unique<Camera>();
		Camera* camPtr = newCam.get();

		Log::Print(
			"Creating camera '" + cameraName + "' with ID '" + to_string(newID) + "'.",
			"CAMERA",
			LogType::LOG_DEBUG);

		camPtr->SetFOV(fov);
		camPtr->SetSpeed(speed);

		camPtr->SetPos(pos);
		camPtr->SetRotVec(rot);
		camPtr->UpdateCameraRotation(vec2(0));

		vec2 size = window->GetFramebufferSize();
		f32 aspectRatio = size.x / size.y;
		camPtr->SetAspectRatio(aspectRatio);

		content.cameras[newID] = move(newCam);
		content.runtimeCameras.push_back(camPtr);

		camPtr->ID = newID;
		camPtr->windowID = window->GetID();

		camPtr->isInitialized = true;

		Log::Print(
			"Created camera '" + cameraName + "' with ID '" + to_string(newID) + "'!",
			"CAMERA",
			LogType::LOG_SUCCESS);

		return camPtr;
	}

	void Camera::SetWindowID(u32 newID)
	{
		//skip if ID is empty
		if (newID == 0) return;
		//skip if ID is the same as current
		if (newID == ID) return;

		Window* window = GetValueByID<Window>(newID);

		//skip if ID doesnt lead to a real window
		if (!window) return;

		windowID = newID;
	}

	Camera::~Camera()
	{
		Log::Print(
			"Destroying camera '" + name + "' with ID '" + to_string(ID) + "'.",
			"CAMERA",
			LogType::LOG_INFO);
	}
}