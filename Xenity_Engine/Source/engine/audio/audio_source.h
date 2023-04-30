#pragma once

#include "audio_clip.h"
#include "../vectors/vector3.h"

class AudioSource
{
public:
	static void Play2DSound(AudioClip audioClip);
	static void Play3DSound(AudioClip audioClip, Vector3 position);
};

