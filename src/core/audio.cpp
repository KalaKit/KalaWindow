//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "core/audio.hpp"
#include "core/log.hpp"
#include "core/containers.hpp"

using KalaWindow::Core::Audio;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;

using std::unordered_map;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::vector;

static vector<string> supportedExtensions =
{
	".wav",
	".flac",
	".mp3",
	".ogg"
};

static ma_engine engine{};
unordered_map<string, unique_ptr<ma_sound>> soundMap{};

static void CheckInitState(const string& targetAction);
static void PrintErrorMessage(const string& message);

namespace KalaWindow::Core
{
	//
	// AUDIO CORE
	//

	void Audio::Initialize()
	{
		if (isInitialized) return;

		ma_result result = ma_engine_init(NULL, &engine);
		if (result != MA_SUCCESS)
		{
			PrintErrorMessage("Failed to initialize MiniAudio!");
		}

		Logger::Print(
			"Initialized MiniAudio!",
			"MINIAUDIO",
			LogType::LOG_SUCCESS);

		isInitialized = true;
	}

	//
	// EACH INDIVIDUAL IMPORTED AUDIO FILE
	//

	AudioTrack* AudioTrack::ImportAudioTrack(
		const string& name,
		const string& filePath)
	{
		CheckInitState("import audio file '" + name + "'");

		if (soundMap.find(filePath) != soundMap.end())
		{
			PrintErrorMessage("Cannot import audio track '" + name + "' because it has already been imported!");

			return nullptr;
		}

		if (!exists(filePath))
		{
			PrintErrorMessage(
				"Cannot import audio track '" + name 
				+ "' because its file path '" + filePath + "' does not exist!");

			return nullptr;
		}

		if (!path(filePath).has_extension()
			|| !is_regular_file(filePath))
		{
			PrintErrorMessage(
				"Cannot import audio track '" + name 
				+ "' because its file path '" + filePath + "' type is unsupported!");

			return nullptr;
		}

		bool hasSupportedExtension = false;
		string extension = path(filePath).extension().string();
		for (const auto& ext : supportedExtensions)
		{
			if (extension == ext)
			{
				hasSupportedExtension = true;
				break;
			}
		}

		if (!hasSupportedExtension)
		{
			PrintErrorMessage(
				"Cannot import audio track '" + name + "' because its extenion '" + extension + "' is unsupported!"
				"You must use .wav, .flac, .mp3 or .ogg only.");

			return nullptr;
		}

		unique_ptr<ma_sound> sound = make_unique<ma_sound>();
		ma_result result = ma_sound_init_from_file(
			&engine,
			filePath.c_str(),
			0,
			NULL,
			NULL,
			sound.get());

		if (result != MA_SUCCESS)
		{
			PrintErrorMessage("Failed to import audio file '" + name + "'");

			return nullptr;
		}

		soundMap[filePath] = move(sound);

		u32 newID = globalID++;

		unique_ptr<AudioTrack> newTrack = make_unique<AudioTrack>();

		createdAudioTracks[newID] = move(newTrack);

		Logger::Print(
			"Imported audio file '" + name + "' with ID '" + to_string(newID) + "'!",
			"MINIAUDIO",
			LogType::LOG_SUCCESS);

		return createdAudioTracks[newID].get();
	}
}

void CheckInitState(const string& targetAction)
{
	if (!Audio::IsInitialized())
	{
		PrintErrorMessage("Cannot " + targetAction + " because MiniAudio is not initialized!");
	}
}
void PrintErrorMessage(const string& message)
{
	Logger::Print(
		message,
		"MINIAUDIO",
		LogType::LOG_ERROR,
		2);
}