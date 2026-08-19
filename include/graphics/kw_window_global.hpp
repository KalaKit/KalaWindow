//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.#pragma once

#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "core_utils.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::string_view;
	using std::vector;
	using std::filesystem::path;

	//Buttons shown on the popup
	enum class PopupAction
	{
		POPUP_ACTION_OK,            //OK button only
		POPUP_ACTION_OK_CANCEL,     //OK and Cancel buttons
		POPUP_ACTION_YES_NO,        //Yes and No buttons
		POPUP_ACTION_YES_NO_CANCEL, //Yes, No, and Cancel buttons
		POPUP_ACTION_RETRY_CANCEL   //Retry and Cancel buttons
	};

	//Icon shown on the popup
	enum class PopupType
	{
		POPUP_TYPE_INFO,    //Info icon (blue 'i')
		POPUP_TYPE_WARNING, //Warning icon (yellow triangle)
		POPUP_TYPE_ERROR,   //Error icon (red X)
		POPUP_TYPE_QUESTION //Question icon (used for confirmations)
	};

	//User response from the popup
	enum class PopupResult
	{
		POPUP_RESULT_NONE,   //No response or unknown
		POPUP_RESULT_OK,     //User clicked OK
		POPUP_RESULT_CANCEL, //User clicked Cancel
		POPUP_RESULT_YES,    //User clicked Yes
		POPUP_RESULT_NO,     //User clicked No
		POPUP_RESULT_RETRY   //User clicked Retry
	};

	enum class FileType
	{
		FILE_ANY,        //Can select any files
		FILE_FOLDER,     //Can select any folders
		FILE_EXE,        //Can select any executables
		FILE_CUSTOM      //Can select a custom list of files, must pass vector
	};

	enum class SoundType
	{
		SOUND_OK,
		SOUND_ERROR
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
		uintptr_t atom_net_wm_state_vertical{};
		uintptr_t atom_net_wm_state_horizontal{};
		uintptr_t atom_net_wm_state_above{};
		uintptr_t atom_net_wm_state_skip_taskbar{};

		uintptr_t atom_wm_delete{};
	};
#endif

	class LIB_API Window_Global
	{
	friend class ProcessWindow;
	public:
		static bool IsVerboseLoggingEnabled();
		//Toggle verbose logging. If true, then global window context 
		//and all windows will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState);

		static const string& GetAppName();
		//Assign the global app name that is used in 
		//Vulkan instance creation and for the crash handler
		static void SetAppName(string&& appName);

#if defined(KLIN_ANY)
		static const X11GlobalData& GetGlobalData();
#endif

#if defined(KWIN_ANY)
		//Returns full Windows version as xx.yyyyyy format,
		//where XX is windows version and YYYYYY is build version.
		//Six digits are reserved for build numbers, so builds are 0yyyyy mostly
		static u32 GetVersion();
		//Returns Windows build number as xxxx because the first digit is always 0 anyway
		static u32 GetBuildNumber();
		//Returns Windows build revision as xxxx
		static u32 GetBuildRevision();

		static string_view GetAppID();
#endif

		//Display any kind of a popup on screen for info that should be shown immediately..
		//Requires zenity on X11 and Wayland.
		static PopupResult CreatePopup(
			string&& title,
			string&& message,
			PopupAction action,
			PopupType type);

		//Uses the file explorer to get a path to selected files by chosen type.
		//RequiredRoot path enforces selected files to be within a specific directory,
		//all files and folders selected outside of it will be discarded
		//even if the file explorer allows to navigate outside that directory.
		//Set multiple to true to allow selecting more than one item,
		//does not work with directories on X11.
		//Requires zenity on X11, FILE_CUSTOM requires to fill out customTypes vector.
		static vector<path> GetFiles(
			FileType type,
			vector<string>&& customTypes = {},
			path&& requiredRoot = {},
			bool multiple = false);

		//Create a notification that shows up on the screen
		static void CreateNotification(
			string&& title,
			string&& message);

		//Play a system sound once of the chosen type
		static void PlaySystemSound(SoundType type);
	private:
		static void Initialize();
		static bool IsInitialized();
#if defined(KLIN_ANY)
		static void Shutdown();
#endif
	};
}