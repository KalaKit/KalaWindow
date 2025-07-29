//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "core/platform.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/texture_opengl.hpp"
#include "graphics/opengl/shader_opengl.hpp"
#include "graphics/vulkan/texture_vulkan.hpp"
#include "graphics/vulkan/shader_vulkan.hpp"

namespace KalaWindow::Core
{
	using KalaWindow::Graphics::Window;
	using KalaWindow::Graphics::OpenGL::Texture_OpenGL;
	using KalaWindow::Graphics::OpenGL::Shader_OpenGL;
	using KalaWindow::Graphics::Vulkan::Texture_Vulkan;
	using KalaWindow::Graphics::Vulkan::Shader_Vulkan;

	using std::string;
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;

	//Keeps track of highest ID to ensure each window,
	//shader and texture gets a unique ID in their map
	static inline u32 globalID;

	//
	// INIT STAGE UNORDERED MAPS
	//

	extern KALAWINDOW_API unordered_map<u32, unique_ptr<Window>> createdWindows;

	extern KALAWINDOW_API unordered_map<u32, unique_ptr<Texture_OpenGL>> createdOpenGLTextures;
	extern KALAWINDOW_API unordered_map<u32, unique_ptr<Shader_OpenGL>> createdOpenGLShaders;

	extern KALAWINDOW_API unordered_map<u32, unique_ptr<Texture_Vulkan>> createdVulkanTextures;
	extern KALAWINDOW_API unordered_map<u32, unique_ptr<Shader_Vulkan>> createdVulkanShaders;

	//
	// RUNTIME STAGE VECTORS
	//

	extern KALAWINDOW_API vector<Window*> runtimeWindows;

	extern KALAWINDOW_API vector<Texture_OpenGL*> runtimeOpenGLTextures;
	extern KALAWINDOW_API vector<Shader_OpenGL*> runtimeOpenGLShaders;

	extern KALAWINDOW_API vector<Texture_Vulkan*> runtimeVulkanTextures;
	extern KALAWINDOW_API vector<Shader_Vulkan*> runtimeVulkanShaders;
}