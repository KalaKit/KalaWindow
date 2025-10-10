//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>

#include "KalaHeaders/core_utils.hpp"

namespace KalaWindow::Graphics::OpenGL
{
	using std::string;

	enum VSyncState
	{
		VSYNC_ON, //Framerate is capped to monitor refresh rate.
		VSYNC_OFF //Framerate is uncapped, runs as fast as render loop allows, introduces tearing.
	};

	//Hardware accelerated antialiasing
	enum MultiSampling
	{
		MSAA_1X = 1, //Same as multisampling disabled
		MSAA_2X = 2,
		MSAA_4X = 4, //default
		MSAA_8X = 8,
		MSAA_16X = 16
	};
	enum SRGBMode
	{
		SRGB_ENABLED, //Enable color-correct gamma rendering (default)
		SRGB_DISABLED //Colors will look washed out when using linear-space lighting
	};
	enum ColorBufferBits
	{
		COLOR_RGBA8,   //8 bits per channel (default)
		COLOR_RGB10_A2 //10 bits color, 2 bits alpha (better color precision, sacrifices alpha quality)
	};
	enum DepthBufferBits
	{
		DEPTH_16, //16-bit integer depth (saves VRAM, bad precision over large distances)
		DEPTH_24  //24-bit integer point depth (default)
	};
	enum StencilBufferBits
	{
		STENCIL_NONE, //Disables stencil completely (default)
		STENCIL_8     //8-bit stencil
	};
	enum AlphaChannel
	{
		ALPHA_NONE, //Disables alpha channel completely, cannot have transparent meshes or textures
		ALPHA_8     //8-bit alpha channel (default)
	};

	class LIB_API OpenGL_Global
	{
	public:
		//Global one-time OpenGL 3.3 init, needs to be called before per-window OpenGL init
		static bool Initialize();
		static inline bool IsInitialized() { return isInitialized; }

		static void SetOpenGLLibrary();
		static inline uintptr_t GetOpenGLLibrary()
		{
			if (openGL32Lib == NULL) SetOpenGLLibrary();

			return openGL32Lib;
		}

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//GL notifications will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		//Check if this extension is supported by the current context (OpenGL 3.3)
		static bool IsExtensionSupported(const string& name);

		//Place after any gl call to check if an issue or error has occurred within that point.
		//Loops through all errors so that all errors at that point are printed, not just the first one.
		static string GetError();
	private:
		static inline bool isInitialized{};
		static inline bool isVerboseLoggingEnabled{};

		static inline uintptr_t openGL32Lib{};
		uintptr_t hglrc{}; //master context for shared resources
	};

	class LIB_API OpenGL_Context
	{
	public:
		//Initialize a per-window OpenGL context.
		//parentContext determines the ID of the parent context which
		//this context will get shaders, textures and buffers from
		static OpenGL_Context* Initialize(
			u32 windowID,
			u32 parentContext,
			MultiSampling msaa = MultiSampling::MSAA_4X,
			SRGBMode srgb = SRGBMode::SRGB_ENABLED,
			ColorBufferBits cBits = ColorBufferBits::COLOR_RGBA8,
			DepthBufferBits dBits = DepthBufferBits::DEPTH_24,
			StencilBufferBits sBits = StencilBufferBits::STENCIL_NONE,
			AlphaChannel aChannel = AlphaChannel::ALPHA_8);

		inline bool IsInitialized() const { return isInitialized; }

		inline u32 GetID() const { return ID; }

		inline const string& GetContextData() { return contextData; }

		void SwapOpenGLBuffers() const;

		void MakeContextCurrent() const;
		bool IsContextValid() const;

		void SetVSyncState(VSyncState vsyncState);
		inline VSyncState GetVSyncState() const { return vsyncState; }

		inline void SetParent(OpenGL_Context* newVal) { parentContext = newVal; }
		inline OpenGL_Context* GetParent() const { return parentContext; }

		inline void SetContext(const uintptr_t& newVal) { hglrc = newVal; }
		inline const uintptr_t& GetContext() const { return hglrc; }

		inline void SetHandle(const uintptr_t& newVal) { hdc = newVal; }
		inline const uintptr_t& GetHandle() const { return hdc; }

		inline void SetLastProgramID(u32 newID) { lastProgramID = newID; }
		inline const u32 GetLastProgramID() const { return lastProgramID; }

		//Do not destroy manually, erase from containers.hpp instead
		~OpenGL_Context();
	private:
#ifdef _WIN32
		bool isInitialized{};

		u32 ID{};
		u32 windowID{};

		OpenGL_Context* parentContext{};

		uintptr_t hglrc{}; //OpenGL context wia WGL
		uintptr_t hdc{};   //OpenGL handle to device context
#else
		uintptr_t glxContext{}; //OpenGL context via glx
#endif
		u32 lastProgramID{};

		string contextData{};

		MultiSampling msaa{};
		SRGBMode srgb{};
		ColorBufferBits cBits{};
		DepthBufferBits dBits{};
		StencilBufferBits sBits{};
		AlphaChannel aChannel{};

		VSyncState vsyncState = VSyncState::VSYNC_ON;
	};
}