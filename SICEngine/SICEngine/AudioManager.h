/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Dylan Bier
 */
 

#pragma once

#include <string>
#include "fmod.hpp"
#include "SICTypes.h"
#include "unordered_map"
#include "StringId.h"

namespace sic
{
	class ResourceManager;

	enum class AudioType : int
	{
		EFFECT = 0,
		MUSIC = 1,
		AMBIENT = 2,
		FOOTSTEPS = 3,
		MENU = 4

	};

	class AudioManager
	{
	public:
		using allocator_type = std::pmr::polymorphic_allocator<>;

		AudioManager();
		explicit AudioManager(allocator_type alloc);
		AudioManager(AudioManager const& src, allocator_type alloc = {}) = delete;
		AudioManager(AudioManager const&) = delete;
		AudioManager& operator=(AudioManager const&) = delete;
		AudioManager(AudioManager&&) = delete;
		AudioManager& operator=(AudioManager&&) = delete;
		~AudioManager();

	public:
		void PlayOnce(FMOD::Sound* sound, AudioType group);
		void Play(FMOD::Sound* sound, AudioType group);
		void PauseSound(FMOD::Sound* sound, bool pause);
		void StopSound(FMOD::Sound* sound);
		void Update();

		void LoopSound(const char* name, AudioType group);
		void LoopSound(sic::StringId::underlying_type hash, AudioType group);

		void PlaySound(const char* name, AudioType group);
		void PlaySound(sic::StringId::underlying_type hash, AudioType group);

		// Channel Controls
		void PauseChannel(AudioType group);
		void ResumeChannel(AudioType group);
		void SetChannelVolume(AudioType group, f32 volume);
		bool IsChannelPaused(AudioType group);
		void ToggleChannelMute(AudioType group);

		void ToggleMuteAll(void);

		FMOD::Sound* LoadSound(const char* path);
		FMOD::System& SoundSystem();


	private:
		FMOD::System* m_sound_system;
		FMOD::ChannelGroup* m_channel_groups[5];
		std::pmr::unordered_map<StringId::underlying_type, FMOD::Channel*> m_channels;

		bool muted = false;
	};
}