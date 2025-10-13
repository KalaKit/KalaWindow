//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <memory>

#include "KalaHeaders/log_utils.hpp"

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

namespace KalaWindow::UI
{
	Font* Font::LoadFont(
		const string& fontPath,
		const string& name)
	{
		u32 newID = ++globalID;
		unique_ptr<Font> newFont = make_unique<Font>();
		Font* fontPtr = newFont.get();

		Log::Print(
			"Loading font '" + name + "' with ID '" + to_string(newID) + "'.",
			"FONT",
			LogType::LOG_DEBUG);

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