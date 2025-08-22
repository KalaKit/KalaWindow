//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32
#include <Windows.h>
#else

#endif

#include <vector>
#include <string>
#include <algorithm>

#include "OpenGL/glcorearb.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "ui/debug_ui.hpp"
#include "graphics/window.hpp"
#include "core/containers.hpp"

using KalaWindow::UI::DebugUI;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Core::runtimeWindows;

using KalaHeaders::Log;
using KalaHeaders::LogType;

using std::vector;
using std::string;
using std::clamp;

static vector<string> loadedFonts{};

static bool CheckInitState(const string& targetAction);

namespace KalaWindow::UI
{
	bool DebugUI::Initialize()
	{
		if (runtimeWindows.empty())
		{
			Log::Print(
				"Failed to initialize ImGui! No window has been created.",
				"DEBUG_UI",
				LogType::LOG_ERROR);

			return false;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		//ImGui_ImplGlfw_InitForOpenGL(Render::window, true);

#ifdef _WIN32
		const WindowData& wData = runtimeWindows[0]->GetWindowData();
		HWND hwnd = ToVar<HWND>(wData.hwnd);
		ImGui_ImplWin32_Init(hwnd);
#else

#endif
		ImGui_ImplOpenGL3_Init("#version 330");

		Log::Print(
			"Initialized ImGui!",
			"DEBUG_UI",
			LogType::LOG_SUCCESS);

		isInitialized = true;
		return true;
	}

	void DebugUI::AddFont(
		const string& fontPath,
		float fontSize)
	{
		if (!CheckInitState("add font '" + fontPath + "'")) return;

		if (find(loadedFonts.begin(), loadedFonts.end(), fontPath) != loadedFonts.end())
		{
			Log::Print(
				"Font with path '" + fontPath + "' has already been loaded!",
				"DEBUG_UI",
				LogType::LOG_ERROR);

			return;
		}

		float clamped = clamp(fontSize, 10.0f, 72.0f);

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(
			fontPath.c_str(),
			clamped);

		Log::Print(
			"Loaded new font '" + fontPath + "'!",
			"DEBUG_UI",
			LogType::LOG_SUCCESS);
	}

	void DebugUI::ClearFonts()
	{
		if (!CheckInitState("clear ImGui fonts")) return;

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();

		loadedFonts.clear();
	}

	void DebugUI::UpdateStyle()
	{
		if (!CheckInitState("update ImGui")) return;
	}

	vec2 DebugUI::CenterWindow(vec2 size)
	{
		if (!CheckInitState("center ImGui window")) return vec2();

		int width{};
		int height{};

		//TODO: CREATE FRAMEBUFFER GETTER
		//TODO: DEFINE PFNGLGETINTEGERVPROC

		float posX = (static_cast<float>(width) - size.x) / 2.0f;
		float posY = (static_cast<float>(height) - size.y) / 2.0f;

		return vec2(posX, posY);
	}

	void DebugUI::Render(
		const function<void()>& userRenderContent,
		const function<void()>& userTopBar)
	{
		if (!CheckInitState("render ImGui")) return;

		ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (userTopBar != NULL) userTopBar();

		ImGuiDockNodeFlags flags =
			ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(
			0,
			ImGui::GetMainViewport(),
			flags,
			nullptr);

		if (userRenderContent != NULL) userRenderContent();

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		if (drawData != nullptr) ImGui_ImplOpenGL3_RenderDrawData(drawData);
	}

	void DebugUI::Shutdown()
	{
		if (!CheckInitState("shut down ImGui")) return;

		isInitialized = false;

		ImGui_ImplOpenGL3_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}

bool CheckInitState(const string& targetAction)
{
	if (!DebugUI::IsInitialized())
	{
		Log::Print(
			"Cannot " + targetAction + " because ImGui is not initialized!",
			"DEBUG_LOG",
			LogType::LOG_ERROR);

		return false;
	}

	return true;
}