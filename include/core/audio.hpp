//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <algorithm>
#include <filesystem>

#include "core/platform.hpp"
#include "core/containers.hpp"

namespace KalaWindow::Core
{
	using std::string;
	using std::clamp;
	using std::filesystem::exists;
	using std::filesystem::path;
	using std::filesystem::is_regular_file;

	//
	// AUDIO CORE
	//

	class KALAWINDOW_API Audio
	{
	public:
		//Initialize Miniaudio
		static void Initialize();

		static bool IsInitialized();

		//Checks if audio file with this name is imported
		static bool IsImported(const string& name);

		//Checks if audio file with this file path is imported
		static bool IsImported(const string& filePath);

		//Runtime function to update the global listener position every frame
		static void UpdateListenerPosition(
			const vec3& pos,
			const vec3& front,
			const vec3& up);

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
		static AudioTrack* ImportAudioTrack(const string& name, const string& filePath);

		//Assign a new name to this audio track
		void SetName(const string& newName)
		{
			if (newName == name
				|| newName.empty()
				|| newName.size() > 20)
			{
				return;
			}

			for (const auto& [_, track] : createdAudioTracks)
			{
				if (track->GetName() == newName) return;
			}

			name = newName;
		}
		const string& GetName() const { return name; }

		//Assign a new file path to this audio track.
		//Important: Successfully setting a path reimports this audio file!
		//You are responsible for getting a pointer to the new audio file, KalaWindow does not store it
		//because the old audio file is removed by the destructor.
		void SetPath(const string& newName, const string& newPath, u32 id)
		{
			if (ImportAudioTrack(newName, newPath) != nullptr)
			{
				createdAudioTracks.erase(id);
			}
		}
		const string& GetPath() const { return filePath; }

		u32 GetID() const { return ID; }

		//Start playing this audio track from the start
		void Play();
		bool IsPlaying() const { return isPlaying; }

		//Get the total duration of this audio track in seconds
		f32 GetDuration();

		//Pause this playing audio track
		void Pause();
		//Continue playing this paused audio track
		void Continue();
		bool IsPaused() const { return isPaused; }

		//Set the loop state of this audio track. If true, then this audio track
		//starts again from the beginning after it finishes playing.
		void SetLoopState(bool newState);
		bool CanLoop() const { return canLoop; }

		//Set the playback position of this audio track in seconds from the start
		void SetPlaybackPosition(f32 newValue);
		f32 GetPlaybackPosition();

		//Stop this playing audio track. If loop is enabled then this audio track starts playing again from the beginning.
		void Stop();
		bool IsFinished();

		//Set the volume of this audio track
		void SetVolume(f32 newVolume);
		f32 GetVolume() const { return volume; }

		//Set the minimum distance at which this audio track is heard at full volume.
		void SetMinRange(f32 newMinRange);
		f32 GetMinRange() const { return minRange; }

		//Set the maximum distance at which this audio track can be heard before it is silent.
		void SetMaxRange(f32 newMaxRange);
		f32 GetMaxRange() const { return maxRange; }

		//Set the pitch of this audio track.
		void SetPitch(f32 newPitch);
		f32 GetPitch() const { return pitch; }

		//Set the 3D state of this audio track. If true, then this audio track
		//plays dynamically on right or left side depending on listener and player position
		void Set3DState(bool newState);
		bool Is3D() const { return current3Dstate; }

		//Runtime function to update this audio track
		void UpdatePlayerPosition(const vec3& pos);

		~AudioTrack();
	private:
		bool isPlaying;
		bool isPaused;
		bool current3Dstate;
		bool canLoop;

		f32 volume;
		f32 minRange;
		f32 maxRange;
		f32 pitch;

		string name;
		string filePath;
		u32 ID;
	};
}