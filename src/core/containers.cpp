//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core/containers.hpp"

namespace KalaWindow::Core
{
	u32 globalID{};

	//
	// INIT STAGE UNORDERED MAPS
	//

	unordered_map<u32, unique_ptr<Window>> createdWindows{};

	unordered_map<u32, unique_ptr<AudioTrack>> createdAudioTracks{};

	unordered_map<u32, unique_ptr<Texture_OpenGL>> createdOpenGLTextures{};
	unordered_map<u32, unique_ptr<Shader_OpenGL>> createdOpenGLShaders{};

	unordered_map<u32, unique_ptr<Texture_Vulkan>> createdVulkanTextures{};
	unordered_map<u32, unique_ptr<Shader_Vulkan>> createdVulkanShaders{};

	//
	// RUNTIME STAGE VECTORS
	//

	vector<Window*> runtimeWindows{};

	vector<Texture_OpenGL*> runtimeOpenGLTextures{};
	vector<Shader_OpenGL*> runtimeOpenGLShaders{};

	vector<Texture_Vulkan*> runtimeVulkanTextures{};
	vector<Shader_Vulkan*> runtimeVulkanShaders{};
}