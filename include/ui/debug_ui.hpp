//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <array>

#include "imgui/imgui.h"

#include "KalaHeaders/core_utils.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::Graphics
{
	class Window;
}

namespace KalaWindow::UI
{
	using std::string;
	using std::to_string;
	using std::function;
	using std::unordered_map;
	using std::vector;
	using std::array;

	using KalaWindow::Graphics::Window;

	struct UserFont
	{
		string fontName{};     //name of this font
		ImFont* fontPointer{}; //imgui font pointer to this font
		string fontPath{};     //path to this font
		float fontSize{};      //size of this font
	};

	struct WindowSettings
	{
		bool isMovable     = true;  //if true, then this window is movable
		bool isResizable   = true;  //if true, then this window is resizable
		bool isCollapsible = true;  //if true, then this window is collapsible
		bool hasToolbar    = true;  //if true, then this window has a toolbar
		bool saveSettings  = true;  //if true, then this window settings will be saved to imgui.ini
		bool topMost       = false; //if true, then this window will stay at the front no matter what
		bool noFocus       = false; //if true, then this window wont go to front even when clicked
	};

	class LIB_API DebugUI
	{
	public:
		//Set up ImGui. Also set docking state with enableDocking.
		//Pass paths and font size per font relative to your executable to load as ImGui fonts
		static DebugUI* Initialize(
			Window* window,
			bool enableDocking = true,
			const vector<UserFont>& userProvidedFonts = {});

		inline const u32 GetID() const { return ID; }

		inline bool IsInitialized() const { return isInitialized; }
		inline bool IsDockingEnabled() const { return isDockingEnabled; }

		//Returns selected user font by name
		inline const UserFont& GetUserFont(const string& name) const
		{
			static const UserFont empty{};

			auto it = userFonts.find(name);
			if (it != userFonts.end()) return it->second;

			return empty;
		}
		//Returns all user fonts
		inline const unordered_map<string, UserFont>& GetAllUserFonts() const
		{
			return userFonts;
		}

		//Assign the top bar function that should hold all your top bar draw functions,
		//does not need to be ran every frame.
		inline void SetTopBarFunction(const function<void()>& newFunction)
		{
			topBarFunction = newFunction;
		}
		inline const function<void()>& GetTopBarFunction()
		{
			return topBarFunction;
		}

		//Assign the main function that should hold all your main draw functions,
		//does not need to be ran every frame.
		inline void SetMainRenderFunction(const function<void()>& newFunction)
		{
			mainRenderFunction = newFunction;
		}
		inline const function<void()>& GetMainRenderFunction()
		{
			return mainRenderFunction;
		}

		//Place ImGui window to the center
		vec2 CenterWindow(
			Window* window,
			vec2 size) const;

		//Renders a regular freeform window that can be 
		//rendered for the whole executable lifetime.
		//Modify the WindowSettings struct bool states to control limitations of this window.
		//Assign a function to control what content is rendered inside this window.
		//Leaving position at 0 moves this ImGui window to the center.
		//Leaving min and max size at 0 adds no size constraints to this ImGui window.
		void RenderWindow(
			u32 ID,
			WindowSettings settings,
			function<void()> func,
			const string& title,
			vec2 size,
			vec2 pos = vec2(0),
			vec2 minSize = vec2(0),
			vec2 maxSize = vec2(0));

		//Renders a non-movable and non-resizable modal ImGui at the center of the
		//main window that should only appear when conditions are met.
		//Window ID needs to be a valid executable window ID that you created.
		//Assign a function to control what content is rendered inside this window.
		//Leaving size at 0 makes the size default to 300x200.
		//This ImGui window data will not be stored by ImGui.
		void RenderModalWindow(
			Window* window,
			u32 ID,
			function<void()> func,
			const string& title,
			vec2 size = vec2(0)) const;

		//Render a dynamic size text field
		// - ID: unique ID for this text element
		// - width: how wide this text field is
		// - buffer: char pointer of your target text where user input will be stored inside of
		// - size: max allowed characters + null terminator
		// - digitsOnly: if true, then only digits can be inserted
		void RenderTextField(
			u32 ID,
			u16 width,
			char* buffer,
			size_t size,
			bool digitsOnly = false)
		{
			string label = "##" + to_string(ID);

			ImGui::PushItemWidth(width);

			if (!digitsOnly)
			{
				ImGui::InputText(
					label.c_str(),
					buffer,
					size);
			}
			else
			{
				ImGui::InputText(
					label.c_str(),
					buffer,
					size,
					ImGuiInputTextFlags_CallbackCharFilter,
					[](ImGuiInputTextCallbackData* data) -> int
					{
						return 
							!(data->EventChar >= '0'
							&& data->EventChar <= '9');
					});
			}

			ImGui::PopItemWidth();
		}
		//Render a fixed size text field
		// - ID: unique ID for this text element
		// - width: how wide this text field is
		// - buffer: char array of your target text where user input will be stored inside of
		// - digitsOnly: if true, then only digits can be inserted
		template <size_t N>
		void RenderTextField(
			u32 ID,
			u16 width,
			array<char, N>& buffer,
			bool digitsOnly = false)
		{
			string label = "##" + to_string(ID);

			ImGui::PushItemWidth(width);

			if (!digitsOnly)
			{
				ImGui::InputText(
					label.c_str(),
					buffer.data(),
					buffer.size());
			}
			else
			{
				ImGui::InputText(
					label.c_str(),
					buffer.data(),
					buffer.size(),
					ImGuiInputTextFlags_CallbackCharFilter,
					[](ImGuiInputTextCallbackData* data) -> int
					{
						return
							!(data->EventChar >= '0'
							&& data->EventChar <= '9');
					});
			}

			ImGui::PopItemWidth();
		}

		//ImGui main draw loop
		void Render(Window* window);

		//Do not destroy manually, erase from containers.hpp instead
		~DebugUI();
	private:
		bool isInitialized{};
		bool isDockingEnabled{};

		ImGuiContext* context{};

		u32 ID{};

		function<void()> topBarFunction{};
		function<void()> mainRenderFunction{};

		unordered_map<string, UserFont> userFonts{};
	};
}