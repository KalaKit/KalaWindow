//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>
#include <vector>
#include <filesystem>
#include <sstream>
#include <memory>

#include "imgui/backends/imgui_impl_opengl3.h"
#ifdef _WIN32
#include "imgui/backends/imgui_impl_win32.h"
#else
//TODO: DEFINE
#endif

#include "KalaHeaders/log_utils.hpp"

#include "ui/debug_ui.hpp"
#include "core/core.hpp"
#include "core/containers.hpp"
#include "graphics/window.hpp"
#include "graphics/opengl/opengl.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using namespace KalaWindow::Core;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Graphics::OpenGL::OpenGL_Global;

using std::vector;
using std::to_string;
using std::filesystem::exists;
using std::filesystem::is_regular_file;
using std::filesystem::path;
using std::filesystem::current_path;
using std::setprecision;
using std::fixed;
using std::ostringstream;
using std::unique_ptr;
using std::make_unique;

static vector<u32> createdIndexes{};

namespace KalaWindow::UI
{
	DebugUI* DebugUI::Initialize(
		u32 windowID,
		bool enableDocking,
		const vector<UserFont>& userProvidedFonts)
	{
		if (!OpenGL_Global::IsInitialized())
		{
			KalaWindowCore::ForceClose(
				"UI error",
				"Failed to initialize ImGui because OpenGL has not been initialized!");

			return nullptr;
		}

		Window* window = GetValueByID<Window>(windowID);

		if (!window)
		{
			KalaWindowCore::ForceClose(
				"UI error",
				"Failed to initialize ImGui because its window was not found!");

			return nullptr;
		}

		OpenGL_Context* cont = GetValueByID<OpenGL_Context>(window->GetOpenGLID());

		if (!cont
			|| (cont
			&& !cont->IsContextValid()))
		{
			KalaWindowCore::ForceClose(
				"UI error",
				"Cannot initialize ImGui context because window '" + window->GetTitle() + "' has no valid OpenGL context!");

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<DebugUI> newDebugUI = make_unique<DebugUI>();
		DebugUI* debugPtr = newDebugUI.get();

		debugPtr->ID = newID;

		IMGUI_CHECKVERSION();
		debugPtr->context = ImGui::CreateContext();
		ImGui::SetCurrentContext(debugPtr->context);

		ImGuiIO& io = ImGui::GetIO();
		if (enableDocking) io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		io.Fonts->AddFontDefault();

		for (const auto& val : userProvidedFonts)
		{
			string keyPath = (current_path() / val.fontPath).string();

			ostringstream oss{};
			oss << "Attempting to load font '" << keyPath 
				<< "' with size '" << fixed << setprecision(2) << val.fontSize << "'";

			Log::Print(
				oss.str(),
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

			debugPtr->userFonts[val.fontName] = newFontData;
		}

		if (debugPtr->userFonts.size() > 0)
		{
			Log::Print(
				"Loaded '" + to_string(debugPtr->userFonts.size()) + "' fonts!",
				"DEBUG_UI",
				LogType::LOG_SUCCESS);
		}

#ifdef _WIN32
		const WindowData& wData = window->GetWindowData();
		HWND hwnd = ToVar<HWND>(wData.hwnd);
		ImGui_ImplWin32_Init(hwnd);
#else
		//TODO: DEFINE
#endif
		ImGui_ImplOpenGL3_Init("#version 330");

		createdUI[newID] = move(newDebugUI);
		runtimeUI.push_back(debugPtr);

		window->SetDebugUIID(debugPtr->ID);
		debugPtr->windowID = window->GetID();

		debugPtr->isInitialized = true;

		Log::Print(
			"Created debug ui context with ID '" + to_string(newID) + "'!",
			"DEBUG_UI",
			LogType::LOG_SUCCESS);

		return debugPtr;
	}

	vec2 DebugUI::CenterWindow(
		u32 windowID,
		vec2 size)
	{
		Window* window = GetValueByID<Window>(windowID);

		if (!window)
		{
			Log::Print(
				"Cannot center window because its window was not found!",
				"DEBUG_UI",
				LogType::LOG_ERROR,
				2);

			return vec2();
		}

		vec2 rectSize = window->GetClientRectSize();

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
		u32 windowID,
		u32 ID,
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

		Window* window = GetValueByID<Window>(windowID);

		if (!window)
		{
			Log::Print(
				"Cannot render modal window because its window was not found!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR,
				2);

			return;
		}

		vec2 center = DebugUI::CenterWindow(
			windowID,
			vec2(size.x, size.y));

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

		Window* window = GetValueByID<Window>(windowID);

		if (!window)
		{
			Log::Print(
				"Cannot render ImGui because its window was not found!",
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

	DebugUI::~DebugUI()
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