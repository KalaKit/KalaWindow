//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "graphics/window.hpp"
#include "graphics/opengl/texture_opengl.hpp"
#include "graphics/opengl/shader_opengl.hpp"
#include "graphics/vulkan/texture_vulkan.hpp"
#include "graphics/vulkan/shader_vulkan.hpp"

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::OpenGL::Texture_OpenGL;
using KalaWindow::Graphics::OpenGL::Shader_OpenGL;
using KalaWindow::Graphics::Vulkan::Texture_Vulkan;
using KalaWindow::Graphics::Vulkan::Shader_Vulkan;

using std::string;
using std::unordered_map;
using std::vector;
using std::unique_ptr;

//
// INIT STAGE UNORDERED MAPS
//

static inline unordered_map<string, unique_ptr<Window>> createdWindows{};

static inline unordered_map<string, unique_ptr<Texture_OpenGL>> createdOpenGLTextures{};
static inline unordered_map<string, unique_ptr<Shader_OpenGL>> createdOpenGLShaders{};

static inline unordered_map<string, unique_ptr<Texture_Vulkan>> createdVulkanTextures{};
static inline unordered_map<string, unique_ptr<Shader_Vulkan>> createdVulkanShaders{};

//
// RUNTIME STAGE VECTORS
//

static inline vector<Window*> runtimeWindows{};

static inline vector<Texture_OpenGL*> runtimeOpenGLTextures{};
static inline vector<Shader_OpenGL*> runtimeOpenGLShaders{};

static inline vector<Texture_Vulkan*> runtimeVulkanTextures{};
static inline vector<Shader_Vulkan*> runtimeVulkanShaders{};