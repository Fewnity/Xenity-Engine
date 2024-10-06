// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "audio_source.h"

#include <thread>
#if defined(__PSP__)
#include <pspkernel.h>
#endif

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/vectors/vector2.h>
#include <engine/graphics/color/color.h>
#include <engine/debug/debug.h>
#include "audio_manager.h"


AudioSource::AudioSource()
{
	m_componentName = "AudioSource";
	AssetManager::AddReflection(this);
}

ReflectiveData AudioSource::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	ReflectiveEntry& volumeEntry = Reflective::AddVariable(reflectedVariables, m_volume, "volume", true);
	volumeEntry.isSlider = true;
	volumeEntry.minSliderValue = 0;
	volumeEntry.maxSliderValue = 1;
	ReflectiveEntry& panEntry = Reflective::AddVariable(reflectedVariables, m_pan, "pan", true);
	panEntry.isSlider = true;
	panEntry.minSliderValue = 0;
	panEntry.maxSliderValue = 1;
	Reflective::AddVariable(reflectedVariables, m_playOnAwake, "playOnAwake", true);
	Reflective::AddVariable(reflectedVariables, m_loop, "loop", true);
	Reflective::AddVariable(reflectedVariables, m_audioClip, "audioClip", true);
	return reflectedVariables;
}

AudioSource::~AudioSource()
{
	AssetManager::RemoveReflection(this);
}

void AudioSource::RemoveReferences()
{
	AudioManager::RemoveAudioSource(this);
}

void AudioSource::Awake()
{
	if (m_playOnAwake)
		Play();
}

void AudioSource::SetVolume(float _volume)
{
	if (_volume > 1)
		_volume = 1;
	else if (_volume < 0)
		_volume = 0;
	m_volume = _volume;
}

void AudioSource::SetPanning(float panning)
{
	if (panning > 1)
		panning = 1;
	else if (panning < 0)
		panning = 0;
	m_pan = panning;
}

void AudioSource::SetLoop(bool isLooping)
{
	m_loop = isLooping;
}

#if defined(__PSP__) || defined(__vita__)
int test(SceSize args, void* argp) 
{
	std::shared_ptr<AudioSource> audioS = *(std::shared_ptr<AudioSource>*)argp;
	AudioManager::PlayAudioSource(audioS);
	sceKernelExitDeleteThread(0);
	return 0;
}
#endif

void AudioSource::Play()
{
	if (m_audioClip != nullptr)
	{
		m_isPlaying = true;
		std::shared_ptr<AudioSource> sharedThis = GetThisShared();
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
		std::thread t(&AudioManager::PlayAudioSource, sharedThis);
		t.detach();
		//AudioManager::PlayAudioSource(GetThisShared());
#elif defined(__PSP__)
		AudioManager::PlayAudioSource(GetThisShared());
		/*SceUID thd_id2 = sceKernelCreateThread("PlayAudioSource", test, 0x18, 0x10000, 0, NULL);
		if (thd_id2 >= 0)
		{
			sceKernelStartThread(thd_id2, sizeof(std::shared_ptr<AudioSource>), &sharedThis);
		}*/
#elif defined(__vita__)
		AudioManager::PlayAudioSource(GetThisShared());
		/*SceUID thd_id2 = sceKernelCreateThread("PlayAudioSource", test, 0x40, 0x400000, 0, 0, NULL);
		if (thd_id2 >= 0)
		{
			sceKernelStartThread(thd_id2, sizeof(std::shared_ptr<AudioSource>), &sharedThis);
		}*/
#endif
	}
}

void AudioSource::Resume()
{
	if (m_audioClip != nullptr)
	{
		m_isPlaying = true;
	}
}

void AudioSource::Pause()
{
	m_isPlaying = false;
}

void AudioSource::Stop()
{
	m_isPlaying = false;
	AudioManager::StopAudioSource(GetThisShared());
}

void AudioSource::OnDrawGizmos()
{
#if defined(EDITOR)
	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[(int)IconName::Icon_Audio_Source], Color::CreateFromRGBFloat(1, 1, 1));
#endif
}