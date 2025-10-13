//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/file_utils.hpp"

#include "ui/font.hpp"
#include "core/containers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaWindow::Core::createdFonts;
using KalaWindow::Core::runtimeFonts;

using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;

namespace KalaWindow::UI
{
	Font* Font::LoadFont(
		const string& name,
		const string& fontPath)
	{
		u32 newID = ++globalID;
		unique_ptr<Font> newFont = make_unique<Font>();
		Font* fontPtr = newFont.get();

		Log::Print(
			"Loading font '" + name + "' with ID '" + to_string(newID) + "'.",
			"FONT",
			LogType::LOG_DEBUG);

		if (!exists(fontPath))
		{
			Log::Print(
				"Cannot load font '" + name + "' because its path '" + fontPath + "' does not exist!",
				"FONT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		if (!path(fontPath).has_extension())
		{
			Log::Print(
				"Cannot load font '" + name + "' because its path '" + fontPath + "' does not have an extension!",
				"FONT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		string extension = path(fontPath).extension().string();

		if (extension != ".otf"
			&& extension != ".ttf")
		{
			Log::Print(
				"Cannot load font '" + name + "' because it path '" + fontPath + "' does not have the supported extension '.otf' or '.ttf'!",
				"FONT",
				LogType::LOG_ERROR);

			return nullptr;
		}

		//<<< load font here

		fontPtr->ID = newID;
		fontPtr->SetName(name);
		fontPtr->fontPath = fontPath;

		createdFonts[newID] = move(newFont);
		runtimeFonts.push_back(fontPtr);

		Log::Print(
			"Loaded font '" + name + "' with ID '" + to_string(newID) + "'!",
			"WINDOW",
			LogType::LOG_SUCCESS);

		return fontPtr;
	}

	Font::~Font()
	{
		Log::Print(
			"Destroying font '" + name + "' with ID '" + to_string(ID) + "' .",
			"FONT",
			LogType::LOG_INFO);
	}
}