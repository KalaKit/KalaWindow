//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>

#include "KalaHeaders/api.hpp"
#include "KalaHeaders/core_types.hpp"

#include "core/glm_global.hpp"

namespace KalaWindow::UI
{
	using std::string;
	using std::function;

	enum class WindowMenuButtonPosition
	{
		Dir_None = -1, //No button
		Dir_Left = 0,  //On the left of the title bar
		Dir_Right = 1  //On the right of the title bar
	};

	enum class ColorButtonPosition
	{
		Dir_Left = 0, //Color square on the left side
		Dir_Right = 1 //Color square on the right side
	};

	class LIB_API ImGuiCore
	{
	public:
		//Initialize ImGui with docking.
		static bool Initialize(
			const string& configPath,
			const string& tempPath);

		//Returns true if ImGui was successfully initialized
		static bool IsInitialized() { return isInitialized; };

		//Place window to framebuffer center
		static vec2 CenterWindow(vec2 size);

		static void SetFloat(
			const string& name,
			f32 value)
		{
			if      (name == "alpha")                               alpha = value;
			else if (name == "childBorderSize")                     childBorderSize = value;
			else if (name == "childRounding")                       childRounding = value;
			else if (name == "columnsMinSpacing")                   columnsMinSpacing = value;
			else if (name == "disabledAlpha")                       disabledAlpha = value;
			else if (name == "frameBorderSize")                     frameBorderSize = value;
			else if (name == "frameRounding")                       frameRounding = value;
			else if (name == "grabMinSize")                         grabMinSize = value;
			else if (name == "grabRounding")                        grabRounding = value;
			else if (name == "indentSpacing")                       indentSpacing = value;
			else if (name == "popupBorderSize")                     popupBorderSize = value;
			else if (name == "popupRounding")                       popupRounding = value;
			else if (name == "scrollbarRounding")                   scrollbarRounding = value;
			else if (name == "scrollbarSize")                       scrollbarSize = value;
			else if (name == "tabBorderSize")                       tabBorderSize = value;
			else if (name == "tabCloseButtonMinWidthSelected")      tabCloseButtonMinWidthSelected = value;
			else if (name == "tabCloseButtonMinWidthUnselected")    tabCloseButtonMinWidthUnselected = value;
			else if (name == "tabRounding")                         tabRounding = value;
			else if (name == "windowBorderSize")                    windowBorderSize = value;
			else if (name == "windowRounding")                      windowRounding = value;
		}

		static void SetVec2(
			const string& name,
			vec2 value)
		{
			if      (name == "buttonTextAlign")     buttonTextAlign = value;
			else if (name == "cellPadding")         cellPadding = value;
			else if (name == "framePadding")        framePadding = value;
			else if (name == "itemInnerSpacing")    itemInnerSpacing = value;
			else if (name == "itemSpacing")         itemSpacing = value;
			else if (name == "selectableTextAlign") selectableTextAlign = value;
			else if (name == "windowMinSize")       windowMinSize = value;
			else if (name == "windowPadding")       windowPadding = value;
			else if (name == "windowTitleAlign")    windowTitleAlign = value;
		}

		inline void SetVec4(
			const string& name,
			const vec4& value)
		{
			if      (name == "color_Border")                color_Border = value;
			else if (name == "color_BorderShadow")          color_BorderShadow = value;
			else if (name == "color_Button")                color_Button = value;
			else if (name == "color_ButtonActive")          color_ButtonActive = value;
			else if (name == "color_ButtonHovered")         color_ButtonHovered = value;
			else if (name == "color_CheckMark")             color_CheckMark = value;
			else if (name == "color_ChildBg")               color_ChildBg = value;
			else if (name == "color_DragDropTarget")        color_DragDropTarget = value;
			else if (name == "color_FrameBg")               color_FrameBg = value;
			else if (name == "color_FrameBgActive")         color_FrameBgActive = value;
			else if (name == "color_FrameBgHovered")        color_FrameBgHovered = value;
			else if (name == "color_Header")                color_Header = value;
			else if (name == "color_HeaderActive")          color_HeaderActive = value;
			else if (name == "color_HeaderHovered")         color_HeaderHovered = value;
			else if (name == "color_MenuBarBg")             color_MenuBarBg = value;
			else if (name == "color_ModalWindowDimBg")      color_ModalWindowDimBg = value;
			else if (name == "color_NavHighlight")          color_NavHighlight = value;
			else if (name == "color_NavWindowingDimBg")     color_NavWindowingDimBg = value;
			else if (name == "color_NavWindowingHighlight") color_NavWindowingHighlight = value;
			else if (name == "color_PlotHistogram")         color_PlotHistogram = value;
			else if (name == "color_PlotHistogramHovered")  color_PlotHistogramHovered = value;
			else if (name == "color_PlotLines")             color_PlotLines = value;
			else if (name == "color_PlotLinesHovered")      color_PlotLinesHovered = value;
			else if (name == "color_PopupBg")               color_PopupBg = value;
			else if (name == "color_ResizeGrip")            color_ResizeGrip = value;
			else if (name == "color_ResizeGripActive")      color_ResizeGripActive = value;
			else if (name == "color_ResizeGripHovered")     color_ResizeGripHovered = value;
			else if (name == "color_ScrollbarBg")           color_ScrollbarBg = value;
			else if (name == "color_ScrollbarGrab")         color_ScrollbarGrab = value;
			else if (name == "color_ScrollbarGrabActive")   color_ScrollbarGrabActive = value;
			else if (name == "color_ScrollbarGrabHovered")  color_ScrollbarGrabHovered = value;
			else if (name == "color_Separator")             color_Separator = value;
			else if (name == "color_SeparatorActive")       color_SeparatorActive = value;
			else if (name == "color_SeparatorHovered")      color_SeparatorHovered = value;
			else if (name == "color_SliderGrab")            color_SliderGrab = value;
			else if (name == "color_SliderGrabActive")      color_SliderGrabActive = value;
			else if (name == "color_Tab")                   color_Tab = value;
			else if (name == "color_TabActive")             color_TabActive = value;
			else if (name == "color_TabHovered")            color_TabHovered = value;
			else if (name == "color_TabUnfocused")          color_TabUnfocused = value;
			else if (name == "color_TabUnfocusedActive")    color_TabUnfocusedActive = value;
			else if (name == "color_TableBorderLight")      color_TableBorderLight = value;
			else if (name == "color_TableBorderStrong")     color_TableBorderStrong = value;
			else if (name == "color_TableHeaderBg")         color_TableHeaderBg = value;
			else if (name == "color_TableRowBg")            color_TableRowBg = value;
			else if (name == "color_TableRowBgAlt")         color_TableRowBgAlt = value;
			else if (name == "color_Text")                  color_Text = value;
			else if (name == "color_TextDisabled")          color_TextDisabled = value;
			else if (name == "color_TextSelectedBg")        color_TextSelectedBg = value;
			else if (name == "color_TitleBg")               color_TitleBg = value;
			else if (name == "color_TitleBgActive")         color_TitleBgActive = value;
			else if (name == "color_TitleBgCollapsed")      color_TitleBgCollapsed = value;
			else if (name == "color_WindowBg")              color_WindowBg = value;
		}

		//Draws all content for the current frame, handles frame start and end.
		//Optionally handles top bar as well. Simply pass functions for your content here.
		static void Render(
			const function<void()>& userRenderContent,
			const function<void()>& userTopBar = NULL);

		//Shut down ImGui
		static void Shutdown();
	private:
		static inline bool isInitialized;

		static inline f32 alpha                             = 1.0f;
		static inline f32 childBorderSize                   = 1.0f;
		static inline f32 childRounding                     = 4.0f;
		static inline f32 columnsMinSpacing                 = 6.0f;
		static inline f32 disabledAlpha                     = 0.6f;
		static inline f32 frameBorderSize                   = 1.0f;
		static inline f32 frameRounding                     = 4.0f;
		static inline f32 grabMinSize                       = 10.0f;
		static inline f32 grabRounding                      = 20.0f;
		static inline f32 indentSpacing                     = 21.0f;
		static inline f32 popupBorderSize                   = 1.0f;
		static inline f32 popupRounding                     = 4.0f;
		static inline f32 scrollbarRounding                 = 4.0f;
		static inline f32 scrollbarSize                     = 14.0f;
		static inline f32 tabBorderSize                     = 1.0f;
		static inline f32 tabCloseButtonMinWidthSelected    = 0.0f;
		static inline f32 tabCloseButtonMinWidthUnselected  = 0.0f;
		static inline f32 tabRounding                       = 4.0f;
		static inline f32 windowBorderSize                  = 1.0f;
		static inline f32 windowRounding                    = 4.0f;

		static inline vec2 buttonTextAlign      = vec2(0.5f, 0.5f);
		static inline vec2 cellPadding          = vec2(4.0f, 2.0f);
		static inline vec2 framePadding         = vec2(4.0f, 3.0f);
		static inline vec2 itemInnerSpacing     = vec2(4.0f, 4.0f);
		static inline vec2 itemSpacing          = vec2(8.0f, 4.0f);
		static inline vec2 selectableTextAlign  = vec2(0.0f, 0.0f);
		static inline vec2 windowMinSize        = vec2(32.0f, 32.0f);
		static inline vec2 windowPadding        = vec2(8.0f, 8.0f);
		static inline vec2 windowTitleAlign     = vec2(0.0f, 0.5f);

		static inline WindowMenuButtonPosition windowMenuButtonPosition = WindowMenuButtonPosition::Dir_None;
		static inline ColorButtonPosition      colorButtonPosition      = ColorButtonPosition::Dir_Right;

		static inline vec4 color_Border                = vec4(1.00f, 1.00f, 1.00f, 0.16f);
		static inline vec4 color_BorderShadow          = vec4(0.00f, 0.00f, 0.00f, 0.00f);
		static inline vec4 color_Button                = vec4(0.15f, 0.15f, 0.15f, 1.00f);
		static inline vec4 color_ButtonActive          = vec4(0.33f, 0.33f, 0.33f, 1.00f);
		static inline vec4 color_ButtonHovered         = vec4(0.25f, 0.25f, 0.25f, 1.00f);
		static inline vec4 color_CheckMark             = vec4(1.00f, 1.00f, 1.00f, 1.00f);
		static inline vec4 color_ChildBg               = vec4(0.00f, 0.00f, 0.00f, 0.00f);
		static inline vec4 color_DragDropTarget        = vec4(1.00f, 1.00f, 0.00f, 0.90f);
		static inline vec4 color_FrameBg               = vec4(0.09f, 0.09f, 0.09f, 1.00f);
		static inline vec4 color_FrameBgActive         = vec4(0.19f, 0.19f, 0.19f, 1.00f);
		static inline vec4 color_FrameBgHovered        = vec4(0.15f, 0.15f, 0.15f, 1.00f);
		static inline vec4 color_Header                = vec4(0.98f, 0.98f, 0.98f, 0.31f);
		static inline vec4 color_HeaderActive          = vec4(0.98f, 0.98f, 0.98f, 1.00f);
		static inline vec4 color_HeaderHovered         = vec4(0.98f, 0.98f, 0.98f, 0.80f);
		static inline vec4 color_MenuBarBg             = vec4(0.11f, 0.11f, 0.11f, 1.00f);
		static inline vec4 color_ModalWindowDimBg      = vec4(0.80f, 0.80f, 0.80f, 0.35f);
		static inline vec4 color_NavHighlight          = vec4(0.25f, 0.59f, 0.98f, 1.00f);
		static inline vec4 color_NavWindowingDimBg     = vec4(0.80f, 0.80f, 0.80f, 0.20f);
		static inline vec4 color_NavWindowingHighlight = vec4(1.00f, 1.00f, 1.00f, 0.70f);
		static inline vec4 color_PlotHistogram         = vec4(0.90f, 0.70f, 0.00f, 1.00f);
		static inline vec4 color_PlotHistogramHovered  = vec4(1.00f, 0.60f, 0.00f, 1.00f);
		static inline vec4 color_PlotLines             = vec4(0.61f, 0.61f, 0.61f, 1.00f);
		static inline vec4 color_PlotLinesHovered      = vec4(1.00f, 0.43f, 0.35f, 1.00f);
		static inline vec4 color_PopupBg               = vec4(0.08f, 0.08f, 0.08f, 0.94f);
		static inline vec4 color_ResizeGrip            = vec4(0.98f, 0.98f, 0.98f, 0.20f);
		static inline vec4 color_ResizeGripActive      = vec4(0.98f, 0.98f, 0.98f, 0.95f);
		static inline vec4 color_ResizeGripHovered     = vec4(0.94f, 0.94f, 0.94f, 0.67f);
		static inline vec4 color_ScrollbarBg           = vec4(0.02f, 0.02f, 0.02f, 0.53f);
		static inline vec4 color_ScrollbarGrab         = vec4(0.31f, 0.31f, 0.31f, 1.00f);
		static inline vec4 color_ScrollbarGrabActive   = vec4(0.51f, 0.51f, 0.51f, 1.00f);
		static inline vec4 color_ScrollbarGrabHovered  = vec4(0.41f, 0.41f, 0.41f, 1.00f);
		static inline vec4 color_Separator             = vec4(0.43f, 0.43f, 0.50f, 0.50f);
		static inline vec4 color_SeparatorActive       = vec4(0.75f, 0.75f, 0.75f, 1.00f);
		static inline vec4 color_SeparatorHovered      = vec4(0.75f, 0.75f, 0.75f, 0.78f);
		static inline vec4 color_SliderGrab            = vec4(0.88f, 0.88f, 0.88f, 1.00f);
		static inline vec4 color_SliderGrabActive      = vec4(0.98f, 0.98f, 0.98f, 1.00f);
		static inline vec4 color_Tab                   = vec4(0.22f, 0.22f, 0.22f, 0.86f);
		static inline vec4 color_TabActive             = vec4(0.27f, 0.27f, 0.27f, 1.00f);
		static inline vec4 color_TabHovered            = vec4(0.32f, 0.32f, 0.32f, 0.80f);
		static inline vec4 color_TabUnfocused          = vec4(0.15f, 0.15f, 0.15f, 0.97f);
		static inline vec4 color_TabUnfocusedActive    = vec4(0.42f, 0.42f, 0.42f, 1.00f);
		static inline vec4 color_TableBorderLight      = vec4(0.23f, 0.23f, 0.25f, 1.00f);
		static inline vec4 color_TableBorderStrong     = vec4(0.31f, 0.31f, 0.35f, 1.00f);
		static inline vec4 color_TableHeaderBg         = vec4(0.19f, 0.19f, 0.20f, 1.00f);
		static inline vec4 color_TableRowBg            = vec4(0.00f, 0.00f, 0.00f, 0.00f);
		static inline vec4 color_TableRowBgAlt         = vec4(1.00f, 1.00f, 1.00f, 0.06f);
		static inline vec4 color_Text                  = vec4(1.00f, 1.00f, 1.00f, 1.00f);
		static inline vec4 color_TextDisabled          = vec4(0.50f, 0.50f, 0.50f, 1.00f);
		static inline vec4 color_TextSelectedBg        = vec4(0.26f, 0.59f, 0.98f, 0.35f);
		static inline vec4 color_TitleBg               = vec4(0.11f, 0.11f, 0.11f, 1.00f);
		static inline vec4 color_TitleBgActive         = vec4(0.11f, 0.11f, 0.11f, 1.00f);
		static inline vec4 color_TitleBgCollapsed      = vec4(0.00f, 0.00f, 0.00f, 0.51f);
		static inline vec4 color_WindowBg              = vec4(0.11f, 0.11f, 0.11f, 1.00f);
	};
}