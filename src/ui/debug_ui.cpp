//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_win32.h"

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

namespace KalaWindow::UI
{
	bool DebugUI::Initialize()
	{
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize ImGui more than once!",
				"DEBUG_UI",
				LogType::LOG_ERROR);

			return false;
		}

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
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

	vec2 DebugUI::CenterWindow(
		vec2 size,
		u32 windowID)
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot center window because ImGui is not initialized!",
				"DEBUG_UI",
				LogType::LOG_ERROR);

			return vec2();
		}

		Window* win = createdWindows[windowID].get();

		if (win == nullptr)
		{
			Log::Print(
				"Cannot center ImGui window because the window ID is invalid!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return vec2();
		}

		vec2 rectSize = win->GetClientRectSize();

		float posX = (rectSize.x - size.x) / 2.0f;
		float posY = (rectSize.y - size.y) / 2.0f;

		return vec2(posX, posY);
	}

	void DebugUI::Render(u32 windowID)
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot run ImGui because ImGui is not initialized!",
				"DEBUG_UI",
				LogType::LOG_ERROR);

			return;
		}

		Window* win = createdWindows[windowID].get();

		if (win == nullptr)
		{
			Log::Print(
				"Cannot render ImGui because the window ID is invalid!",
				"WINDOW_WINDOWS",
				LogType::LOG_ERROR);

			return;
		}

		vec2 rectSize = win->GetClientRectSize();
		vec2 fbSize = win->GetFramebufferSize();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(fbSize.x, fbSize.y);

		if (fbSize == rectSize)
		{
			io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		}
		else
		{
			io.DisplayFramebufferScale = ImVec2(
				rectSize.x > 0 ? (float)fbSize.x / (float)rectSize.x : 1.0f,
				rectSize.y > 0 ? (float)fbSize.y / (float)rectSize.y : 1.0f);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (topBarFunction != NULL) topBarFunction();

		ImGuiDockNodeFlags flags =
			ImGuiDockNodeFlags_PassthruCentralNode;

		ImGui::DockSpaceOverViewport(
			0,
			ImGui::GetMainViewport(),
			flags,
			nullptr);

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
				LogType::LOG_ERROR);

			return;
		}

		isInitialized = false;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}