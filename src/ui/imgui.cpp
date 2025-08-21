//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "OpenGL/glcorearb.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "ui/imgui.hpp"

namespace KalaWindow::UI
{
	void ImGuiCore::Initialize(
		const string& configPath,
		const string& tempPath)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGui::GetCurrentContext());
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		//ImGui_ImplGlfw_InitForOpenGL(Render::window, true);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplOpenGL3_Init("#version 330");

		isInitialized = true;
		return true;
	}

	vec2 ImGuiCore::CenterWindow(vec2 size)
	{
		//TODO: CREATE FRAMEBUFFER GETTER
		//TODO: DEFINE PFNGLGETINTEGERVPROC
	}

	void ImGuiCore::Render(
		const function<void()>& userRenderContent,
		const function<void()>& userTopBar = NULL)
	{

	}

	void ImGuiCore::Shutdown()
	{

	}
}