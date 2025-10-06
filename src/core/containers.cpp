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

	unordered_map<u32, unique_ptr<Input>> createdInput{};

	unordered_map<u32, unique_ptr<MenuBarEvent>> createdMenuBarEvents;

	unordered_map<u32, unique_ptr<AudioPlayer>> createdAudioPlayers{};

	unordered_map<u32, unique_ptr<OpenGL_Texture>> createdOpenGLTextures{};
	unordered_map<u32, unique_ptr<OpenGL_Shader>> createdOpenGLShaders{};

	unordered_map<u32, unique_ptr<DebugUI>> createdUI{};

	//unordered_map<u32, unique_ptr<Texture_Vulkan>> createdVulkanTextures{};
	//unordered_map<u32, unique_ptr<Shader_Vulkan>> createdVulkanShaders{};

	//
	// RUNTIME STAGE VECTORS
	//

	vector<Window*> runtimeWindows{};

	vector<Input*> runtimeInput{};

	vector<MenuBarEvent*> runtimeMenuBarEvents;

	vector<AudioPlayer*> runtimeAudioPlayers{};

	vector<OpenGL_Texture*> runtimeOpenGLTextures{};
	vector<OpenGL_Shader*> runtimeOpenGLShaders{};

	vector<DebugUI*> runtimeUI{};

	//vector<Texture_Vulkan*> runtimeVulkanTextures{};
	//vector<Shader_Vulkan*> runtimeVulkanShaders{};
}