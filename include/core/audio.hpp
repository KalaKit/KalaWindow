//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>
#include <algorithm>

#include "KalaHeaders/core_utils.hpp"

#include "glm_global.hpp"

//min is 1MB
static constexpr u64 MIN_STREAM_SIZE = static_cast<size_t>(1 * 1024) * 1024; 
//max is 20MB
static constexpr u64 MAX_STREAM_SIZE = static_cast<size_t>(20 * 1024) * 1024;

namespace KalaWindow::Core
{
	using std::string;
	using std::filesystem::exists;
	using std::filesystem::path;
	using std::filesystem::is_regular_file;
	using std::clamp;

	//Stores data for managing directional audio hearing/playback
	struct AudioCone
	{
		//Start of cone, full volume
		f32 innerConeAngle{};
		//End of cone, minimum volume
		f32 outerConeAngle{};
		//Sound fades out to this level at the end of the cone
		f32 outerGain{};
	};

	enum class SampleRate
	{
		SAMPLE_DEFAULT, //Automatically picks device-native for best compatibility
		SAMPLE_48000,   //Modern default
		SAMPLE_44100    //Good for rare cases where 44.1KHz is needed like old CDs and older music standards
	};

	//The formula or curve shape used to reduce volume over distance.
	enum class AttenuationModel
	{
		Attenuation_None,       //No volume change with distance (UI/music)
		Attenuation_Inverse,    //Realistic 1/distance falloff
		Attenuation_Linear,     //Straight line between min and max distance
		Attenuation_Exponential //Sharp exponential fade, very dramatic cutoff
	};

	//Controls how left/right panning is interpreted via SetPan
	enum class PanMode
	{
		PanMode_Balance, //Default "stereo" balance
		PanMode_Pan      //Maintains constant perceived loudness when moving between channels
	};

	//Controls how a sound's position is interpreted when spatialization is enabled
	enum class Positioning
	{
		//Position is relative to the listener
		Positioning_Relative,

		//Position is absolute in world space
		Positioning_Absolute
	};

	//
	// AUDIO CORE
	//

	class LIB_API Audio
	{
	public:
		//Initialize Miniaudio.
		//Listener count is internally clamped from 1 to 4.
		static bool Initialize(
			u32 listeners = 1,
			SampleRate sampleRate = SampleRate::SAMPLE_DEFAULT);
		static inline bool IsInitialized() { return isInitialized; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//SetPlayerPosition, SetListenerPosition will dump their debug logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isVerboseLoggingEnabled; }

		//Set threshold where audio files will be streamed instead of loaded to memory in full.
		//Only affects newly imported audio files.
		static inline void SetStreamThreshold(u64 newThreshold)
		{
			streamThreshold = clamp(newThreshold, MIN_STREAM_SIZE, MAX_STREAM_SIZE);
		};
		static inline u64 GetStreamThreshold() { return streamThreshold; }

		//Shut down Miniaudio
		static void Shutdown();
	private:
		static inline bool isInitialized;
		static inline bool isVerboseLoggingEnabled;

		static inline u64 streamThreshold = static_cast<size_t>(5 * 1024) * 1024; //default is 5MB
	};

	//
	// EACH INDIVIDUAL AUDIO LISTENER
	//

	class LIB_API AudioListener
	{
	public:
		//Set audio listener mute state
		static void SetMuteState(
			bool state,
			u32 ID = 0);
		static bool IsMuted(u32 ID = 0);

		//Set audio listener up direction
		static void SetWorldUp(
			const vec3& up,
			u32 ID = 0);
		static vec3 GetWorldUp(u32 ID = 0);

		//Set audio listener position
		static void SetPosition(
			const vec3& pos,
			u32 ID = 0);
		static vec3 GetPosition(u32 ID = 0);

		//Set audio listener velocity
		static void SetVelocity(
			const vec3& vel,
			u32 ID = 0);
		static vec3 GetVelocity(u32 ID = 0);

		//Set audio listener direction
		static void SetDirection(
			const vec3& pos,
			u32 ID = 0);
		static vec3 GetDirection(u32 ID = 0);

		//Set audio listener cone values.
		//Inner cone angle and outer cone angle are internally clamped from 0.0f to 359.99f.
		//Outer gain is internally clamped from 0.0f to 1.0f;
		static void SetConeData(
			const AudioCone& cone,
			u32 ID = 0);
		static AudioCone GetConeData(u32 ID = 0);
	};

	//
	// EACH INDIVIDUAL CREATED AUDIO PLAYER
	//

	class LIB_API AudioPlayer
	{
	public:
		//Create a new audio player. If file size is less than or equal to 10MB
		//then file is loaded into memory in full, otherwise it is streamed.
		static AudioPlayer* CreateAudioPlayer(
			const string& name,
			const string& filePath);

		//Assign a new name to this audio player
		void SetName(const string& newName);
		inline const string& GetName() const { return name; }

		inline const string& GetPath() const { return filePath; }

		inline u32 GetID() const { return ID; }

		//Start playing this audio player from the start
		void Play() const;
		bool IsPlaying() const;

		//Set the playback position of this audio player in seconds from the start
		void SetPlaybackPosition(u32 newValue) const;
		//Get either length played or total audio player length in seconds
		u32 GetPlaybackPosition(bool getFullDuration) const;

		//Pause this playing audio player
		void Pause() const;
		//Continue playing this paused audio player
		void Continue() const;
		inline bool IsPaused() const { return isPaused; };

		//Set the loop state of this audio player. If true, then this audio player
		//starts again from the beginning after it finishes playing.
		void SetLoopState(bool newState) const;
		bool CanLoop() const;

		//Stop this playing audio player. If loop is enabled then this audio player starts playing again from the beginning.
		void Stop() const;
		//Returns true if this audio player is not playing and is not paused
		bool HasFinished() const;

		//Set the volume of this audio player.
		//Clamped internally from 0.0f to 5.0f, but recommended up to 1.0
		void SetVolume(f32 newVolume) const;
		f32 GetVolume() const;

		//Toggle whether this sound is affected by spatial audio effects or not
		void SetSpatializationState(bool newState) const;
		bool GetSpatializationState() const;

		//Controls how a sound's position is interpreted when spatialization is enabled
		void SetPositioningState(Positioning pos) const;
		Positioning GetPositioningState() const;

		//Set the pitch of this audio player.
		//Clamped internally from 0.0f to 5.0f, but recommended up to 1.0
		void SetPitch(f32 newPitch) const;
		f32 GetPitch() const;

		//Controls how left/right panning is interpreted via SetPan
		void SetPanMode(PanMode panMode) const;
		PanMode GetPanMode() const;

		//Balance audio between left and right speakers.
		//Clamped internally from -1.0f to 1.0f
		void SetPan(f32 pan) const;
		f32 GetPan() const;

		//Set audio playback position
		void SetPosition(const vec3& pos) const;
		vec3 GetPosition() const;

		//Set audio playback velocity
		void SetVelocity(const vec3& vel) const;
		vec3 GetVelocity() const;

		//Set audio player direction
		void SetDirection(const vec3& pos) const;
		vec3 GetDirection() const;

		//Set audio player cone values.
		//Inner cone angle and outer cone angle are internally clamped from 0.0f to 359.99f.
		//Outer gain is internally clamped from 0.0f to 1.0f;
		void SetConeData(const AudioCone& cone) const;
		AudioCone GetConeData() const;

		//The formula or curve shape used to reduce volume over distance
		void SetAttenuationModel(AttenuationModel model) const;
		AttenuationModel GetAttenuationModel() const;

		//Scaling multiplier applied on top of the attenuation model.
		//Controls how aggressively the chosen attenuation curve reduces volume.
		//Clamped internally from 0.0f to 5.0f, but recommended up to 1.0
		void SetRolloff(f32 newRolloffFactor) const;
		f32 GetRolloff() const;

		//Scales how dramatic the Doppler effect is when either listener or source is moving.
		//Clamped internally from 0.0f to 5.0f, but recommended up to 1.0
		void SetDopplerFactor(f32 factor) const;
		f32 GetDopplerFactor() const;

		//Set the minimum final volume that this audio player can drop to, 
		//even after attenuation. Clamped internally from 0.0f to MaxGain - 0.1f
		void SetMinGain(f32 newMinGain) const;
		f32 GetMinGain() const;

		//Set the maximum final volume that this audio player can rise to, 
		//even after boosts. Clamped internally from MinGain + 0.1f to 5.0f, but recommended up to 1.0
		void SetMaxGain(f32 newMaxGain) const;
		f32 GetMaxGain() const;

		//Set the minimum distance at which this audio player is heard at full volume.
		//Clamped internally from 0.0f to MaxRange - 0.1f
		void SetMinRange(f32 newMinRange) const;
		f32 GetMinRange() const;

		//Set the maximum distance at which this audio player can be heard before it is silent.
		//Clamped internally from MinRange + 0.1f to 1000.0f
		void SetMaxRange(f32 newMaxRange) const;
		f32 GetMaxRange() const;

		//Do not destroy manually, erase from containers.hpp instead
		~AudioPlayer();
	private:
		string name{};
		string filePath{};
		u32 ID{};

		bool isPaused{};
	};
}