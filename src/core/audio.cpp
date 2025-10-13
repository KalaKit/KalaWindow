//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <sstream>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "KalaHeaders/log_utils.hpp"

#include "core/containers.hpp"
#include "core/audio.hpp"
#include "core/core.hpp"
#include "graphics/window.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::Audio;
using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::windowContent;
using KalaWindow::Core::WindowContent;
using KalaWindow::Core::GetValueByID;
using KalaWindow::Graphics::Window;

using std::unordered_map;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::vector;
using std::clamp;
using std::filesystem::file_size;
using std::ostringstream;

struct PlayerData
{
	ma_sound sound{};
	ma_decoder decoder{};
	bool isStreaming{};

	~PlayerData() 
	{
		ma_sound_uninit(&sound);
		if (isStreaming) ma_decoder_uninit(&decoder);
	}
};

static vector<string> supportedExtensions =
{
	".wav",
	".flac",
	".mp3",
	".ogg"
};

static ma_engine engine{};
unordered_map<u32, unique_ptr<PlayerData>> playerMap{};

static bool CheckInitState(
	const string& targetAction,
	bool originatesFromAudio = false);

static PlayerData* CommonChecker(
	const string& message,
	u32 ID);

static void CheckHugeValue(
	f32 value,
	const string& valueName,
	bool originatesFromAudio = false);

static void PrintErrorMessage(
	const string& message,
	bool originatesFromAudio = false);
static void PrintWarningMessage(
	const string& message,
	bool originatesFromAudio = false);

namespace KalaWindow::Core
{
	//
	// AUDIO CORE
	//

	bool Audio::Initialize(
		u32 listeners,
		SampleRate sampleRate)
	{
		if (isInitialized)
		{
			Log::Print(
				"Cannot initialize MiniAudio because it is already initialized!",
				"AUDIO",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		ma_engine_config config = ma_engine_config_init();

		//enforce listeners between 1 and 4 always
		u32 clamped = clamp(listeners, 1u, 4u);

		config.listenerCount = clamped;

		switch (sampleRate)
		{
		case SampleRate::SAMPLE_DEFAULT:
			config.sampleRate = 0;
			break;
		case SampleRate::SAMPLE_48000:
			config.sampleRate = ma_standard_sample_rate_48000;
			break;
		case SampleRate::SAMPLE_44100:
			config.sampleRate = ma_standard_sample_rate_44100;
			break;
		}

		if (ma_engine_init(&config, &engine) != MA_SUCCESS)
		{
			KalaWindowCore::ForceClose(
				"Audio error",
				"Failed to initialize MiniAudio!");

			return false;
		}

		Log::Print(
			"Initialized MiniAudio!",
			"AUDIO",
			LogType::LOG_SUCCESS);

		isInitialized = true;

		return true;
	}

	void Audio::Shutdown()
	{
		if (!CheckInitState("shut down MiniAudio", true)) return;
		isInitialized = false;

		playerMap.clear();

		ma_engine_uninit(&engine);

		Log::Print(
			"Shut down MiniAudio!",
			"AUDIO",
			LogType::LOG_SUCCESS);
	}

	//
	// EACH INDIVIDUAL AUDIO LISTENER
	//

	void AudioListener::SetMuteState(
		bool state,
		u32 ID)
	{
		if (!CheckInitState("set listener mute state", true)) return;

		ma_engine_listener_set_enabled(
			&engine,
			ID,
			state);

		string stateStr = state ? "true" : "false";

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + to_string(ID) + "' mute state to '" + stateStr + "'!",
				"AUDIO_LISTENER",
				LogType::LOG_INFO);
		}
	}
	bool AudioListener::IsMuted(u32 ID)
	{
		if (!CheckInitState("get listener mute state", true)) return false;

		return ma_engine_listener_is_enabled(
			&engine,
			ID);
	}

	void AudioListener::SetPosition(
		const vec3& pos,
		u32 ID)
	{
		if (!CheckInitState("set listener position", true)) return;

#ifdef _DEBUG
		CheckHugeValue(pos.x, "listener x position", true);
		CheckHugeValue(pos.y, "listener y position", true);
		CheckHugeValue(pos.z, "listener z position", true);
#endif

		ma_engine_listener_set_position(
			&engine,
			ID,
			pos.x,
			pos.y,
			pos.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set listener position to '"
				+ to_string(pos.x) + ", " + to_string(pos.y) + ", " + to_string(pos.z),
				"AUDIO_LISTENER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioListener::GetPosition(u32 ID)
	{
		if (!CheckInitState("get listener position", true)) return vec3();

		ma_vec3f pos = ma_engine_listener_get_position(
			&engine,
			ID);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioListener::SetWorldUp(
		const vec3& up,
		u32 ID)
	{
		if (!CheckInitState("set listener up", true)) return;

#ifdef _DEBUG
		CheckHugeValue(up.x, "listener x up", true);
		CheckHugeValue(up.y, "listener y up", true);
		CheckHugeValue(up.z, "listener z up", true);
#endif

		ma_engine_listener_set_world_up(
			&engine,
			ID,
			up.x,
			up.y,
			up.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set listener up to '"
				+ to_string(up.x) + ", " + to_string(up.y) + ", " + to_string(up.z),
				"AUDIO_LISTENER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioListener::GetWorldUp(u32 ID)
	{
		if (!CheckInitState("get listener up", true)) return vec3();

		ma_vec3f up = ma_engine_listener_get_world_up(
			&engine,
			ID);

		return vec3(up.x, up.y, up.z);
	}

	void AudioListener::SetVelocity(
		const vec3& vel,
		u32 ID)
	{
		if (!CheckInitState("set listener velocity", true)) return;

#ifdef _DEBUG
		CheckHugeValue(vel.x, "listener x velocity");
		CheckHugeValue(vel.y, "listener y velocity");
		CheckHugeValue(vel.z, "listener z velocity");
#endif

		ma_engine_listener_set_velocity(
			&engine,
			ID,
			vel.x,
			vel.y,
			vel.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set listener velocity to '"
				+ to_string(vel.x) + ", " + to_string(vel.y) + ", " + to_string(vel.z) + "'!",
				"AUDIO_LISTENER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioListener::GetVelocity(u32 ID)
	{
		if (!CheckInitState("get listener velocity", true)) return vec3();

		ma_vec3f pos = ma_engine_listener_get_velocity(
			&engine,
			ID);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioListener::SetDirection(
		const vec3& dir,
		u32 ID)
	{
		if (!CheckInitState("set listener direction", true)) return;

#ifdef _DEBUG
		CheckHugeValue(dir.x, "listener cone x direction");
		CheckHugeValue(dir.y, "listener cone y direction");
		CheckHugeValue(dir.z, "listener cone z direction");
#endif

		ma_engine_listener_set_direction(
			&engine,
			ID,
			dir.x,
			dir.y,
			dir.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set listener direction to '"
				+ to_string(dir.x) + ", " + to_string(dir.y) + ", " + to_string(dir.z),
				"AUDIO_LISTENER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioListener::GetDirection(u32 ID)
	{
		if (!CheckInitState("get listener direction", true)) return vec3();

		ma_vec3f front = ma_engine_listener_get_direction(
			&engine,
			ID);

		return vec3(front.x, front.y, front.z);
	}

	void AudioListener::SetConeData(
		const AudioCone& cone,
		u32 ID)
	{
		if (!CheckInitState("set listener cone data", true)) return;

		f32 innerAngleClamped = clamp(cone.innerConeAngle, 0.0f, 359.99f);
		f32 outerAngleClamped = clamp(cone.outerConeAngle, 0.0f, 359.99f);
		f32 outerGainClamped = clamp(cone.outerGain, 0.0f, 1.0f);

		if (outerAngleClamped < innerAngleClamped) outerAngleClamped = innerAngleClamped;

		ma_engine_listener_set_cone(
			&engine,
			ID,
			innerAngleClamped,
			outerAngleClamped,
			outerGainClamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set listener cone inner angle to '" + to_string(innerAngleClamped) + "', "
				+ "cone outer angle to '" + to_string(outerAngleClamped)
				+ "' and cone outer gain to '" + to_string(outerGainClamped) + "'!",
				"AUDIO_LISTENER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	AudioCone AudioListener::GetConeData(u32 ID)
	{
		AudioCone cone{};

		if (!CheckInitState("get listener cone data", true)) return cone;

		ma_vec3f front = ma_engine_listener_get_direction(
			&engine,
			0);

		f32 innerConeAngle{};
		f32 outerConeAngle{};
		f32 outerGain{};

		ma_engine_listener_get_cone(
			&engine,
			0,
			&innerConeAngle,
			&outerConeAngle,
			&outerGain);

		cone.innerConeAngle = innerConeAngle;
		cone.outerConeAngle = outerConeAngle;
		cone.outerGain = outerGain;

		return cone;
	}

	//
	// EACH INDIVIDUAL CREATED AUDIO PLAYER
	//

	AudioPlayer* AudioPlayer::CreateAudioPlayer(
		u32 windowID,
		const string& name,
		const string& filePath)
	{
		Window* window = GetValueByID<Window>(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Failed to create audio player '" + name + "' because its window reference is invalid!",
				"AUDIO_PLAYER",
				LogType::LOG_ERROR);

			return nullptr;
		}

		WindowContent* content{};
		if (windowContent.contains(window))
		{
			content = windowContent[window].get();
		}

		if (!content)
		{
			Log::Print(
				"Failed to create audio player '" + name + "' because its window '" + window->GetTitle() + "' is missing from window content!",
				"AUDIO_PLAYER",
				LogType::LOG_ERROR);

			return nullptr;
		}

		if (!CheckInitState("create audio player '" + name + "'")) return nullptr;

		if (!exists(filePath))
		{
			PrintErrorMessage(
				"Cannot create audio player '" + name
				+ "' because its file path '" + filePath + "' does not exist!");

			return nullptr;
		}

		if (!path(filePath).has_extension()
			|| !is_regular_file(filePath))
		{
			PrintErrorMessage(
				"Cannot create audio player '" + name
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
				"Cannot create audio player '" + name + "' because its extenion '" 
				+ extension + "' is unsupported! You must use .wav, .flac, .mp3 or .ogg only.");

			return nullptr;
		}

		u64 fileSize = file_size(filePath);

		unique_ptr<PlayerData> pData = make_unique<PlayerData>();

		//load full file into memory
		if (fileSize <= Audio::GetStreamThreshold())
		{
			if (ma_sound_init_from_file(
				&engine,
				filePath.c_str(),
				0,
				NULL,
				NULL,
				&pData->sound) != MA_SUCCESS)
			{
				KalaWindowCore::ForceClose(
					"Audio error",
					"Failed to create audio file '" + name + "' from path '" + filePath + "'!");

				return nullptr;
			}

			ostringstream oss{};
			oss << "Loaded audio file '" << filePath << "' into memory because its size '"
				<< to_string(fileSize) << "' is less or equal to stream limit '"
				<< to_string(Audio::GetStreamThreshold()) + "'.";

			Log::Print(
				oss.str(),
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
		//decode and stream audio instead of loading entirely into memory
		else
		{
			if (ma_decoder_init_file(
				filePath.c_str(),
				NULL,
				&pData->decoder) != MA_SUCCESS)
			{
				KalaWindowCore::ForceClose(
					"Audio error",
					"Failed to create audio file '" + name + "' from path '" + filePath + "'!");

				return nullptr;
			}

			ma_sound_init_from_data_source(
				&engine,
				&pData->decoder,
				MA_SOUND_FLAG_STREAM,
				NULL,
				&pData->sound);

			pData->isStreaming = true;

			ostringstream oss{};
			oss << "Streaming audio file '" << filePath << "' because its size '"
				<< to_string(fileSize) << "' is over the stream limit '"
				<< to_string(Audio::GetStreamThreshold()) + "'.";

			Log::Print(
				oss.str(),
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}

		ma_sound_set_volume(&pData->sound, 1.0f);
		ma_sound_set_pitch(&pData->sound, 1.0f);

		u32 newID = ++globalID;

		playerMap[newID] = move(pData);

		unique_ptr<AudioPlayer> newTrack = make_unique<AudioPlayer>();
		AudioPlayer* trackPtr = newTrack.get();

		trackPtr->name = name;
		trackPtr->filePath = filePath;
		trackPtr->ID = newID;
		trackPtr->windowID = windowID;

		content->audioPlayers[newID] = move(newTrack);
		content->runtimeAudioPlayers.push_back(trackPtr);

		Log::Print(
			"Created audio file '" + name + "' with ID '" + to_string(newID) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_SUCCESS);

		return trackPtr;
	}

	void AudioPlayer::SetName(const string& newName)
	{
		PlayerData* pData = CommonChecker(
			"set audio player name to '" + name + "'",
			ID);

		if (!pData) return;

		string oldName = name;

		auto PrintNameError = [&oldName, &newName](const string& reason)
			{
				PrintErrorMessage(
					"Cannot change audio player old name '" + oldName + "' to new name '" + newName + "'"
					"because " + reason + "!");
			};

		if (newName == name)
		{
			PrintNameError("the old name already is the same as the new name");

			return;
		}

		if (newName.empty())
		{
			PrintNameError("the new name is empty!");

			return;
		}

		if (newName.size() > 20)
		{
			PrintNameError("the new name is too long! You must use 20 characters or less.");

			return;
		}

		Window* window = GetValueByID<Window>(windowID);

		WindowContent* content{};
		if (window
			&& window->IsInitialized()
			&& windowContent.contains(window))
		{
			content = windowContent[window].get();
		}

		if (!content)
		{
			Log::Print(
				"Failed to update audio player '" + name + "' name because its window '" + window->GetTitle() + "' is missing from window content!",
				"AUDIO_PLAYER",
				LogType::LOG_ERROR);

			return;
		}

		name = newName;

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Changed audio player old name '" + oldName + "' to new name '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}

	void AudioPlayer::Play() const
	{
		PlayerData* pData = CommonChecker(
			"play audio player '" + name + "'",
			ID);

		if (!pData) return;

		//always move to start
		ma_sound_seek_to_pcm_frame(&pData->sound, 0);

		//start playing sound
		ma_sound_start(&pData->sound);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Started playing audio player '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	bool AudioPlayer::IsPlaying() const
	{
		PlayerData* pData = CommonChecker(
			"get play state for audio player '" + name + "'",
			ID);

		if (!pData) return false;

		return (ma_sound_is_playing(&pData->sound) == MA_TRUE);
	}

	void AudioPlayer::SetPlaybackPosition(u32 newPosition) const
	{
		PlayerData* pData = CommonChecker(
			"set playback position for audio player '" + name + "'",
			ID);

		if (!pData) return;

		if (newPosition > GetPlaybackPosition(true))
		{
			PrintErrorMessage(
				"Playback position '" + to_string(newPosition) + "' is past audio player '" + name + "' duration!");

			return;
		}

		ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);

		ma_uint64 frames = (ma_uint64)newPosition * (ma_uint64)sampleRate;

		ma_sound_seek_to_pcm_frame(&pData->sound, frames);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set playback position for audio player '" + name + "' to '" + to_string(newPosition) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	u32 AudioPlayer::GetPlaybackPosition(bool getFullDuration) const
	{
		PlayerData* pData = CommonChecker(
			"get playback position for audio player '" + name + "'",
			ID);

		if (!pData) return 0;

		ma_uint64 length{};

		//get full audio player duration
		if (getFullDuration) ma_sound_get_length_in_pcm_frames(&pData->sound, &length);

		//get duration up to current frame in audio player
		else ma_sound_get_cursor_in_pcm_frames(&pData->sound, &length);

		ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);

		f64 seconds = (f64)length / (f64)sampleRate;
		return u32(seconds + 0.5);
	}

	void AudioPlayer::Pause() const
	{
		PlayerData* pData = CommonChecker(
			"pause audio player '" + name + "'",
			ID);

		if (!pData) return;

		//pause song without resetting back to beginning
		ma_sound_stop(&pData->sound);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Paused audio player '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	void AudioPlayer::Continue() const
	{
		PlayerData* pData = CommonChecker(
			"continue audio player '" + name + "'",
			ID);

		if (!pData) return;

		//start playing sound but continue where we last left off
		ma_sound_start(&pData->sound);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Continuing playing audio player '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}

	void AudioPlayer::SetLoopState(bool newState) const
	{
		PlayerData* pData = CommonChecker(
			"set loop state for audio player '" + name + "'",
			ID);

		if (!pData) return;

		ma_sound_set_looping(&pData->sound, newState);

		string state = newState ? "true" : "false";

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' loop state to '" + state + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	bool AudioPlayer::CanLoop() const
	{
		PlayerData* pData = CommonChecker(
			"get loop state for audio player '" + name + "'",
			ID);

		if (!pData) return false;

		return (ma_sound_is_looping(&pData->sound) == MA_TRUE);
	}

	void AudioPlayer::Stop() const
	{
		PlayerData* pData = CommonChecker(
			"stop audio player '" + name + "'",
			ID);

		if (!pData) return;

		ma_sound_stop(&pData->sound);

		//reset the audio position to the start, so the next play starts fresh
		ma_sound_seek_to_pcm_frame(&pData->sound, 0);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Stopped playing audio player '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	bool AudioPlayer::HasFinished() const
	{
		PlayerData* pData = CommonChecker(
			"check if audio player '" + name + "' has finished",
			ID);

		if (!pData) return false;

		return (
			ma_sound_is_playing(&pData->sound) == MA_FALSE
			&& !isPaused);
	}

	void AudioPlayer::SetVolume(f32 newVolume) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' volume",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newVolume, 0.0f, 5.0f);

		ma_sound_set_volume(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' volume to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetVolume() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' volume",
			ID);

		if (!pData) return 0;

		return ma_sound_get_volume(&pData->sound);
	}

	void AudioPlayer::SetSpatializationState(bool state) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' spatialization state",
			ID);

		if (!pData) return;

		string stateVal = state ? "true" : "false";
		ma_sound_set_spatialization_enabled(&pData->sound, state);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' spatialization state to '" + stateVal + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	bool AudioPlayer::GetSpatializationState() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' spatialization state",
			ID);

		if (!pData) return false;

		return ma_sound_is_spatialization_enabled(&pData->sound);
	}

	void AudioPlayer::SetPositioningState(Positioning pos) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' positioning state",
			ID);

		if (!pData) return;

		ma_positioning val = pos == Positioning::Positioning_Relative
			? ma_positioning_relative
			: ma_positioning_absolute;

		string valStr = pos == Positioning::Positioning_Relative
			? "relative"
			: "absolute";

		ma_sound_set_positioning(&pData->sound, val);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' positioning state to '" + valStr + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	Positioning AudioPlayer::GetPositioningState() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' positioning state",
			ID);

		if (!pData) return Positioning::Positioning_Relative;

		ma_positioning state = ma_sound_get_positioning(&pData->sound);
		return state == ma_positioning_relative
			? Positioning::Positioning_Relative
			: Positioning::Positioning_Absolute;
	}

	void AudioPlayer::SetPitch(f32 newPitch) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' pitch",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newPitch, 0.0f, 5.0f);

		ma_sound_set_pitch(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' pitch to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetPitch() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' pitch",
			ID);

		if (!pData) return 0;

		return ma_sound_get_pitch(&pData->sound);
	}

	void AudioPlayer::SetPan(f32 newPan) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' pan",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newPan, -1.0f, 1.0f);

		ma_sound_set_pan(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' pan to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetPan() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' pan",
			ID);

		if (!pData) return 0;

		return ma_sound_get_pan(&pData->sound);
	}

	void AudioPlayer::SetPanMode(PanMode mode) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' pan mode",
			ID);

		if (!pData) return;

		ma_pan_mode val = mode == PanMode::PanMode_Balance
			? ma_pan_mode_balance
			: ma_pan_mode_pan;
		string valStr = mode == PanMode::PanMode_Balance
			? "balance"
			: "pan";

		ma_sound_set_pan_mode(&pData->sound, val);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' pan mode to '" + valStr + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	PanMode AudioPlayer::GetPanMode() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' pan mode",
			ID);

		if (!pData) return PanMode::PanMode_Balance;

		ma_pan_mode mode = ma_sound_get_pan_mode(&pData->sound);
		return mode == ma_pan_mode_balance 
			? PanMode::PanMode_Balance 
			: PanMode::PanMode_Pan;
	}

	void AudioPlayer::SetPosition(const vec3& pos) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' player position",
			ID);

		if (!pData) return;

#ifdef _DEBUG
		CheckHugeValue(pos.x, "player x position");
		CheckHugeValue(pos.y, "player y position");
		CheckHugeValue(pos.z, "player z position");
#endif

		ma_sound_set_position(
			&pData->sound,
			pos.x,
			pos.y,
			pos.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' position to '"
				+ to_string(pos.x) + ", " + to_string(pos.y) + ", " + to_string(pos.z) + "'!",
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioPlayer::GetPosition() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' player position",
			ID);

		if (!pData) return vec3(0);

		ma_vec3f pos = ma_sound_get_position(&pData->sound);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioPlayer::SetVelocity(const vec3& vel) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' player velocity",
			ID);

		if (!pData) return;

#ifdef _DEBUG
		CheckHugeValue(vel.x, "player x velocity");
		CheckHugeValue(vel.y, "player y velocity");
		CheckHugeValue(vel.z, "player z velocity");
#endif

		ma_sound_set_velocity(
			&pData->sound,
			vel.x,
			vel.y,
			vel.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' velocity to '"
				+ to_string(vel.x) + ", " + to_string(vel.y) + ", " + to_string(vel.z) + "'!",
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioPlayer::GetVelocity() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' player velocity",
			ID);

		if (!pData) return vec3(0);

		ma_vec3f pos = ma_sound_get_velocity(&pData->sound);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioPlayer::SetDirection(const vec3& dir) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' player direction",
			ID);

		if (!pData) return;

#ifdef _DEBUG
		CheckHugeValue(dir.x, "player direction x");
		CheckHugeValue(dir.y, "player direction y");
		CheckHugeValue(dir.z, "player direction z");
#endif

		ma_sound_set_direction(
			&pData->sound,
			dir.x,
			dir.y,
			dir.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set player direction to '"
				+ to_string(dir.x) + ", " + to_string(dir.y) + ", " + to_string(dir.z),
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	vec3 AudioPlayer::GetDirection() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' direction",
			ID);

		if (!pData) return vec3();

		ma_vec3f front = ma_sound_get_direction(&pData->sound);

		return vec3(front.x, front.y, front.z);
	}
	
	void AudioPlayer::SetConeData(const AudioCone& cone) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' player cone data",
			ID);

		if (!pData) return;

		f32 innerAngleClamped = clamp(cone.innerConeAngle, 0.0f, 359.99f);
		f32 outerAngleClamped = clamp(cone.outerConeAngle, 0.0f, 359.99f);
		f32 outerGainClamped = clamp(cone.outerGain, 0.0f, 1.0f);

		if (outerAngleClamped < innerAngleClamped) outerAngleClamped = innerAngleClamped;

		ma_sound_set_cone(
			&pData->sound,
			innerAngleClamped,
			outerAngleClamped,
			outerGainClamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player cone inner angle to '" + to_string(innerAngleClamped) + "', "
				+ "cone outer angle to '" + to_string(outerAngleClamped)
				+ "' and cone outer gain to '" + to_string(outerGainClamped) + "'!",
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_INFO);
		}
	}
	AudioCone AudioPlayer::GetConeData() const
	{
		AudioCone cone{};

		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' cone data",
			ID);

		if (!pData) return cone;

		f32 innerConeAngle{};
		f32 outerConeAngle{};
		f32 outerGain{};

		ma_sound_get_cone(
			&pData->sound,
			&innerConeAngle,
			&outerConeAngle,
			&outerGain);

		cone.innerConeAngle = innerConeAngle;
		cone.outerConeAngle = outerConeAngle;
		cone.outerGain = outerGain;

		return cone;
	}

	void AudioPlayer::SetAttenuationModel(AttenuationModel model) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' attenuation model",
			ID);

		if (!pData) return;

		switch (model)
		{
		case AttenuationModel::Attenuation_None:
			ma_sound_set_attenuation_model(&pData->sound, ma_attenuation_model_none);

			if (Audio::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Set audio player '" + name + "' attenuation mode to 'none'!",
					"AUDIO_PLAYER",
					LogType::LOG_INFO);
			}

			break;
		case AttenuationModel::Attenuation_Inverse:
			ma_sound_set_attenuation_model(&pData->sound, ma_attenuation_model_inverse);

			if (Audio::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Set audio player '" + name + "' attenuation mode to 'inverse'!",
					"AUDIO_PLAYER",
					LogType::LOG_INFO);
			}

			break;
		case AttenuationModel::Attenuation_Linear:
			ma_sound_set_attenuation_model(&pData->sound, ma_attenuation_model_linear);

			if (Audio::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Set audio player '" + name + "' attenuation mode to 'linear'!",
					"AUDIO_PLAYER",
					LogType::LOG_INFO);
			}

			break;
		case AttenuationModel::Attenuation_Exponential:
			ma_sound_set_attenuation_model(&pData->sound, ma_attenuation_model_exponential);

			if (Audio::IsVerboseLoggingEnabled())
			{
				Log::Print(
					"Set audio player '" + name + "' attenuation mode to 'exponential'!",
					"AUDIO_PLAYER",
					LogType::LOG_INFO);
			}

			break;
		}
	}
	AttenuationModel AudioPlayer::GetAttenuationModel() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' attenuation model",
			ID);

		if (!pData) return AttenuationModel::Attenuation_None;

		ma_attenuation_model model = ma_sound_get_attenuation_model(&pData->sound);

		if (model == ma_attenuation_model_inverse)          return AttenuationModel::Attenuation_Inverse;
		else if (model == ma_attenuation_model_linear)      return AttenuationModel::Attenuation_Linear;
		else if (model == ma_attenuation_model_exponential) return AttenuationModel::Attenuation_Exponential;

		return AttenuationModel::Attenuation_None;
	}

	void AudioPlayer::SetRolloff(f32 newRolloffFactor) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' rolloff factor",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newRolloffFactor, 0.0f, 5.0f);

		ma_sound_set_rolloff(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' rolloff factor to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetRolloff() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' rolloff factor",
			ID);

		if (!pData) return 0;

		return ma_sound_get_rolloff(&pData->sound);
	}

	void AudioPlayer::SetDopplerFactor(f32 newFactor) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' doppler factor",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newFactor, 0.0f, 5.0f);

		ma_sound_set_doppler_factor(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' doppler factor to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetDopplerFactor() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' doppler factor",
			ID);

		if (!pData) return 0;

		return ma_sound_get_doppler_factor(&pData->sound);
	}

	void AudioPlayer::SetMinGain(f32 newMinGain) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' min gain",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newMinGain, 0.0f, GetMaxGain() - 0.1f);

		ma_sound_set_min_gain(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' min gain to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetMinGain() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' min gain",
			ID);

		if (!pData) return 0;

		return ma_sound_get_min_gain(&pData->sound);
	}

	void AudioPlayer::SetMaxGain(f32 newMaxGain) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' max gain",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newMaxGain, GetMinGain() + 0.1f, 5.0f);

		ma_sound_set_max_gain(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' max gain to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetMaxGain() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' max gain",
			ID);

		if (!pData) return 0;

		return ma_sound_get_max_gain(&pData->sound);
	}

	void AudioPlayer::SetMinRange(f32 newMinRange) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' min range",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newMinRange, 0.0f, GetMaxRange() - 0.1f);

		ma_sound_set_min_distance(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' min range to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetMinRange() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' min range",
			ID);

		if (!pData) return 0;

		return ma_sound_get_min_distance(&pData->sound);
	}

	void AudioPlayer::SetMaxRange(f32 newMaxRange) const
	{
		PlayerData* pData = CommonChecker(
			"set audio player '" + name + "' max range",
			ID);

		if (!pData) return;

		f32 clamped = clamp(newMaxRange, GetMinRange() + 0.1f, 1000.0f);

		ma_sound_set_max_distance(&pData->sound, clamped);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' max range to '" + to_string(clamped) + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_INFO);
		}
	}
	f32 AudioPlayer::GetMaxRange() const
	{
		PlayerData* pData = CommonChecker(
			"get audio player '" + name + "' max range",
			ID);

		if (!pData) return 0;

		return ma_sound_get_max_distance(&pData->sound);
	}

	AudioPlayer::~AudioPlayer()
	{
		auto it = playerMap.find(ID);
		if (it != playerMap.end())
		{
			playerMap.erase(it);

			Log::Print(
				"Destroyed audio player '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_SUCCESS);
		}
	}
}

bool CheckInitState(
	const string& targetAction,
	bool originatesFromAudio)
{
	if (!Audio::IsInitialized())
	{
		PrintErrorMessage(
			"Cannot " + targetAction + " because MiniAudio is not initialized!",
			originatesFromAudio);

		return false;
	}

	return true;
}

PlayerData* CommonChecker(
	const string& message,
	u32 ID)
{
	if (!CheckInitState(message)) return nullptr;

	auto it = playerMap.find(ID);
	if (it != playerMap.end())
	{

		return it->second.get();
	}

	PrintErrorMessage(
		"Cannot " + message + " because the audio pointer ID '" + to_string(ID) + "' was not found in internal MiniAudio sound map!",
		false);

	return nullptr;
}

void CheckHugeValue(
	f32 value,
	const string& valueName,
	bool originatesFromAudio)
{
	if (value < -10000.0f
		|| value > 10000.0f)
	{
		PrintWarningMessage(
			"Value '" + valueName + "' is outside the normal range [-10000, 10000]! Consider validating it.");
	}
}

void PrintErrorMessage(
	const string& message,
	bool originatesFromAudio)
{
	string type = originatesFromAudio ? "AUDIO" : "AUDIO_PLAYER";

	Log::Print(
		message,
		type,
		LogType::LOG_ERROR,
		2);
}
void PrintWarningMessage(
	const string& message,
	bool originatesFromAudio)
{
	string type = originatesFromAudio ? "AUDIO" : "AUDIO_PLAYER";

	Log::Print(
		message,
		type,
		LogType::LOG_WARNING,
		2);
}