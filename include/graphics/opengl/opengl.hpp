//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/api.hpp"

#include "graphics/window.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::string;

	enum VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};

	class LIB_API OpenGL_Renderer
	{
	public:
		//
		// GENERAL OPENGL
		//

		static bool IsInitialized() { return isInitialized; }

		static VSyncState GetVSyncState() { return vsyncState; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//GL notifications will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		//Check if this extension is supported by the current context (OpenGL 3.3)
		static bool IsExtensionSupported(const string& name);

		//Place after any gl call to check if an issue or error has occured within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static void GetError(const string& context);

		//
		// OS-SPECIFIC
		//

		//Global one-time OpenGL 3.3 init, needs to be called before per-window OpenGL init
		static bool GlobalInitialize();

		//Per-window OpenGL context init
		static bool Initialize(Window* targetWindow);

		//Allows to set vsync true or false.
		static void SetVSyncState(VSyncState vsyncState);

		//Call at the end of your render loop
		static void SwapOpenGLBuffers(Window* targetWindow);

		static void MakeContextCurrent(Window* window);
		static bool IsContextValid(Window* window);

		//Close the opengl context and clean the opengl textures and shaders.
		//This should always be called before the window that owns the opengl context is destroyed as well.
		static void Shutdown(Window* window);
	private:
		static inline bool isInitialized{};
		static inline bool isVerboseLoggingEnabled{};

		//If off, then all framerate is uncapped
		static inline VSyncState vsyncState = VSyncState::VSYNC_ON;
	};
}