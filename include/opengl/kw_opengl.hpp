//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "core_utils.hpp"

#include "core/kw_registry.hpp"

namespace KalaWindow::OpenGL
{
	using std::string;
	using std::string_view;
	using std::function;

	using u32 = uint32_t;

	using KalaWindow::Core::KalaWindowRegistry;

	//Hardware accelerated antialiasing
	enum class MultiSampling
	{
		MSAA_1X = 1, //Same as multisampling disabled
		MSAA_2X = 2,
		MSAA_4X = 4, //default
		MSAA_8X = 8,
		MSAA_16X = 16
	};
	enum class SRGBMode
	{
		SRGB_ENABLED, //Enable color-correct gamma rendering (default)
		SRGB_DISABLED //Colors will look washed out when using linear-space lighting
	};
	enum class ColorBufferBits
	{
		COLOR_RGBA8,   //8 bits per channel (default)
		COLOR_RGB10_A2 //10 bits color, 2 bits alpha (better color precision, sacrifices alpha quality)
	};
	enum class DepthBufferBits
	{
		DEPTH_16, //16-bit integer depth (saves VRAM, bad precision over large distances)
		DEPTH_24  //24-bit integer point depth (default)
	};
	enum class StencilBufferBits
	{
		STENCIL_NONE, //Disables stencil completely (default)
		STENCIL_8     //8-bit stencil
	};

	class OpenGL_Context;

	class LIB_API OpenGL_Global
	{
	public:
		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//GL notifications will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState);
		static bool IsVerboseLoggingEnabled();

		//Global one-time OpenGL 3.3 init, needs to be called before per-window OpenGL init.
		static void Initialize();
		static bool IsInitialized();
		
		static void SetOpenGLLibrary();
		static uintptr_t GetOpenGLLibrary();

		//Check if this extension is supported by the current context (OpenGL 3.3)
		static bool IsExtensionSupported(string_view name);

		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();

		//Shut down Global OpenGL and all contexts
        static void Shutdown();
	};

	class LIB_API OpenGL_Context
	{
	public:
		static KalaWindowRegistry<OpenGL_Context>& GetRegistry();

		//Initialize a per-window OpenGL context.
		//parentContext determines the ID of the parent context which
		//this context will get shaders, textures and buffers from
		static OpenGL_Context* Initialize(
			u32 windowID,
			u32 parentContext = 0,
			MultiSampling msaa = MultiSampling::MSAA_4X,
			SRGBMode srgb = SRGBMode::SRGB_ENABLED,
			ColorBufferBits cBits = ColorBufferBits::COLOR_RGBA8,
			DepthBufferBits dBits = DepthBufferBits::DEPTH_24,
			StencilBufferBits sBits = StencilBufferBits::STENCIL_NONE);

		bool IsInitialized() const;

		u32 GetID() const;
		u32 GetWindowID() const;

		const string& GetContextData();
		
		uintptr_t GetContext() const;
		uintptr_t GetParentContext() const;

		~OpenGL_Context();
	private:
		bool isInitialized{};

		u32 ID{};
		u32 windowID{};

		uintptr_t context{};
		uintptr_t parentContext{};

		string contextData{};
	};
}