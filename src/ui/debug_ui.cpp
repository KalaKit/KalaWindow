//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>
#include <vector>
#include <filesystem>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#ifdef _WIN32
#include "imgui/backends/imgui_impl_win32.h"
#else
//TODO: DEFINE
#endif

#include "KalaHeaders/logging.hpp"

#include "ui/debug_ui.hpp"
#include "core/containers.hpp"
#include "graphics/window.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::createdWindows;
using KalaWindow::Core::runtimeWindows;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;

using std::vector;
using std::to_string;
using std::filesystem::exists;
using std::filesystem::is_regular_file;
using std::filesystem::path;
using std::filesystem::current_path;

static ImGuiContext* context{};

static vector<u32> createdIndexes{};

namespace KalaWindow::UI
{
	bool DebugUI::Initialize(
		bool enableDocking,
		const vector<UserFont>& userProvidedFonts)
	{
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize ImGui more than once!",
				"DEBUG_UI",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (runtimeWindows.empty())
		{
			Log::Print(
				"Failed to initialize ImGui! No window has been created.",
				"DEBUG_UI",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		IMGUI_CHECKVERSION();
		context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);

		ImGuiIO& io = ImGui::GetIO();
		if (enableDocking) io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		io.Fonts->AddFontDefault();

		for (const auto& val : userProvidedFonts)
		{
			string keyPath = (current_path() / val.fontPath).string();

			Log::Print(
				"Attempting to load font '" + keyPath + "' with size '" + to_string(val.fontSize),
				"DEBUG_UI",
				LogType::LOG_DEBUG);

			if (!exists(keyPath))
			{
				Log::Print(
					"Failed to load font '" + keyPath + "' because it does not exist!",
					"DEBUG_UI",
					LogType::LOG_ERROR,
					2);

				continue;
			}
			if (!is_regular_file(keyPath))
			{
				Log::Print(
					"Failed to load font '" + keyPath + "' because it is not a font file!",
					"DEBUG_UI",
					LogType::LOG_ERROR,
					2);

				continue;
			}
			if (path(keyPath).extension().string() != ".ttf"
				&& path(keyPath).extension().string() != ".otf")
			{
				Log::Print(
					"Failed to load font '" + keyPath + "' because its extension is not correct for a font file!",
					"DEBUG_UI",
					LogType::LOG_ERROR,
					2);

				continue;
			}
			if (val.fontSize < 5.0f
				|| val.fontSize > 150.0f)
			{
				Log::Print(
					"Failed to load font '" + keyPath + "' because its size '" + to_string(val.fontSize) + "' is out of range! Pick a range from 5 to 150.",
					"DEBUG_UI",
					LogType::LOG_ERROR,
					2);

				continue;
			}

			ImFont* newFont = io.Fonts->AddFontFromFileTTF(keyPath.c_str(), val.fontSize);

			if (!newFont)
			{
				Log::Print(
					"Failed to load font '" + keyPath + "' because ImGui couldn't load it!",
					"DEBUG_UI",
					LogType::LOG_ERROR,
					2);

				continue;
			}

			UserFont newFontData =
			{
				.fontName = val.fontName,
				.fontPointer = newFont,
				.fontPath = val.fontPath,
				.fontSize = val.fontSize
			};

			userFonts[val.fontName] = newFontData;
		}

		if (userFonts.size() > 0)
		{
			Log::Print(
				"Loaded '" + to_string(userFonts.size()) + "' fonts!",
				"DEBUG_UI",
				LogType::LOG_SUCCESS);
		}

#ifdef _WIN32
		const WindowData& wData = runtimeWindows[0]->GetWindowData();
		HWND hwnd = ToVar<HWND>(wData.hwnd);
		ImGui_ImplWin32_Init(hwnd);
#else
		//TODO: DEFINE
#endif
		ImGui_ImplOpenGL3_Init("#version 330");

		Log::Print(
			"Initialized ImGui!",
			"DEBUG_UI",
			LogType::LOG_SUCCESS);

		isInitialized = true;
		return true;
	}

	vec2 DebugUI::CenterWindow(
		vec2 size,
		u32 windowID)
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot center window because ImGui is not initialized!",
				"DEBUG_UI",
				LogType::LOG_ERROR,
				2);

			return vec2();
		}

		Window* win = createdWindows[windowID].get();

		if (win == nullptr)
		{
			Log::Print(
				"Cannot center ImGui window because the window ID is invalid!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return vec2();
		}

		vec2 rectSize = win->GetClientRectSize();

		float posX = (rectSize.x - size.x) / 2.0f;
		float posY = (rectSize.y - size.y) / 2.0f;

		return vec2(posX, posY);
	}

	void DebugUI::RenderWindow(
		u32 ID,
		WindowSettings settings,
		function<void()> func,
		const string& title,
		vec2 size,
		vec2 pos,
		vec2 minSize,
		vec2 maxSize)
	{
		if (find(createdIndexes.begin(), createdIndexes.end(), ID) != createdIndexes.end())
		{
			Log::Print(
				"Failed to create ImGui window '" + title + "' because its ID '" + to_string(ID) + "' has already been used by another ImGui window!",
				"IMGUI",
				LogType::LOG_ERROR,
				2);

			return;
		}

		ImGuiCond cond = settings.isMovable
			? ImGuiCond_FirstUseEver
			: ImGuiCond_Always;

		ImGui::SetNextWindowPos(
			ImVec2(pos.x, pos.y),
			cond);

		ImGui::SetNextWindowSize(
			ImVec2(size.x, size.y),
			settings.isResizable
			? ImGuiCond_FirstUseEver
			: ImGuiCond_Always);

		if (minSize != vec2(0)
			&& maxSize != vec2(0))
		{
			if (minSize.x > maxSize.x
				|| minSize.y > maxSize.y)
			{
				Log::Print(
					"Window '" + title + "' max size cannot be lower than window min size! Size constraints are ignored.",
					"IMGUI",
					LogType::LOG_ERROR,
					2);
			}
			else
			{
				ImGui::SetNextWindowSizeConstraints(
					ImVec2(minSize.x, minSize.y),
					ImVec2(maxSize.x, maxSize.y));
			}
		}

		ImGuiWindowFlags flags = 0;

		if (!settings.isMovable)     flags |= ImGuiWindowFlags_NoMove;
		if (!settings.isResizable)   flags |= ImGuiWindowFlags_NoResize;
		if (!settings.isCollapsible) flags |= ImGuiWindowFlags_NoCollapse;
		if (!settings.hasToolbar)    flags |= ImGuiWindowFlags_NoTitleBar;
		if (!settings.saveSettings)  flags |= ImGuiWindowFlags_NoSavedSettings;
		if (settings.noFocus)        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		if (settings.topMost) ImGui::SetNextWindowFocus();

		if (ImGui::Begin((title + "##" + to_string(ID)).c_str(), NULL, flags))
		{
			if (!func) ImGui::Text("This window has no content.");
			else       func();
		}
		ImGui::End();
	}

	void DebugUI::RenderModalWindow(
		u32 ID,
		u32 windowID,
		function<void()> func,
		const string& title,
		vec2 size)
	{
		if (find(createdIndexes.begin(), createdIndexes.end(), ID) != createdIndexes.end())
		{
			Log::Print(
				"Failed to create ImGui window '" + title + "' because its ID '" + to_string(ID) + "' has already been used by another ImGui window!",
				"IMGUI",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (createdWindows.find(windowID) == createdWindows.end())
		{
			Log::Print(
				"Failed to create ImGui window '" + title + "' because window ID '" + to_string(windowID) + "' does not match any existing user-created executable window ID!",
				"IMGUI",
				LogType::LOG_ERROR,
				2);

			return;
		}

		vec2 center = DebugUI::CenterWindow(
			vec2(size.x, size.y),
			windowID);

		ImGui::SetNextWindowPos(
			ImVec2(center.x, center.y),
			ImGuiCond_Always);

		ImVec2 finalSize = size != vec2()
			? ImVec2(size.x, size.y)
			: ImVec2(300.0f, 200.0f);
		ImGui::SetNextWindowSize(
			finalSize,
			ImGuiCond_Always);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiTableFlags_NoSavedSettings;

		if (ImGui::Begin((title + "##" + to_string(ID)).c_str(), NULL, flags))
		{
			if (!func) ImGui::Text("This window has no content.");
			else       func();
		}
		ImGui::End();
	}

	void DebugUI::Render(u32 windowID)
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot run ImGui because ImGui is not initialized!",
				"DEBUG_UI",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Window* win = createdWindows[windowID].get();

		if (win == nullptr)
		{
			Log::Print(
				"Cannot render ImGui because the window ID is invalid!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
		}

		ImGui_ImplOpenGL3_NewFrame();
#ifdef _WIN32
		ImGui_ImplWin32_NewFrame();
#elif __linux__
		//TODO: DEFINE
#endif
		ImGui::NewFrame();

		if (topBarFunction != NULL) topBarFunction();

		if (isDockingEnabled)
		{
			ImGuiDockNodeFlags flags =
				ImGuiDockNodeFlags_PassthruCentralNode;

			ImGui::DockSpaceOverViewport(
				0,
				ImGui::GetMainViewport(),
				flags,
				nullptr);
		}

		if (mainRenderFunction != NULL) mainRenderFunction();

		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();
		if (drawData != nullptr) ImGui_ImplOpenGL3_RenderDrawData(drawData);
	}

	void DebugUI::Shutdown()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot shut down ImGui because ImGui is not initialized!",
				"DEBUG_UI",
				LogType::LOG_ERROR,
				2);

			return;
		}

		isInitialized = false;

		context = nullptr;

		ImGui_ImplOpenGL3_Shutdown();
#ifdef _WIN32
		ImGui_ImplWin32_Shutdown();
#elif __linux__
		//TODO: DEFINE
#endif
		ImGui::DestroyContext();

		Log::Print(
			"Shut down ImGui!",
			"DEBUG_UI",
			LogType::LOG_SUCCESS);
	}
}