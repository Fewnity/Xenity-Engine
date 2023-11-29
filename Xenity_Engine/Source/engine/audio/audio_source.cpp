#include "audio_source.h"
#include "audio_manager.h"

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>

#include <engine/game_elements/transform.h>
#include <engine/vectors/vector2.h>
#include <engine/graphics/color/color.h>

#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>


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

void AudioSource::Play()
{
	if (audioClip != nullptr)
	{
		isPlaying = true;
		AudioManager::PlayAudioSource(GetThisShared());
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
	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[Icon_Audio_Source], Color::CreateFromRGBFloat(1, 1, 1));
#endif
}