//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/file_utils.hpp"

#include "ui/font.hpp"
#include "core/core.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::globalID;
using KalaFont::ImportKalaFont;

using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::filesystem::path;
using std::filesystem::exists;
using std::filesystem::is_regular_file;
using std::move;

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
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}
		
		path correctFontPath = path(fontPath);
		
		if (!is_regular_file(correctFontPath))
		{
			Log::Print(
				"Cannot load font '" + name + "' because its path '" + fontPath + "' does not have an extension!",
				"FONT",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		if (correctFontPath.extension() != ".kfont")
		{
			Log::Print(
				"Cannot load font '" + name + "' because its path '" + fontPath + "' does not have the supported extension '.kfont'!",
				"FONT",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		vector<GlyphResult> result{};
		if (!ImportKalaFont(correctFontPath, result))
		{
			Log::Print(
				"Failed to load font '" + name + "'!",
				"FONT",
				LogType::LOG_ERROR,
				2);
				
			return nullptr;
		}
		
		fontPtr->result = move(result);

		fontPtr->ID = newID;
		fontPtr->SetName(name);
		fontPtr->fontPath = fontPath;

		registry.AddContent(newID, move(newFont));

		Log::Print(
			"Loaded font '" + name + "' with ID '" + to_string(newID) + "'!",
			"FONT",
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