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
#include "core/log.hpp"
#include "core/containers.hpp"
#include "core/core.hpp"

using KalaWindow::Core::Audio;
using KalaWindow::Core::Logger;
using KalaWindow::Core::LogType;
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
unordered_map<u32, unique_ptr<ma_sound>> soundMap{};

static ma_sound* CommonChecker(
	const string& message,
	u32 ID);
static bool CheckInitState(const string& targetAction);

static void PrintErrorMessage(const string& message);

static void CheckHugeValue(f32 value, const string& valueName);
static void PrintWarningMessage(const string& message);

namespace KalaWindow::Core
{
	//
	// AUDIO CORE
	//

	bool Audio::Initialize()
	{
		if (isInitialized)
		{
			Logger::Print(
				"Cannot initialize MiniAudio because it is already initialized!",
				"AUDIO",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
		{
			KalaWindowCore::ForceClose(
				"Audio error",
				"Failed to initialize MiniAudio!");

			return false;
		}

		Logger::Print(
			"Initialized MiniAudio!",
			"AUDIO",
			LogType::LOG_SUCCESS);

		isInitialized = true;

		return true;
	}

	void Audio::SetListenerPosition(const AudioListener& listener)
	{
		if (!CheckInitState("set listener position")) return;

#ifdef _DEBUG
		CheckHugeValue(listener.pos.x, "listener x position");
		CheckHugeValue(listener.pos.y, "listener y position");
		CheckHugeValue(listener.pos.z, "listener z position");

		CheckHugeValue(listener.front.x, "listener x direction");
		CheckHugeValue(listener.front.y, "listener y direction");
		CheckHugeValue(listener.front.z, "listener z direction");

		CheckHugeValue(listener.up.x, "listener x up");
		CheckHugeValue(listener.up.y, "listener y up");
		CheckHugeValue(listener.up.z, "listener z up");
#endif

		ma_engine_listener_set_position(
			&engine,
			0,
			listener.pos.x,
			listener.pos.y,
			listener.pos.z);

		ma_engine_listener_set_direction(
			&engine,
			0,
			listener.front.x,
			listener.front.y,
			listener.front.z);

		ma_engine_listener_set_world_up(
			&engine,
			0,
			listener.up.x,
			listener.up.y,
			listener.up.z);

		Logger::Print(
			"Set audio listener position to '"
			+ to_string(listener.pos.x) + ", " + to_string(listener.pos.y) + ", " + to_string(listener.pos.z)
			+ ", audio listener direction to '"
			+ to_string(listener.front.x) + ", " + to_string(listener.front.y) + ", " + to_string(listener.front.z)
			+ "' and audio listener world up to '"
			+ to_string(listener.up.x) + ", " + to_string(listener.up.y) + ", " + to_string(listener.up.z) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	AudioListener GetListenerPosition()
	{
		AudioListener listener{};

		if (!CheckInitState("get listener position")) return listener;

		ma_vec3f pos = ma_engine_listener_get_position(&engine, 0);
		ma_vec3f front = ma_engine_listener_get_direction(&engine, 0);
		ma_vec3f up = ma_engine_listener_get_world_up(&engine, 0);

		listener.pos = vec3(pos.x, pos.y, pos.z);
		listener.front = vec3(front.x, front.y, front.z);
		listener.up = vec3(up.x, up.y, up.z);

		return listener;
	}

	void Audio::Shutdown()
	{
		if (!CheckInitState("shut down MiniAudio")) return;
		isInitialized = false;

		for (const auto& [_, sound] : soundMap)
		{
			ma_sound_uninit(sound.get());
		}
		soundMap.clear();

		createdAudioTracks.clear(); //also clear all created audio files

		ma_engine_uninit(&engine);

		Logger::Print(
			"Shut down MiniAudio!",
			"AUDIO",
			LogType::LOG_SUCCESS);
	}

	//
	// EACH INDIVIDUAL IMPORTED AUDIO FILE
	//

	AudioTrack* AudioTrack::ImportAudioTrack(
		const string& name,
		const string& filePath)
	{
		if (!CheckInitState("import audio track '" + name + "'")) return nullptr;

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
				"Cannot import audio track '" + name + "' because its extenion '" 
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
				"Failed to import audio file '" + name + "' from path '" + filePath + "'!");

			return nullptr;
		}
		ma_sound_set_volume(sound.get(), 1.0f);
		ma_sound_set_pitch(sound.get(), 1.0f);

		u32 newID = ++globalID;

		soundMap[newID] = move(sound);

		unique_ptr<AudioTrack> newTrack = make_unique<AudioTrack>();
		newTrack->name = name;
		newTrack->filePath = filePath;
		newTrack->ID = newID;

		createdAudioTracks[newID] = move(newTrack);

		Logger::Print(
			"Imported audio file '" + name + "' with ID '" + to_string(newID) + "'!",
			"AUDIO",
			LogType::LOG_SUCCESS);

		return createdAudioTracks[newID].get();
	}

	void AudioTrack::SetName(const string& newName)
	{
		ma_sound* sound = CommonChecker(
			"set audio track name to '" + name + "'",
			ID);

		if (sound == nullptr) return;

		string oldName = name;

		auto PrintNameError = [&oldName, &newName](const string& reason)
			{
				PrintErrorMessage(
					"Cannot change audio track old name '" + oldName + "' to new name '" + newName + "'"
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

		for (const auto& [_, track] : createdAudioTracks)
		{
			if (track->GetName() == newName)
			{
				PrintNameError("an audio track with the new name already exists!");

				return;
			}
		}

		name = newName;

		Logger::Print(
			"Changed audio track old name '" + oldName + "' to new name '" + name + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}

	void AudioTrack::Play() const
	{
		ma_sound* sound = CommonChecker(
			"play audio track '" + name + "'",
			ID);

		if (sound == nullptr) return;

		//always move to start
		ma_sound_seek_to_pcm_frame(sound, 0);

		//start playing sound
		ma_sound_start(sound);

		Logger::Print(
			"Started playing audio track '" + name + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	bool AudioTrack::IsPlaying() const
	{
		ma_sound* sound = CommonChecker(
			"get play state for audio track '" + name + "'",
			ID);

		if (sound == nullptr) return false;

		return (ma_sound_is_playing(sound) == MA_TRUE);
	}

	void AudioTrack::SetPlaybackPosition(u32 newPosition) const
	{
		ma_sound* sound = CommonChecker(
			"set playback position for audio track '" + name + "'",
			ID);

		if (sound == nullptr) return;

		if (newPosition > GetPlaybackPosition(true))
		{
			PrintErrorMessage(
				"Playback position '" + to_string(newPosition) + "' is past audio track '" + name + "' duration!");

			return;
		}

		ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);

		ma_uint64 frames = (ma_uint64)newPosition * (ma_uint64)sampleRate;

		ma_sound_seek_to_pcm_frame(sound, frames);

		Logger::Print(
			"Set playback position for audio track '" + name + "' to '" + to_string(newPosition) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	u32 AudioTrack::GetPlaybackPosition(bool getFullDuration) const
	{
		ma_sound* sound = CommonChecker(
			"get playback position for audio track '" + name + "'",
			ID);

		if (sound == nullptr) return 0;

		ma_uint64 length{};

		//get full audio track duration
		if (getFullDuration) ma_sound_get_length_in_pcm_frames(sound, &length);

		//get duration up to current frame in audio track
		else ma_sound_get_cursor_in_pcm_frames(sound, &length);

		ma_uint32 sampleRate = ma_engine_get_sample_rate(&engine);

		f64 seconds = (f64)length / (f64)sampleRate;
		return u32(seconds + 0.5);
	}

	void AudioTrack::Pause() const
	{
		ma_sound* sound = CommonChecker(
			"pause audio track '" + name + "'",
			ID);

		if (sound == nullptr) return;

		//pause song without resetting back to beginning
		ma_sound_stop(sound);

		Logger::Print(
			"Paused audio track '" + name + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	void AudioTrack::Continue() const
	{
		ma_sound* sound = CommonChecker(
			"continue audio track '" + name + "'",
			ID);

		if (sound == nullptr) return;

		//start playing sound but continue where we last left off
		ma_sound_start(sound);

		Logger::Print(
			"Continuing playing audio track '" + name + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}

	void AudioTrack::SetLoopState(bool newState) const
	{
		ma_sound* sound = CommonChecker(
			"set loop state for audio track '" + name + "'",
			ID);

		if (sound == nullptr) return;

		ma_sound_set_looping(sound, newState);

		string state = newState ? "true" : "false";
		Logger::Print(
			"Set audio track '" + name + "' loop state to '" + state + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	bool AudioTrack::CanLoop() const
	{
		ma_sound* sound = CommonChecker(
			"get loop state for audio track '" + name + "'",
			ID);

		if (sound == nullptr) return false;

		return (ma_sound_is_looping(sound) == MA_TRUE);
	}

	void AudioTrack::Stop() const
	{
		ma_sound* sound = CommonChecker(
			"stop audio track '" + name + "'",
			ID);

		if (sound == nullptr) return;

		ma_sound_stop(sound);

		//reset the audio position to the start, so the next play starts fresh
		ma_sound_seek_to_pcm_frame(sound, 0);

		Logger::Print(
			"Stopped playing audio track '" + name + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	bool AudioTrack::HasFinished() const
	{
		ma_sound* sound = CommonChecker(
			"check if audio track '" + name + "' has finished",
			ID);

		if (sound == nullptr) return false;

		return (
			ma_sound_is_playing(sound) == MA_FALSE
			&& !isPaused);
	}

	void AudioTrack::SetVolume(f32 newVolume) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' volume",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newVolume, 0.0f, 5.0f);

		ma_sound_set_volume(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' volume to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetVolume() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' volume",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_volume(sound);
	}

	void AudioTrack::SetMinGain(f32 newMinGain) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' min gain",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMinGain, 0.0f, GetMaxGain() - 0.1f);

		ma_sound_set_min_gain(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' min gain to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetMinGain() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' min gain",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_min_gain(sound);
	}

	void AudioTrack::SetMaxGain(f32 newMaxGain) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' max gain",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMaxGain, GetMinGain() + 0.1f, 5.0f);

		ma_sound_set_max_gain(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' max gain to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetMaxGain() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' max gain",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_max_gain(sound);
	}

	void AudioTrack::SetMinRange(f32 newMinRange) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' min range",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMinRange, 0.0f, GetMaxRange() - 0.1f);

		ma_sound_set_min_distance(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' min range to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetMinRange() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' min range",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_min_distance(sound);
	}

	void AudioTrack::SetMaxRange(f32 newMaxRange) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' max range",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newMaxRange, GetMinRange() + 0.1f, 1000.0f);

		ma_sound_set_max_distance(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' max range to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetMaxRange() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' max range",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_max_distance(sound);
	}

	void AudioTrack::SetSpatializationState(bool state) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' spatialization state",
			ID);

		if (sound == nullptr) return;

		if (state)
		{
			Logger::Print(
				"Set audio track '" + name + "' spatialization state to 'true'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			ma_sound_set_spatialization_enabled(sound, true);
		}
		else
		{
			Logger::Print(
				"Set audio track '" + name + "' spatialization state to 'false'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			ma_sound_set_spatialization_enabled(sound, false);
		}
	}
	bool AudioTrack::GetSpatializationState() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' spatialization state",
			ID);

		if (sound == nullptr) return false;

		return ma_sound_is_spatialization_enabled(sound);
	}

	void AudioTrack::SetPositioningState(Positioning pos) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' positioning state",
			ID);

		if (sound == nullptr) return;

		switch (pos)
		{
		case Positioning::Positioning_Relative:

			Logger::Print(
				"Set audio track '" + name + "' positioning state to 'relative'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			ma_sound_set_positioning(sound, ma_positioning_relative);
			break;
		
		case Positioning::Positioning_Absolute:

			Logger::Print(
				"Set audio track '" + name + "' positioning state to 'absolute'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			ma_sound_set_positioning(sound, ma_positioning_absolute);
			break;
		}
	}
	Positioning AudioTrack::GetPositioningState() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' positioning state",
			ID);

		if (sound == nullptr) return Positioning::Positioning_Relative;

		ma_positioning state = ma_sound_get_positioning(sound);
		return state == ma_positioning_relative
			? Positioning::Positioning_Relative
			: Positioning::Positioning_Absolute;
	}

	void AudioTrack::SetPitch(f32 newPitch) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' pitch",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newPitch, 0.0f, 5.0f);

		ma_sound_set_pitch(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' pitch to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetPitch() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' pitch",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_pitch(sound);
	}

	void AudioTrack::SetPan(f32 newPan) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' pan",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newPan, -1.0f, 1.0f);

		ma_sound_set_pan(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' pan to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetPan() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' pan",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_pan(sound);
	}

	void AudioTrack::SetPanMode(PanMode mode) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' pan mode",
			ID);

		if (sound == nullptr) return;

		switch (mode)
		{
		case PanMode::PanMode_Balance:
			ma_sound_set_pan_mode(sound, ma_pan_mode_balance);

			Logger::Print(
				"Set audio track '" + name + "' pan mode to 'balance'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			break;
		case PanMode::PanMode_Pan:
			ma_sound_set_pan_mode(sound, ma_pan_mode_pan);

			Logger::Print(
				"Set audio track '" + name + "' pan mode to 'pan'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			break;
		}
	}
	PanMode AudioTrack::GetPanMode() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' pan mode",
			ID);

		if (sound == nullptr) return PanMode::PanMode_Balance;

		ma_pan_mode mode = ma_sound_get_pan_mode(sound);
		return mode == ma_pan_mode_balance 
			? PanMode::PanMode_Balance 
			: PanMode::PanMode_Pan;
	}

	void AudioTrack::SetPlayerPosition(const vec3& pos) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' player position",
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

		Logger::Print(
			"Set audio track '" + name + "' position to '"
			+ to_string(pos.x) + ", " + to_string(pos.y) + ", " + to_string(pos.z) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	vec3 AudioTrack::GetPlayerPosition() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' player position",
			ID);

		if (sound == nullptr) return vec3(0);

		ma_vec3f pos = ma_sound_get_position(sound);

		return vec3(pos.x, pos.y, pos.z);
	}

	void AudioTrack::SetPlayerVelocity(const vec3& vel) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' player velocity",
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

		Logger::Print(
			"Set audio track '" + name + "' velocity to '" 
			+ to_string(vel.x) + ", " + to_string(vel.y) + ", " + to_string(vel.z) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	vec3 AudioTrack::GetPlayerVelocity() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' player velocity",
			ID);

		if (sound == nullptr) return vec3(0);

		ma_vec3f pos = ma_sound_get_velocity(sound);

		return vec3(pos.x, pos.y, pos.z);
	}
	
	void AudioTrack::SetDirectionalData(const AudioPlayerCone& cone) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' player directional data",
			ID);

		if (sound == nullptr) return;

#ifdef _DEBUG
		CheckHugeValue(cone.front.x, "player cone x direction");
		CheckHugeValue(cone.front.y, "player cone y direction");
		CheckHugeValue(cone.front.z, "player cone z direction");
#endif

		f32 innerAngleClamped = clamp(cone.innerConeAngle, 0.0f, 359.99f);
		f32 outerAngleClamped = clamp(cone.outerConeAngle, 0.0f, 359.99f);
		f32 outerGainClamped = clamp(cone.outerGain, 0.0f, 1.0f);

		if (outerAngleClamped < innerAngleClamped) outerAngleClamped = innerAngleClamped;

		ma_sound_set_direction(
			sound,
			cone.front.x,
			cone.front.y,
			cone.front.z);

		ma_sound_set_cone(
			sound,
			innerAngleClamped,
			outerAngleClamped,
			outerGainClamped);

		Logger::Print(
			"Set audio player cone direction to '"
			+ to_string(cone.front.x) + ", " + to_string(cone.front.y) + ", " + to_string(cone.front.z) + "', "
			+ "cone inner angle to '" + to_string(innerAngleClamped) + "', "
			+ "cone outer angle to '" + to_string(outerAngleClamped)
			+ "' and cone outer gain to '" + to_string(outerGainClamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	AudioPlayerCone AudioTrack::GetDirectionalData() const
	{
		AudioPlayerCone cone{};

		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' player directional data",
			ID);

		if (sound == nullptr) return cone;

		ma_vec3f front = ma_sound_get_direction(sound);

		f32 innerConeAngle{};
		f32 outerConeAngle{};
		f32 outerGain{};

		ma_sound_get_cone(
			sound,
			&innerConeAngle,
			&outerConeAngle,
			&outerGain);

		cone.front = vec3(front.x, front.y, front.z);
		cone.innerConeAngle = innerConeAngle;
		cone.outerConeAngle = outerConeAngle;
		cone.outerGain = outerGain;

		return cone;
	}

	void AudioTrack::SetAttenuationModel(AttenuationModel model) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' attenuation model",
			ID);

		if (sound == nullptr) return;

		switch (model)
		{
		case AttenuationModel::Attenuation_None:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_none);

			Logger::Print(
				"Set audio track '" + name + "' attenuation mode to 'none'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			break;
		case AttenuationModel::Attenuation_Inverse:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_inverse);

			Logger::Print(
				"Set audio track '" + name + "' attenuation mode to 'inverse'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			break;
		case AttenuationModel::Attenuation_Linear:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_linear);

			Logger::Print(
				"Set audio track '" + name + "' attenuation mode to 'linear'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			break;
		case AttenuationModel::Attenuation_Exponential:
			ma_sound_set_attenuation_model(sound, ma_attenuation_model_exponential);

			Logger::Print(
				"Set audio track '" + name + "' attenuation mode to 'exponential'!",
				"AUDIO",
				LogType::LOG_DEBUG);

			break;
		}
	}
	AttenuationModel AudioTrack::GetAttenuationModel() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' attenuation model",
			ID);

		if (sound == nullptr) return AttenuationModel::Attenuation_None;

		ma_attenuation_model model = ma_sound_get_attenuation_model(sound);

		if (model == ma_attenuation_model_inverse)     return AttenuationModel::Attenuation_Inverse;
		else if (model == ma_attenuation_model_linear)      return AttenuationModel::Attenuation_Linear;
		else if (model == ma_attenuation_model_exponential) return AttenuationModel::Attenuation_Exponential;

		return AttenuationModel::Attenuation_None;
	}

	void AudioTrack::SetRolloff(f32 newRolloffFactor) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' rolloff factor",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newRolloffFactor, 0.0f, 5.0f);

		ma_sound_set_rolloff(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' rolloff factor to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetRolloff() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' rolloff factor",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_rolloff(sound);
	}

	void AudioTrack::SetDopplerFactor(f32 newFactor) const
	{
		ma_sound* sound = CommonChecker(
			"set audio track '" + name + "' doppler factor",
			ID);

		if (sound == nullptr) return;

		f32 clamped = clamp(newFactor, 0.0f, 5.0f);

		ma_sound_set_doppler_factor(sound, clamped);

		Logger::Print(
			"Set audio track '" + name + "' doppler factor to '" + to_string(clamped) + "'!",
			"AUDIO",
			LogType::LOG_DEBUG);
	}
	f32 AudioTrack::GetDopplerFactor() const
	{
		ma_sound* sound = CommonChecker(
			"get audio track '" + name + "' doppler factor",
			ID);

		if (sound == nullptr) return 0;

		return ma_sound_get_doppler_factor(sound);
	}

	AudioTrack::~AudioTrack()
	{
		auto it = soundMap.find(ID);
		if (it != soundMap.end())
		{
			ma_sound_uninit(it->second.get());
			soundMap.erase(it);

			Logger::Print(
				"Destroyed audio track '" + name + "'!",
				"AUDIO",
				LogType::LOG_SUCCESS);
		}
	}
}

ma_sound* CommonChecker(
	const string& message,
	u32 ID)
{
	if (!CheckInitState(message)) return nullptr;

	auto it = soundMap.find(ID);
	if (it != soundMap.end()) return it->second.get();

	PrintErrorMessage(
		"Cannot " + message + " because the audio pointer ID '" + to_string(ID) + "' was not found in internal MiniAudio sound map!");

	return nullptr;
}
bool CheckInitState(const string& targetAction)
{
	if (!Audio::IsInitialized())
	{
		PrintErrorMessage("Cannot " + targetAction + " because MiniAudio is not initialized!");

		return false;
	}

	return true;
}

void PrintErrorMessage(const string& message)
{
	Logger::Print(
		message,
		"AUDIO",
		LogType::LOG_ERROR,
		2);
}

void CheckHugeValue(f32 value, const string& valueName)
{
	if (value < -10000.0f
		|| value > 10000.0f)
	{
		PrintWarningMessage(
			"Value '" + valueName + "' is outside the normal range [-10000, 10000]! Consider validating it.");
	}
}
void PrintWarningMessage(const string& message)
{
	Logger::Print(
		message,
		"AUDIO",
		LogType::LOG_WARNING,
		2);
}