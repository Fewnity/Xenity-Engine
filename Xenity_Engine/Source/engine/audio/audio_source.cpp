#include "audio_source.h"
#include "audio_manager.h"
#include "../engine.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"

AudioSource::AudioSource()
{
	componentName = "AudioSource";
	AssetManager::AddReflection(this);
}

std::unordered_map<std::string, ReflectionEntry> AudioSource::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, volume, "volume", true);
	Reflection::AddReflectionVariable(reflectedVariables, pan, "pan", true);
	Reflection::AddReflectionVariable(reflectedVariables, playOnAwake, "playOnAwake", true);
	Reflection::AddReflectionVariable(reflectedVariables, loop, "loop", true);
	Reflection::AddReflectionVariable(reflectedVariables, audioClip, "audioClip", true);
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