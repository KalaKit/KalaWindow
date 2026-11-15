//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.#pragma once

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "macros.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::string_view;
	using std::vector;
	using std::filesystem::path;

	//Buttons shown on the popup
	enum class PopupAction
	{
		//OK button only
		OK,
		//OK and Cancel buttons
		OK_CANCEL,
		//Yes and No buttons
		YES_NO,
		//Yes, No, and Cancel buttons
		YES_NO_CANCEL,
		//Retry and Cancel buttons
		RETRY_CANCEL
	};

	//Icon shown on the popup
	enum class PopupType
	{
		//Info icon (blue 'i')
		INFO,
		//Warning icon (yellow triangle)
		WARNING,
		//Error icon (red X)
		ERROR,
		//Question icon (used for confirmations)
		QUESTION
	};

	//User response from the popup
	enum class PopupResult
	{
		//No response or unknown (counts as bad input parameters or something did not work right)
		NONE,
		//User clicked OK
		OK,
		//User clicked Cancel
		CANCEL,
		//User clicked Yes
		YES,
		//User clicked No
		NO,
		//User clicked Retry
		RETRY
	};

	enum class FileType
	{
		//Can select any files
		ANY,
		//Can select any folders
		FOLDER,
		//Can select any executables
		EXE,
		//Can select a custom list of files, must pass vector
		CUSTOM
	};

	enum class SoundType
	{
		OK,
		ERROR
	};

#if defined(KLIN_ANY)
	struct X11GlobalData
	{
		uintptr_t display{};
		uintptr_t window_root{};

		uintptr_t xim{};

		int xiErrorBase{};
		int xiOpcode{};

		uintptr_t atom_utf8{};

		uintptr_t atom_xDndAware{};
		uintptr_t atom_xDndEnter{};
		uintptr_t atom_xDndPosition{};
		uintptr_t atom_xDndDrop{};
		uintptr_t atom_xDndStatus{};
		uintptr_t atom_xDndFinished{};
		uintptr_t atom_xDndActionCopy{};
		uintptr_t atom_xDndSelection{};
		uintptr_t atom_xDndTypeList{};
		uintptr_t atom_textUri{};

		uintptr_t atom_net_frame_extents{};

		uintptr_t atom_net_active_window{};
				
		uintptr_t atom_net_wm_window_type{};
		uintptr_t atom_net_wm_window_type_normal{};

		uintptr_t atom_net_wm_name{};
		uintptr_t atom_net_wm_pid{};

		uintptr_t atom_net_wm_allowed_actions{};
		uintptr_t atom_net_wm_action_resize{};

		uintptr_t atom_net_wm_state{};
		uintptr_t atom_net_wm_state_hidden{};
		uintptr_t atom_net_wm_state_fullscreen{};
		uintptr_t atom_net_wm_state_maximized_horizontal{};
		uintptr_t atom_net_wm_state_maximized_vertical{};
		uintptr_t atom_net_wm_state_above{};
		uintptr_t atom_net_wm_state_skip_taskbar{};

		uintptr_t atom_wm_delete{};
	};
#endif

	class LIB_API Window_Global
	{
	friend class ProcessWindow;
	public:
		KNODISCARD
		static bool IsVerboseLoggingEnabled();
		//Toggle verbose logging. If true, then global window context 
		//and all windows will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState);

		static const string& GetAppName();
		//Assign the global app name that is used in 
		//Vulkan instance creation and for the crash handler
		static void SetAppName(string_view appName);

#if defined(KLIN_ANY)
		KNODISCARD
		static const X11GlobalData& GetGlobalData();
#endif

#if defined(KWIN_ANY)
		//Returns full Windows version as xx.yyyyyy format,
		//where XX is windows version and YYYYYY is build version.
		//Six digits are reserved for build numbers, so builds are 0yyyyy mostly
		KNODISCARD
		static u32 GetVersion();
		//Returns Windows build number as xxxx because the first digit is always 0 anyway
		KNODISCARD
		static u32 GetBuildNumber();
		//Returns Windows build revision as xxxx
		KNODISCARD
		static u32 GetBuildRevision();

		KNODISCARD
		static string_view GetAppID();
#endif

		//Display any kind of a popup on screen for info that should be shown immediately..
		//Requires zenity on X11 and Wayland.
		KNODISCARD
		static PopupResult CreatePopup(
			string_view title,
			string_view message,
			PopupAction action,
			PopupType type);

		//Uses the file explorer to get a path to selected files by chosen type.
		//RequiredRoot path enforces selected files to be within a specific directory,
		//all files and folders selected outside of it will be discarded
		//even if the file explorer allows to navigate outside that directory.
		//Set multiple to true to allow selecting more than one item,
		//does not work with directories on X11.
		//Requires zenity on X11, CUSTOM requires to fill out customTypes vector.
		KNODISCARD
		static vector<path> GetFiles(
			FileType type,
			vector<string>&& customTypes = {},
			path&& requiredRoot = {},
			bool multiple = false);

		//Create a notification that shows up on the screen
		static void CreateNotification(
			string_view title,
			string_view message);

		//Play a system sound once of the chosen type
		static void PlaySystemSound(SoundType type);
	private:
		static void Initialize();
		KNODISCARD
		static bool IsInitialized();
#if defined(KLIN_ANY)
		static void Shutdown();
#endif
	};
}