//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "core/core.hpp"
#include "graphics/camera.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::TargetType;
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
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot create camera '" + cameraName + "' because its target window is invalid!",
				"CAMERA",
				LogType::LOG_ERROR);

			return nullptr;
		}

		vector<OpenGL_Context*> contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
		OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

		if (!context
			|| !context->IsInitialized())
		{
			Log::Print(
				"Cannot load camera because its OpenGL context is invalid!",
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

		registry.AddContent(newID, move(newCam));
		window->AddValue(TargetType::TYPE_CAMERA, newID);

		camPtr->ID = newID;
		camPtr->windowID = window->GetID();

		camPtr->isInitialized = true;

		Log::Print(
			"Created camera '" + cameraName + "' with ID '" + to_string(newID) + "'!",
			"CAMERA",
			LogType::LOG_SUCCESS);

		return camPtr;
	}

	Camera::~Camera()
	{
		Log::Print(
			"Destroying camera '" + name + "' with ID '" + to_string(ID) + "'.",
			"CAMERA",
			LogType::LOG_INFO);
	}
}