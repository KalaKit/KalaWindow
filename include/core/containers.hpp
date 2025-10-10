//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "KalaHeaders/core_utils.hpp"

#include "core/input.hpp"
#include "core/audio.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "windows/menubar.hpp"

namespace KalaWindow::Core
{
	using KalaWindow::Core::Input;
	using KalaWindow::Core::AudioPlayer;
	using KalaWindow::Graphics::Window;
	using KalaWindow::Windows::MenuBar;
	using KalaWindow::Windows::MenuBarEvent;
	using KalaWindow::Graphics::OpenGL::OpenGL_Context;
	using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;

	using std::string;
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;

	//Keeps track of highest ID to ensure each window,
	//shader and texture gets a unique ID in their map
	LIB_API extern u32 globalID;

	//
	// INIT STAGE UNORDERED MAPS
	//

	LIB_API extern unordered_map<u32, unique_ptr<Window>> createdWindows;

	LIB_API extern unordered_map<u32, unique_ptr<Input>> createdInput;

	LIB_API extern unordered_map<u32, unique_ptr<MenuBarEvent>> createdMenuBarEvents;

	LIB_API extern unordered_map<u32, unique_ptr<AudioPlayer>> createdAudioPlayers;

	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Context>> createdOpenGLContext;
	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Texture>> createdOpenGLTextures;
	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Shader>> createdOpenGLShaders;

	//
	// RUNTIME STAGE VECTORS (NON-OWNING, REFERENCE ONLY TO OWNERS ABOVE)
	//

	LIB_API extern vector<Window*> runtimeWindows;

	LIB_API extern vector<Input*> runtimeInput;

	LIB_API extern vector<MenuBarEvent*> runtimeMenuBarEvents;

	LIB_API extern vector<AudioPlayer*> runtimeAudioPlayers;

	LIB_API extern vector<OpenGL_Context*> runtimeOpenGLContext;
	LIB_API extern vector<OpenGL_Texture*> runtimeOpenGLTextures;
	LIB_API extern vector<OpenGL_Shader*> runtimeOpenGLShaders;

	//
	// GET VALUE FROM CONTAINER BY TYPE
	//

	template<typename T> struct ContainerOf;

	template<> struct ContainerOf<Window>         { static inline auto& get() { return createdWindows; } };
	template<> struct ContainerOf<Input>          { static inline auto& get() { return createdInput; } };
	template<> struct ContainerOf<MenuBarEvent>   { static inline auto& get() { return createdMenuBarEvents; } };
	template<> struct ContainerOf<AudioPlayer>    { static inline auto& get() { return createdAudioPlayers; } };
	template<> struct ContainerOf<OpenGL_Context> { static inline auto& get() { return createdOpenGLContext; } };
	template<> struct ContainerOf<OpenGL_Texture> { static inline auto& get() { return createdOpenGLTextures; } };
	template<> struct ContainerOf<OpenGL_Shader>  { static inline auto& get() { return createdOpenGLShaders; } };

	template<typename T>
	inline T* GetValueByID(u32 ID)
	{
		auto& container = ContainerOf<T>::get();

		if (auto it = container.find(ID); it != container.end())
		{
			return it->second.get();
		}

		return nullptr;
	}
}