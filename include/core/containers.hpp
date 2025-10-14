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

#include "windows/menubar.hpp"
#include "graphics/window.hpp"
#include "core/input.hpp"
#include "core/audio.hpp"
#include "graphics/camera.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/opengl/opengl_texture.hpp"
#include "graphics/opengl/opengl_shader.hpp"
#include "ui/font.hpp"
#include "ui/widget.hpp"
#include "ui/text.hpp"
#include "ui/image.hpp"

namespace KalaWindow::Core
{
	using KalaWindow::Graphics::Window;
	using KalaWindow::Windows::MenuBar;
	using KalaWindow::Windows::MenuBarEvent;
	using KalaWindow::Core::Input;
	using KalaWindow::Core::AudioPlayer;
	using KalaWindow::Graphics::Camera;
	using KalaWindow::Graphics::OpenGL::OpenGL_Context;
	using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
	using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
	using KalaWindow::UI::Font;
	using KalaWindow::UI::Widget;
	using KalaWindow::UI::Text;
	using KalaWindow::UI::Image;

	using std::string;
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;

	//Keeps track of highest ID to ensure each window,
	//shader and texture gets a unique ID in their map
	LIB_API extern u32 globalID;

	struct LIB_API WindowContent
	{
		Window* parentWindow{};
		vector<Window*> childWindows{};

		unique_ptr<Input> input{};
		unique_ptr<OpenGL_Context> glContext{};

		unique_ptr<MenuBar> menubar{};

		unordered_map<u32, unique_ptr<MenuBarEvent>> menuBarEvents{};
		vector<MenuBarEvent*> runtimeMenuBarEvents{};

		unordered_map<u32, unique_ptr<AudioPlayer>> audioPlayers{};
		vector<AudioPlayer*> runtimeAudioPlayers{};

		unordered_map<u32, unique_ptr<Camera>> cameras{};
		vector<Camera*> runtimeCameras{};

		unordered_map<u32, unique_ptr<Widget>> widgets{};
		vector<Widget*> runtimeWidgets{};

		vector<Text*> runtimeText{};
		vector<Image*> runtimeImages{};

		template<typename T> struct ContainerOf;

		template<> struct ContainerOf<MenuBarEvent> { static inline auto& get(WindowContent& c) { return c.menuBarEvents; } };
		template<> struct ContainerOf<AudioPlayer>  { static inline auto& get(WindowContent& c) { return c.audioPlayers; } };
		template<> struct ContainerOf<Camera>       { static inline auto& get(WindowContent& c) { return c.cameras; } };
		template<> struct ContainerOf<Widget>       { static inline auto& get(WindowContent& c) { return c.widgets; } };

		template<typename T>
		T* GetValueByID(u32 ID)
		{
			auto& container = ContainerOf<T>::get(*this);

			if (auto it = container.find(ID); it != container.end())
			{
				return it->second.get();
			}

			return nullptr;
		}

		~WindowContent()
		{
			parentWindow = nullptr;
			childWindows.clear();

			widgets.clear();
			runtimeWidgets.clear();
			runtimeText.clear();
			runtimeImages.clear();

			menuBarEvents.clear();
			runtimeMenuBarEvents.clear();

			audioPlayers.clear();
			runtimeAudioPlayers.clear();

			cameras.clear();
			runtimeCameras.clear();

			input = nullptr;
			
			menubar = nullptr;

			glContext = nullptr;
		}
	};

	//
	// INIT STAGE UNORDERED MAPS
	//

	LIB_API extern unordered_map<u32, unique_ptr<Window>> createdWindows;
	LIB_API extern unordered_map<Window*, unique_ptr<WindowContent>> windowContent;

	LIB_API extern unordered_map<u32, unique_ptr<Font>> createdFonts;

	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Texture>> createdOpenGLTextures;
	LIB_API extern unordered_map<u32, unique_ptr<OpenGL_Shader>> createdOpenGLShaders;

	//
	// RUNTIME STAGE VECTORS (NON-OWNING, REFERENCE ONLY TO OWNERS ABOVE)
	//

	LIB_API extern vector<Window*> runtimeWindows;
	LIB_API extern vector<WindowContent*> runtimeWindowContent;

	LIB_API extern vector<Font*> runtimeFonts;

	LIB_API extern vector<OpenGL_Texture*> runtimeOpenGLTextures;
	LIB_API extern vector<OpenGL_Shader*> runtimeOpenGLShaders;

	//
	// ADD/REMOVE/GET WINDOW
	//

	//Returns true if target window is connected
	//to current window as a child, parent or sibling.
	//Set recursive to true if you want deep window search
	inline bool HasWindow(
		Window* currentWindow,
		Window* targetWindow,
		bool recursive = false)
	{
		if (!currentWindow
			|| !targetWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		if (currentWindow == targetWindow) return true;

		//check descendants
		for (auto* c : windowContent[currentWindow]->childWindows)
		{
			if (c == targetWindow) return true;

			if (recursive
				&& HasWindow(c, targetWindow, true))
			{
				return true;
			}
		}

		//check ancestors

		Window* parent = windowContent[currentWindow]->parentWindow;
		if (parent)
		{
			if (parent == targetWindow) return true;

			if (recursive
				&& HasWindow(parent, targetWindow, true))
			{
				return true;
			}
		}

		return false;
	}

	inline bool IsParentWindow(
		Window* currentWindow, 
		Window* targetWindow,
		bool recursive = false)
	{
		if (!currentWindow
			|| !targetWindow
			|| currentWindow == targetWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		Window* parent = windowContent[currentWindow]->parentWindow;
		if (!parent) return false;

		if (parent == targetWindow) return true;

		if (recursive
			&& IsParentWindow(parent, targetWindow, true))
		{
			return true;
		}

		return false;
	}
	inline Window* GetParentWindow(Window* currentWindow)
	{
		if (!currentWindow
			|| !windowContent.contains(currentWindow))
		{
			return nullptr;
		}

		return windowContent[currentWindow]->parentWindow;
	}
	inline bool SetParentWindow(
		Window* currentWindow, 
		Window* targetWindow)
	{
		if (!currentWindow
			|| !targetWindow
			|| currentWindow == targetWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		if (HasWindow(currentWindow, targetWindow, true)
			|| HasWindow(targetWindow, currentWindow, true))
		{
			return false;
		}

		windowContent[currentWindow]->parentWindow = targetWindow;
		return true;
	}
	inline bool RemoveParentWindow(Window* currentWindow)
	{
		if (!currentWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		windowContent[currentWindow]->parentWindow = nullptr;

		return true;
	}

	inline bool IsChildWindow(
		Window* currentWindow,
		Window* targetWindow,
		bool recursive = false)
	{
		if (!currentWindow
			|| !targetWindow
			|| currentWindow == targetWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		for (auto* c : windowContent[currentWindow]->childWindows)
		{
			if (c == targetWindow) return true;

			if (recursive
				&& IsChildWindow(c, targetWindow, true))
			{
				return true;
			}
		}

		return false;
	}
	inline bool AddChildWindow(
		Window* currentWindow, 
		Window* targetWindow)
	{
		if (!currentWindow
			|| !targetWindow
			|| currentWindow == targetWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		if (HasWindow(currentWindow, targetWindow, true)
			|| HasWindow(targetWindow, currentWindow, true))
		{
			return false;
		}

		windowContent[currentWindow]->childWindows.push_back(targetWindow);
		return true;
	}
	inline bool RemoveChildWindow(
		Window* currentWindow, 
		Window* targetWindow)
	{
		if (!currentWindow
			|| !targetWindow
			|| currentWindow == targetWindow
			|| !windowContent.contains(currentWindow)
			|| windowContent[currentWindow]->parentWindow == targetWindow)
		{
			return false;
		}

		vector<Window*>& childWindows = windowContent[currentWindow]->childWindows;

		childWindows.erase(remove(
			childWindows.begin(),
			childWindows.end(),
			targetWindow),
			childWindows.end());

		return true;
	}

	inline const vector<Window*>& GetAllChildWindows(Window* currentWindow)
	{
		if (!currentWindow
			|| !windowContent.contains(currentWindow))
		{
			return{};
		}

		return windowContent[currentWindow]->childWindows;
	}
	inline bool RemoveAllChildWindows(Window* currentWindow)
	{
		if (!currentWindow
			|| !windowContent.contains(currentWindow))
		{
			return false;
		}

		vector<Window*>& childWindows = windowContent[currentWindow]->childWindows;

		for (auto* c : childWindows)
		{
			windowContent[currentWindow]->parentWindow = nullptr;
		}
		childWindows.clear();
	}

	//
	// GET VALUE FROM CONTAINER BY TYPE
	//

	template<typename T> struct ContainerOf;

	template<> struct ContainerOf<Window>         { static inline auto& get() { return createdWindows; } };
	template<> struct ContainerOf<Font>           { static inline auto& get() { return createdFonts; } };
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