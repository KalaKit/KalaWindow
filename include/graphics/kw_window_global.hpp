//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.#pragma once

#pragma once

#include <string>
#include <vector>

#include "KalaHeaders/core_utils.hpp"
#include "core/kw_core.hpp"
#include "graphics/kw_window.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::string_view;
	using std::vector;

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
		FILE_TEXT,       //Can select .txt, .ini, .rtf and .md files
		FILE_STRUCTURED, //Can select .json, .xml, .yaml, .yml and .toml files
		FILE_SCRIPT,     //Can select .lua, .cpp, .hpp, .c and .h files
		FILE_ARCHIVE,    //Can select .zip, .7z, .rar and .kdat files
		FILE_VIDEO,      //Can select .mp4, .mov and .mkv files
		FILE_AUDIO,      //Can select .wav, .flac, .mp3 and .ogg files
		FILE_MODEL,      //Can select .fbx, .obj and .gltf files
		FILE_SHADER,     //Can select .vert, .frag and .geom files
		FILE_TEXTURE     //Can select .png, .jpg and .jpeg files
	};

	enum class SoundType
	{
		SOUND_OK,
		SOUND_ERROR
	};

#if defined(__linux__) && defined(KW_USE_X11)
	struct X11GlobalData
	{
		uintptr_t display{};
		uintptr_t window_root{};
		uintptr_t atom_wmDelete{};
	};
#endif

	class LIB_API Window_Global
	{
	friend ProcessWindow;
	friend KalaWindow::Core::KalaWindowCore;
	public:
		//Toggle verbose logging. If true, then global window context 
		//and all windows will dump their logs into the console.
		static void SetVerboseLoggingState(bool newState);
		static bool IsVerboseLoggingEnabled();

		static bool Initialize();
		static bool IsInitialized();

#ifdef _WIN32
		//Returns Windows version as xxyyyyyy format,
		//where XX is windows version and YYYYYY is build version.
		//Six digits are reserved for build numbers, so builds are 0yyyyy mostly
		static u32 GetVersion();

		static const string& GetAppID();
#endif

		//Display any kind of a popup on screen for info that should be shown immediately..
		//Requires zenity on X11 and Wayland.
		static PopupResult CreatePopup(
			string_view title,
			string_view message,
			PopupAction action,
			PopupType type);

		//Uses the file explorer to get a path to selected files by chosen type.
		//Set multiple to true to allow returning more than one item.
		//Requires zenity on X11 and Wayland.
		static vector<string> GetFile(
			FileType type,
			bool multiple = false);

		//Create a notification that shows up on the screen
		static void CreateNotification(
			string_view title,
			string_view message);

		//Play a system sound once of the chosen type.
		//Requires libcanberra on X11 and Wayland
		static void PlaySystemSound(SoundType type);

		//Places selected string to clipboard
		static void SetClipboardText(string_view text);
		//Returns string from clipboard
		static string GetClipboardText();
	private:
#if defined(__linux__) && defined(KW_USE_X11)
		static const X11GlobalData& GetGlobalData();
#endif
	};
}