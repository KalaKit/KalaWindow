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

#include "core/audio.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "graphics/vulkan/vulkan_texture.hpp"
#include "graphics/vulkan/vulkan_shader.hpp"

namespace KalaWindow::Core
{
	using KalaWindow::Core::AudioPlayer;
	using KalaWindow::Graphics::Window;
	using KalaWindow::Graphics::MenuBar;
	using KalaWindow::Graphics::MenuBarEvent;
	using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
	//using KalaWindow::Graphics::Vulkan::Texture_Vulkan;
	//using KalaWindow::Graphics::Vulkan::Shader_Vulkan;

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

	LIB_API extern unordered_map<u32, unique_ptr<MenuBarEvent>> createdMenuBarEvents;

	LIB_API extern unordered_map<u32, unique_ptr<AudioPlayer>> createdAudioPlayers;

	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Texture>> createdOpenGLTextures;
	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Shader>> createdOpenGLShaders;

	//LIB_API extern unordered_map<u32, unique_ptr<Texture_Vulkan>> createdVulkanTextures;
	//LIB_API extern unordered_map<u32, unique_ptr<Shader_Vulkan>> createdVulkanShaders;

	//
	// RUNTIME STAGE VECTORS (NON-OWNING, REFERENCE ONLY TO OWNERS ABOVE)
	//

	LIB_API extern vector<Window*> runtimeWindows;

	LIB_API extern vector<MenuBarEvent*> runtimeMenuBarEvents;

	LIB_API extern vector<AudioPlayer*> runtimeAudioPlayers;

	LIB_API extern vector<OpenGL_Texture*> runtimeOpenGLTextures;
	LIB_API extern vector<OpenGL_Shader*> runtimeOpenGLShaders;

	//LIB_API extern vector<Texture_Vulkan*> runtimeVulkanTextures;
	//LIB_API extern vector<Shader_Vulkan*> runtimeVulkanShaders;
}