// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "audio_source.h"
#include "audio_manager.h"

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>

#include <engine/game_elements/transform.h>
#include <engine/vectors/vector2.h>
#include <engine/graphics/color/color.h>
#include <engine/debug/debug.h>

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#elif defined(__PSP__)
#include <pspkernel.h>
#endif

AudioSource::AudioSource()
{
	componentName = "AudioSource";
	AssetManager::AddReflection(this);
}

ReflectiveData AudioSource::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, volume, "volume", true);
	Reflective::AddVariable(reflectedVariables, pan, "pan", true);
	Reflective::AddVariable(reflectedVariables, playOnAwake, "playOnAwake", true);
	Reflective::AddVariable(reflectedVariables, loop, "loop", true);
	Reflective::AddVariable(reflectedVariables, audioClip, "audioClip", true);
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
	if (playOnAwake)
		Play();
}

void AudioSource::SetVolume(float _volume)
{
	if (_volume > 1)
		_volume = 1;
	else if (_volume < 0)
		_volume = 0;
	this->volume = _volume;
}

void AudioSource::SetPanning(float panning)
{
	if (panning > 1)
		panning = 1;
	else if (panning < 0)
		panning = 0;
	this->pan = panning;
}

void AudioSource::SetLoop(bool isLooping)
{
	this->loop = isLooping;
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
	if (audioClip != nullptr)
	{
		isPlaying = true;
		std::shared_ptr<AudioSource> sharedThis = GetThisShared();
#if defined(_WIN32) || defined(_WIN64)
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
	if (audioClip != nullptr)
	{
		isPlaying = true;
	}
}

void AudioSource::Pause()
{
	isPlaying = false;
}

void AudioSource::Stop()
{
	isPlaying = false;
	AudioManager::StopAudioSource(GetThisShared());
}

void AudioSource::OnDrawGizmos()
{
#if defined(EDITOR)
	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[(int)IconName::Icon_Audio_Source], Color::CreateFromRGBFloat(1, 1, 1));
#endif
}