/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: - Andrew Rudasics
 * Additional Authors: - Dylan Bier
 */
 

#include "AudioManager.h"

#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "fmod_errors.h"
#include "ResourceManager.h"
#include "System.h"

namespace sic
{
	AudioManager::AudioManager() : AudioManager(allocator_type())
	{

	}

	AudioManager::AudioManager(allocator_type alloc) : m_channels(alloc)
	{
		FMOD_RESULT result;
		
		result = FMOD::System_Create(&m_sound_system);
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}

		result = m_sound_system->init(512, FMOD_INIT_NORMAL, 0);
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}

		result = m_sound_system->createChannelGroup("Effects", &m_channel_groups[static_cast<int>(AudioType::EFFECT)]);
		assert(result == FMOD_OK);
		
		result = m_sound_system->createChannelGroup("Music", &m_channel_groups[static_cast<int>(AudioType::MUSIC)]);
		assert(result == FMOD_OK);

		result = m_sound_system->createChannelGroup("Ambient", &m_channel_groups[static_cast<int>(AudioType::AMBIENT)]);
		assert(result == FMOD_OK);

		result = m_sound_system->createChannelGroup("Footsteps", &m_channel_groups[static_cast<int>(AudioType::FOOTSTEPS)]);
		assert(result == FMOD_OK);

		result = m_sound_system->createChannelGroup("Menu", &m_channel_groups[static_cast<int>(AudioType::MENU)]);
		assert(result == FMOD_OK);
	}

	AudioManager::~AudioManager()
	{
		m_sound_system->release();
	}

	void AudioManager::Play(FMOD::Sound* sound, AudioType group)
	{
		FMOD_RESULT result;

		FMOD::Channel* channel;
		result = m_sound_system->playSound(sound, m_channel_groups[static_cast<int>(group)], false, &channel);

		char name[256];
		result = sound->getName(name, 256);
		assert(result == FMOD_OK);

		auto name_hash = StrHash(name);
		m_channels.insert_or_assign(name_hash, channel);
	}

	void AudioManager::PlayOnce(FMOD::Sound* sound, AudioType group)
	{
		FMOD_RESULT result;

		result = m_sound_system->playSound(sound, m_channel_groups[static_cast<int>(group)], false, 0);
		assert(result == FMOD_OK);
	}

	void AudioManager::PauseSound(FMOD::Sound* sound, bool pause)
	{
		FMOD_RESULT result;
		char name[256];
		result = sound->getName(name, 256);
		assert(result == FMOD_OK);

		FMOD::Channel* channel;

		auto name_hash = StrHash(name);
		channel = m_channels[name_hash];

		if (channel != NULL)
		{
			result = channel->setPaused(pause);
		}		
	}

	void AudioManager::StopSound(FMOD::Sound* sound)
	{
		FMOD_RESULT result;
		char name[256];
		result = sound->getName(name, 256);
		assert(result == FMOD_OK);

		FMOD::Channel* channel;

		auto name_hash = StrHash(name);
		channel = m_channels[name_hash];

		if (channel != NULL)
		{
			result = channel->stop();
		}
	}

#pragma region Audio Controls

	void AudioManager::LoopSound(const char* name, AudioType group)
	{
		
		u64 file = StrHash(name);
		FMOD::Sound* sound = System::Resources().GetSound(file);
		if (sound == NULL)
		{
			sound = LoadSound(name);
		}

		FMOD_RESULT result = sound->setLoopCount(-1);
		assert(result == FMOD_OK);

		FMOD::Channel* channel;
		result = m_sound_system->playSound(sound, m_channel_groups[static_cast<int>(group)], false, &channel);

		channel->setMode(FMOD_LOOP_NORMAL);
		channel->setPosition(0, FMOD_TIMEUNIT_PCM);
	}

	void AudioManager::LoopSound(StringId::underlying_type hash, AudioType group)
	{
		FMOD::Sound* sound = System::Resources().GetSound(hash);
		if (sound != NULL)
		{
			FMOD_RESULT result = sound->setLoopCount(-1);
			assert(result == FMOD_OK);

			FMOD::Channel* channel;
			result = m_sound_system->playSound(sound, m_channel_groups[static_cast<int>(group)], false, &channel);

			channel->setMode(FMOD_LOOP_NORMAL);
			channel->setPosition(0, FMOD_TIMEUNIT_PCM);
		}
	}

	void AudioManager::PlaySound(const char* name, AudioType group)
	{
		u64 file = StrHash(name);
		FMOD::Sound* sound = System::Resources().GetSound(file);
		if (sound == NULL)
		{
			sound = LoadSound(name);
		}

		FMOD_RESULT result = sound->setLoopCount(0);
		assert(result == FMOD_OK);

		FMOD::Channel* channel;
		result = m_sound_system->playSound(sound, m_channel_groups[static_cast<int>(group)], false, &channel);

		channel->setMode(FMOD_LOOP_OFF);
		channel->setPosition(0, FMOD_TIMEUNIT_PCM);
	}

	void AudioManager::PlaySound(StringId::underlying_type hash, AudioType group)
	{
		FMOD::Sound* sound = System::Resources().GetSound(hash);
		if (sound != NULL)
		{
			FMOD_RESULT result = sound->setLoopCount(0);
			assert(result == FMOD_OK);

			FMOD::Channel* channel;
			result = m_sound_system->playSound(sound, m_channel_groups[static_cast<int>(group)], false, &channel);

			channel->setMode(FMOD_LOOP_OFF);
			channel->setPosition(0, FMOD_TIMEUNIT_PCM);
		}
	}


	void AudioManager::ToggleMuteAll(void)
	{
		if (!muted)
		{
			PauseChannel(AudioType::AMBIENT);
			PauseChannel(AudioType::EFFECT);
			PauseChannel(AudioType::MUSIC);
			PauseChannel(AudioType::FOOTSTEPS);
			PauseChannel(AudioType::MENU);
		}
		else
		{
			ResumeChannel(AudioType::AMBIENT);
			ResumeChannel(AudioType::EFFECT);
			ResumeChannel(AudioType::MUSIC);
			ResumeChannel(AudioType::FOOTSTEPS);
			ResumeChannel(AudioType::MENU);
		}
		muted = !muted;
	}

#pragma endregion



#pragma region Channel Controls

	void AudioManager::PauseChannel(AudioType group)
	{
		if (m_channel_groups[static_cast<int>(group)] != NULL)
		{
			m_channel_groups[static_cast<int>(group)]->setPaused(true);
		}
	}

	void AudioManager::ResumeChannel(AudioType group)
	{
		if (m_channel_groups[static_cast<int>(group)] != NULL)
		{
			m_channel_groups[static_cast<int>(group)]->setPaused(false);
		}
	}

	void AudioManager::SetChannelVolume(AudioType group, f32 volume)
	{
		if (m_channel_groups[static_cast<int>(group)] != NULL)
		{
			m_channel_groups[static_cast<int>(group)]->setVolume(volume);
		}
	}

	bool AudioManager::IsChannelPaused(AudioType group)
	{
		if (static_cast<int>(group) < 0 || static_cast<int>(group) >= 5)
		{
			return true;
		}

		bool playing;

		playing = false;

		if (m_channel_groups[static_cast<int>(group)] != NULL)
		{
			m_channel_groups[static_cast<int>(group)]->getMute(&playing);//->isPlaying(&playing);
		}

		return !playing;
	}

	void AudioManager::ToggleChannelMute(AudioType group)
	{
		bool muted = false;

		if (m_channel_groups[static_cast<int>(group)] != NULL)
		{
			m_channel_groups[static_cast<int>(group)]->getMute(&muted);
			m_channel_groups[static_cast<int>(group)]->setMute(!muted);
			
		}
	}

#pragma endregion


	

	void AudioManager::Update()
	{
		m_sound_system->update();
	}

	FMOD::Sound* AudioManager::LoadSound(const char* filepath)
	{
		FMOD::Sound* sound = System::Resources().LoadAudioFile(*m_sound_system, filepath);	
		return sound;	
	}

	FMOD::System& AudioManager::SoundSystem()
	{
		return *m_sound_system;
	}
}