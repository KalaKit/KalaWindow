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

	unordered_map<u32, unique_ptr<OpenGL_Context>> createdOpenGLContext{};
	unordered_map<u32, unique_ptr<OpenGL_Texture>> createdOpenGLTextures{};
	unordered_map<u32, unique_ptr<OpenGL_Shader>> createdOpenGLShaders{};

	//
	// RUNTIME STAGE VECTORS
	//

	vector<Window*> runtimeWindows{};

	vector<Input*> runtimeInput{};

	vector<MenuBarEvent*> runtimeMenuBarEvents;

	vector<AudioPlayer*> runtimeAudioPlayers{};

	vector<OpenGL_Context*> runtimeOpenGLContext{};
	vector<OpenGL_Texture*> runtimeOpenGLTextures{};
	vector<OpenGL_Shader*> runtimeOpenGLShaders{};
}