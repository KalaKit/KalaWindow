//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "core/audio.hpp"
#include "core/containers.hpp"
#include "core/core.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::Audio;
using KalaWindow::Core::KalaWindowCore;

using std::unordered_map;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::vector;
using std::clamp;

static vector<string> supportedExtensions =
{
	".wav",
	".flac",
	".mp3",
	".ogg"
};

static ma_engine engine{};
unordered_map<u32, unique_ptr<ma_sound>> playerMap{};

static bool CheckInitState(
	const string& targetAction,
	bool originatesFromAudio = false);

static bool CheckListenerID(u32 ID);

static ma_sound* CommonChecker(
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

		for (const auto& [_, sound] : playerMap)
		{
			ma_sound_uninit(sound.get());
		}
		playerMap.clear();

		createdAudioPlayers.clear(); //also clear all created audio players

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

		if (!CheckListenerID(ID)) return;

		ma_engine_listener_set_enabled(
			&engine,
			ID,
			state);

		string stateStr = state ? "true" : "false";

		Log::Print(
			"Set audio player '" + to_string(ID) + "' mute state to '" + stateStr + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	bool AudioListener::IsMuted(u32 ID)
	{
		if (!CheckInitState("get listener mute state", true)) return false;

		if (!CheckListenerID(ID)) return false;

		return ma_engine_listener_is_enabled(
			&engine,
			ID);
	}

	void AudioListener::SetPosition(
		const vec3& pos,
		u32 ID)
	{
		if (!CheckInitState("set listener position", true)) return;

		if (!CheckListenerID(ID)) return;

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
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioListener::GetPosition(u32 ID)
	{
		if (!CheckInitState("get listener position", true)) return vec3();

		if (!CheckListenerID(ID)) return vec3();

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

		if (!CheckListenerID(ID)) return;

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
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioListener::GetWorldUp(u32 ID)
	{
		if (!CheckInitState("get listener up", true)) return vec3();

		if (!CheckListenerID(ID)) return vec3();

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

		if (!CheckListenerID(ID)) return;

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
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioListener::GetVelocity(u32 ID)
	{
		if (!CheckInitState("get listener velocity", true)) return vec3();

		if (!CheckListenerID(ID)) return vec3();

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

		if (!CheckListenerID(ID)) return;

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
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioListener::GetDirection(u32 ID)
	{
		if (!CheckInitState("get listener direction", true)) return vec3();

		if (!CheckListenerID(ID)) return vec3();

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

		if (!CheckListenerID(ID)) return;

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
				LogType::LOG_DEBUG);
		}
	}
	AudioCone AudioListener::GetConeData(u32 ID)
	{
		AudioCone cone{};

		if (!CheckInitState("get listener cone data", true)) return cone;

		if (!CheckListenerID(ID)) return cone;

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
		const string& name,
		const string& filePath)
	{
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

		unique_ptr<ma_sound> sound = make_unique<ma_sound>();
		if (ma_sound_init_from_file(
			&engine,
			filePath.c_str(),
			0,
			NULL,
			NULL,
			sound.get()) != MA_SUCCESS)
		{
			KalaWindowCore::ForceClose(
				"Audio error",
				"Failed to create audio file '" + name + "' from path '" + filePath + "'!");

			return nullptr;
		}
		ma_sound_set_volume(sound.get(), 1.0f);
		ma_sound_set_pitch(sound.get(), 1.0f);

		u32 newID = ++globalID;

		playerMap[newID] = move(sound);

		unique_ptr<AudioPlayer> newTrack = make_unique<AudioPlayer>();
		newTrack->name = name;
		newTrack->filePath = filePath;
		newTrack->ID = newID;

		createdAudioPlayers[newID] = move(newTrack);

		AudioPlayer* newPlayer = createdAudioPlayers[newID].get();
		runtimeAudioPlayers.push_back(newPlayer);

		Log::Print(
			"Created audio file '" + name + "' with ID '" + to_string(newID) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_SUCCESS);

		return newPlayer;
	}

	void AudioPlayer::SetName(const string& newName)
	{
		ma_sound* sound = CommonChecker(
			"set audio player name to '" + name + "'",
			ID);

		if (sound == nullptr) return;

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

		for (const auto& [_, track] : createdAudioPlayers)
		{
			if (track->GetName() == newName)
			{
				PrintNameError("an audio player with the new name already exists!");

				return;
			}
		}

		name = newName;

		Log::Print(
			"Changed audio player old name '" + oldName + "' to new name '" + name + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}

	void AudioPlayer::Play() const
	{
		ma_sound* sound = CommonChecker(
			"play audio player '" + name + "'",
			ID);

		if (sound == nullptr) return;

		//always move to start
		ma_sound_seek_to_pcm_frame(sound, 0);

		//start playing sound
		ma_sound_start(sound);

		Log::Print(
			"Started playing audio player '" + name + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	bool AudioPlayer::IsPlaying() const
	{
		ma_sound* sound = CommonChecker(
			"get play state for audio player '" + name + "'",
			ID);

		if (sound == nullptr) return false;

		return (ma_sound_is_playing(sound) == MA_TRUE);
	}

	void AudioPlayer::SetPlaybackPosition(u32 newPosition) const
	{
		ma_sound* sound = CommonChecker(
			"set playback position for audio player '" + name + "'",
			ID);

		if (sound == nullptr) return;

		if (newPosition > GetPlaybackPosition(true))
		{
			PrintErrorMessage(
				"Playback position '" + to_string(newPosition) + "' is past audio player '" + name + "' duration!");

			return;
		}

		ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);

		ma_uint64 frames = (ma_uint64)newPosition * (ma_uint64)sampleRate;

		ma_sound_seek_to_pcm_frame(sound, frames);

		Log::Print(
			"Set playback position for audio player '" + name + "' to '" + to_string(newPosition) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	u32 AudioPlayer::GetPlaybackPosition(bool getFullDuration) const
	{
		ma_sound* sound = CommonChecker(
			"get playback position for audio player '" + name + "'",
			ID);

		if (sound == nullptr) return 0;

		ma_uint64 length{};

		//get full audio player duration
		if (getFullDuration) ma_sound_get_length_in_pcm_frames(sound, &length);

		//get duration up to current frame in audio player
		else ma_sound_get_cursor_in_pcm_frames(sound, &length);

		ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);

		f64 seconds = (f64)length / (f64)sampleRate;
		return u32(seconds + 0.5);
	}

	void AudioPlayer::Pause() const
	{
		ma_sound* sound = CommonChecker(
			"pause audio player '" + name + "'",
			ID);

		if (sound == nullptr) return;

		//pause song without resetting back to beginning
		ma_sound_stop(sound);

		Log::Print(
			"Paused audio player '" + name + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	void AudioPlayer::Continue() const
	{
		ma_sound* sound = CommonChecker(
			"continue audio player '" + name + "'",
			ID);

		if (sound == nullptr) return;

		//start playing sound but continue where we last left off
		ma_sound_start(sound);

		Log::Print(
			"Continuing playing audio player '" + name + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}

	void AudioPlayer::SetLoopState(bool newState) const
	{
		ma_sound* sound = CommonChecker(
			"set loop state for audio player '" + name + "'",
			ID);

		if (sound == nullptr) return;

		ma_sound_set_looping(sound, newState);

		string state = newState ? "true" : "false";

		Log::Print(
			"Set audio player '" + name + "' loop state to '" + state + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	bool AudioPlayer::CanLoop() const
	{
		ma_sound* sound = CommonChecker(
			"get loop state for audio player '" + name + "'",
			ID);

		if (sound == nullptr) return false;

		return (ma_sound_is_looping(sound) == MA_TRUE);
	}

	void AudioPlayer::Stop() const
	{
		ma_sound* sound = CommonChecker(
			"stop audio player '" + name + "'",
			ID);

		if (sound == nullptr) return;

		ma_sound_stop(sound);

		//reset the audio position to the start, so the next play starts fresh
		ma_sound_seek_to_pcm_frame(sound, 0);

		Log::Print(
			"Stopped playing audio player '" + name + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	bool AudioPlayer::HasFinished() const
	{
		ma_sound* sound = CommonChecker(
			"check if audio player '" + name + "' has finished",
			ID);

		if (sound == nullptr) return false;

		return (
			ma_sound_is_playing(sound) == MA_FALSE
			&& !isPaused);
	}

	void AudioPlayer::SetVolume(f32 newVolume) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' volume",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newVolume, 0.0f, 5.0f);

		ma_sound_set_volume(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' volume to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetVolume() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' volume",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_volume(sound);
	}

	void AudioPlayer::SetSpatializationState(bool state) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' spatialization state",
			ID);

		if (sound == nullptr) return;

		if (state)
		{
			Log::Print(
				"Set audio player '" + name + "' spatialization state to 'true'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			ma_sound_set_spatialization_enabled(sound, true);
		}
		else
		{
			Log::Print(
				"Set audio player '" + name + "' spatialization state to 'false'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			ma_sound_set_spatialization_enabled(sound, false);
		}
	}
	bool AudioPlayer::GetSpatializationState() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' spatialization state",
			ID);

		if (sound == nullptr) return false;

		return ma_sound_is_spatialization_enabled(sound);
	}

	void AudioPlayer::SetPositioningState(Positioning pos) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' positioning state",
			ID);

		if (sound == nullptr) return;

		switch (pos)
		{
		case Positioning::Positioning_Relative:

			Log::Print(
				"Set audio player '" + name + "' positioning state to 'relative'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			ma_sound_set_positioning(sound, ma_positioning_relative);
			break;
		
		case Positioning::Positioning_Absolute:

			Log::Print(
				"Set audio player '" + name + "' positioning state to 'absolute'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			ma_sound_set_positioning(sound, ma_positioning_absolute);
			break;
		}
	}
	Positioning AudioPlayer::GetPositioningState() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' positioning state",
			ID);

		if (sound == nullptr) return Positioning::Positioning_Relative;

		ma_positioning state = ma_sound_get_positioning(sound);
		return state == ma_positioning_relative
			? Positioning::Positioning_Relative
			: Positioning::Positioning_Absolute;
	}

	void AudioPlayer::SetPitch(f32 newPitch) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' pitch",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newPitch, 0.0f, 5.0f);

		ma_sound_set_pitch(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' pitch to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetPitch() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' pitch",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_pitch(sound);
	}

	void AudioPlayer::SetPan(f32 newPan) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' pan",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newPan, -1.0f, 1.0f);

		ma_sound_set_pan(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' pan to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetPan() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' pan",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_pan(sound);
	}

	void AudioPlayer::SetPanMode(PanMode mode) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' pan mode",
			ID);

		if (sound == nullptr) return;

		switch (mode)
		{
		case PanMode::PanMode_Balance:
			ma_sound_set_pan_mode(sound, ma_pan_mode_balance);

			Log::Print(
				"Set audio player '" + name + "' pan mode to 'balance'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			break;
		case PanMode::PanMode_Pan:
			ma_sound_set_pan_mode(sound, ma_pan_mode_pan);

			Log::Print(
				"Set audio player '" + name + "' pan mode to 'pan'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			break;
		}
	}
	PanMode AudioPlayer::GetPanMode() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' pan mode",
			ID);

		if (sound == nullptr) return PanMode::PanMode_Balance;

		ma_pan_mode mode = ma_sound_get_pan_mode(sound);
		return mode == ma_pan_mode_balance 
			? PanMode::PanMode_Balance 
			: PanMode::PanMode_Pan;
	}

	void AudioPlayer::SetPosition(const vec3& pos) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' player position",
			ID);

		if (sound == nullptr) return;

#ifdef _DEBUG
		CheckHugeValue(pos.x, "player x position");
		CheckHugeValue(pos.y, "player y position");
		CheckHugeValue(pos.z, "player z position");
#endif

		ma_sound_set_position(
			sound,
			pos.x,
			pos.y,
			pos.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' position to '"
				+ to_string(pos.x) + ", " + to_string(pos.y) + ", " + to_string(pos.z) + "'!",
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioPlayer::GetPosition() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' player position",
			ID);

		if (sound == nullptr) return vec3(0);

		ma_vec3f pos = ma_sound_get_position(sound);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioPlayer::SetVelocity(const vec3& vel) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' player velocity",
			ID);

		if (sound == nullptr) return;

#ifdef _DEBUG
		CheckHugeValue(vel.x, "player x velocity");
		CheckHugeValue(vel.y, "player y velocity");
		CheckHugeValue(vel.z, "player z velocity");
#endif

		ma_sound_set_velocity(
			sound,
			vel.x,
			vel.y,
			vel.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set audio player '" + name + "' velocity to '"
				+ to_string(vel.x) + ", " + to_string(vel.y) + ", " + to_string(vel.z) + "'!",
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioPlayer::GetVelocity() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' player velocity",
			ID);

		if (sound == nullptr) return vec3(0);

		ma_vec3f pos = ma_sound_get_velocity(sound);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioPlayer::SetDirection(const vec3& dir) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' player direction",
			ID);

		if (sound == nullptr) return;

#ifdef _DEBUG
		CheckHugeValue(dir.x, "player direction x");
		CheckHugeValue(dir.y, "player direction y");
		CheckHugeValue(dir.z, "player direction z");
#endif

		ma_sound_set_direction(
			sound,
			dir.x,
			dir.y,
			dir.z);

		if (Audio::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set player direction to '"
				+ to_string(dir.x) + ", " + to_string(dir.y) + ", " + to_string(dir.z),
				"AUDIO_PLAYER_VERBOSE",
				LogType::LOG_DEBUG);
		}
	}
	vec3 AudioPlayer::GetDirection() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' direction",
			ID);

		if (sound == nullptr) return vec3();

		ma_vec3f front = ma_sound_get_direction(sound);

		return vec3(front.x, front.y, front.z);
	}
	
	void AudioPlayer::SetConeData(const AudioCone& cone) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' player cone data",
			ID);

		if (sound == nullptr) return;

		f32 innerAngleClamped = clamp(cone.innerConeAngle, 0.0f, 359.99f);
		f32 outerAngleClamped = clamp(cone.outerConeAngle, 0.0f, 359.99f);
		f32 outerGainClamped = clamp(cone.outerGain, 0.0f, 1.0f);

		if (outerAngleClamped < innerAngleClamped) outerAngleClamped = innerAngleClamped;

		ma_sound_set_cone(
			sound,
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
				LogType::LOG_DEBUG);
		}
	}
	AudioCone AudioPlayer::GetConeData() const
	{
		AudioCone cone{};

		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' cone data",
			ID);

		if (sound == nullptr) return cone;

		f32 innerConeAngle{};
		f32 outerConeAngle{};
		f32 outerGain{};

		ma_sound_get_cone(
			sound,
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
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' attenuation model",
			ID);

		if (sound == nullptr) return;

		switch (model)
		{
		case AttenuationModel::Attenuation_None:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_none);

			Log::Print(
				"Set audio player '" + name + "' attenuation mode to 'none'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			break;
		case AttenuationModel::Attenuation_Inverse:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_inverse);

			Log::Print(
				"Set audio player '" + name + "' attenuation mode to 'inverse'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			break;
		case AttenuationModel::Attenuation_Linear:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_linear);

			Log::Print(
				"Set audio player '" + name + "' attenuation mode to 'linear'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			break;
		case AttenuationModel::Attenuation_Exponential:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_exponential);

			Log::Print(
				"Set audio player '" + name + "' attenuation mode to 'exponential'!",
				"AUDIO_PLAYER",
				LogType::LOG_DEBUG);

			break;
		}
	}
	AttenuationModel AudioPlayer::GetAttenuationModel() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' attenuation model",
			ID);

		if (sound == nullptr) return AttenuationModel::Attenuation_None;

		ma_attenuation_model model = ma_sound_get_attenuation_model(sound);

		if (model == ma_attenuation_model_inverse)          return AttenuationModel::Attenuation_Inverse;
		else if (model == ma_attenuation_model_linear)      return AttenuationModel::Attenuation_Linear;
		else if (model == ma_attenuation_model_exponential) return AttenuationModel::Attenuation_Exponential;

		return AttenuationModel::Attenuation_None;
	}

	void AudioPlayer::SetRolloff(f32 newRolloffFactor) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' rolloff factor",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newRolloffFactor, 0.0f, 5.0f);

		ma_sound_set_rolloff(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' rolloff factor to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetRolloff() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' rolloff factor",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_rolloff(sound);
	}

	void AudioPlayer::SetDopplerFactor(f32 newFactor) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' doppler factor",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newFactor, 0.0f, 5.0f);

		ma_sound_set_doppler_factor(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' doppler factor to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetDopplerFactor() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' doppler factor",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_doppler_factor(sound);
	}

	AudioPlayer::~AudioPlayer()
	{
		auto it = playerMap.find(ID);
		if (it != playerMap.end())
		{
			ma_sound_uninit(it->second.get());
			playerMap.erase(it);

			Log::Print(
				"Destroyed audio player '" + name + "'!",
				"AUDIO_PLAYER",
				LogType::LOG_SUCCESS);
		}
	}

	void AudioPlayer::SetMinGain(f32 newMinGain) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' min gain",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMinGain, 0.0f, GetMaxGain() - 0.1f);

		ma_sound_set_min_gain(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' min gain to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetMinGain() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' min gain",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_min_gain(sound);
	}

	void AudioPlayer::SetMaxGain(f32 newMaxGain) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' max gain",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMaxGain, GetMinGain() + 0.1f, 5.0f);

		ma_sound_set_max_gain(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' max gain to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetMaxGain() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' max gain",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_max_gain(sound);
	}

	void AudioPlayer::SetMinRange(f32 newMinRange) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' min range",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMinRange, 0.0f, GetMaxRange() - 0.1f);

		ma_sound_set_min_distance(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' min range to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetMinRange() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' min range",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_min_distance(sound);
	}

	void AudioPlayer::SetMaxRange(f32 newMaxRange) const
	{
		ma_sound* sound = CommonChecker(
			"set audio player '" + name + "' max range",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMaxRange, GetMinRange() + 0.1f, 1000.0f);

		ma_sound_set_max_distance(sound, clamped);

		Log::Print(
			"Set audio player '" + name + "' max range to '" + to_string(clamped) + "'!",
			"AUDIO_PLAYER",
			LogType::LOG_DEBUG);
	}
	f32 AudioPlayer::GetMaxRange() const
	{
		ma_sound* sound = CommonChecker(
			"get audio player '" + name + "' max range",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_max_distance(sound);
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

bool CheckListenerID(u32 ID)
{
	if (ID == 0)
	{
		PrintErrorMessage(
			"Listener ID " + to_string(ID) + " is too low! It must be atleast 1.");

		return false;
	}

	if (ID > 4)
	{
		PrintErrorMessage(
			"Listener ID " + to_string(ID) + " is too high! It must be below 4.");

		return false;
	}

	return true;
}

ma_sound* CommonChecker(
	const string& message,
	u32 ID)
{
	if (!CheckInitState(message)) return nullptr;

	auto it = playerMap.find(ID);
	if (it != playerMap.end()) return it->second.get();

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