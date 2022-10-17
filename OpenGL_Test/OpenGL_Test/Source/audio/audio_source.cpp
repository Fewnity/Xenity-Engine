//#include "audio_source.h"
#include "audio.h"


void AudioSource::Play2DSound(AudioClip audioClip)
{
	Audio::engine->play2D(audioClip.GetFilePath().c_str());
}

void AudioSource::Play3DSound(AudioClip audioClip, Vector3 position)
{
	Audio::engine->play3D(audioClip.GetFilePath().c_str(), vec3df(position.x, position.y, position.z));
}