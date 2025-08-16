//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>

#include "core/platform.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::filesystem::exists;
	using std::filesystem::path;
	using std::filesystem::is_regular_file;

	//Stores data for managing where to listen to audio from
	struct AudioListener
	{
		//Origin of audio listener
		vec3 pos{};
		//Direction at which audio listener is facing
		vec3 front{};
		//World up direction for audio listener
		vec3 up{};
	};
	//Stores data per audio track for managing directional audio playback
	struct AudioPlayerCone
	{
		//Direction of played sound. Assigning this turns this audio player from omnidirectional to directional audio player.
		//To reset back to omnidirectional simply set front to vec3(0)
		vec3 front{};
		//Start of cone, full volume
		f32 innerConeAngle{};
		//End of cone, minimum volume
		f32 outerConeAngle{};
		//Sound fades out to this level at the end of the cone
		f32 outerGain{};
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

	class KALAWINDOW_API Audio
	{
	public:
		//Initialize Miniaudio
		static void Initialize();
		static bool IsInitialized() { return isInitialized; }

		//Runtime function to update the global listener position
		static void SetListenerPosition(const AudioListener& listener);
		static AudioListener GetListenerPosition();

		//Shut down Miniaudio
		static void Shutdown();
	private:
		static inline bool isInitialized;
	};

	//
	// EACH INDIVIDUAL IMPORTED AUDIO FILE
	//

	class KALAWINDOW_API AudioTrack
	{
	public:
		//Import a new audio track
		static AudioTrack* ImportAudioTrack(
			const string& name,
			const string& filePath);

		//Assign a new name to this audio track
		void SetName(const string& newName);
		const string& GetName() const { return name; }

		const string& GetPath() const { return filePath; }

		u32 GetID() const { return ID; }

		//Start playing this audio track from the start
		void Play() const;
		bool IsPlaying() const;

		//Set the playback position of this audio track in seconds from the start
		void SetPlaybackPosition(u32 newValue) const;
		//Get either length played or total audio track length in seconds
		u32 GetPlaybackPosition(bool getFullDuration) const;

		//Pause this playing audio track
		void Pause() const;
		//Continue playing this paused audio track
		void Continue() const;
		bool IsPaused() const { return isPaused; };

		//Set the loop state of this audio track. If true, then this audio track
		//starts again from the beginning after it finishes playing.
		void SetLoopState(bool newState) const;
		bool CanLoop() const;

		//Stop this playing audio track. If loop is enabled then this audio track starts playing again from the beginning.
		void Stop() const;
		//Returns true if this audio track is not playing and is not paused
		bool HasFinished() const;

		//Set the volume of this audio track.
		//Clamped internally from 0.0f to 5.0f, but recommended up to 1.0
		void SetVolume(f32 newVolume) const;
		f32 GetVolume() const;

		//Set the minimum final volume that this audio track can drop to, 
		//even after attenuation. Clamped internally from 0.0f to MaxGain - 0.1f
		void SetMinGain(f32 newMinGain) const;
		f32 GetMinGain() const;

		//Set the maximum final volume that this audio track can rise to, 
		//even after boosts. Clamped internally from MinGain + 0.1f to 5.0f, but recommended up to 1.0
		void SetMaxGain(f32 newMaxGain) const;
		f32 GetMaxGain() const;

		//Set the minimum distance at which this audio track is heard at full volume.
		//Clamped internally from 0.0f to MaxRange - 0.1f
		void SetMinRange(f32 newMinRange) const;
		f32 GetMinRange() const;

		//Set the maximum distance at which this audio track can be heard before it is silent.
		//Clamped internally from MinRange + 0.1f to 1000.0f
		void SetMaxRange(f32 newMaxRange) const;
		f32 GetMaxRange() const;

		//Toggle whether this sound is affected by spatial audio effects or not
		void SetSpatializationState(bool newState) const;
		bool GetSpatializationState() const;

		//Controls how a sound's position is interpreted when spatialization is enabled
		void SetPositioningState(Positioning pos) const;
		Positioning GetPositioningState() const;

		//Set the pitch of this audio track.
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
		void SetPlayerPosition(const vec3& pos) const;
		vec3 GetPlayerPosition() const;

		//Set audio playback velocity
		void SetPlayerVelocity(const vec3& vel) const;
		vec3 GetPlayerVelocity() const;

		//Set audio playback direction and cone data.
		//Inner cone angle and outer cone angle are internally clamped from 0.0f to 359.99f.
		//Outer gain is internally clamped from 0.0f to 1.0f;
		void SetDirectionalData(const AudioPlayerCone& cone) const;
		AudioPlayerCone GetDirectionalData() const;

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

		//Note: Do not destroy manually, erase from containers.hpp instead
		~AudioTrack();
	private:
		string name{};
		string filePath{};
		u32 ID{};

		bool isPaused{};
	};
}