// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "audio_manager.h"

#include <cstring>

#if defined(__PSP__)
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspkernel.h>
#include <psppower.h>
#elif defined(__vita__)
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <malloc.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <thread>
#include <chrono>
#include <windows.h>
#include <mmsystem.h>

WAVEHDR waveHdr[2];
HWAVEOUT hWaveOut;
short* audioData = nullptr;
short* audioData2 = nullptr;
int currentBuffer = 0;
#elif defined(_EE)
#include <thread>
#elif defined(__LINUX__)
#include <thread>
#endif

#include "audio_clip.h"
#include "audio_clip_stream.h"
#include <engine/engine.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/debug/debug.h>
#include <engine/assertions/assertions.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/constants.h>

bool AudioManager::s_isAdding = false;
Channel* AudioManager::s_channel;
constexpr int buffSize = 1024 * 16;
int halfBuffSize = 0;
int quarterBuffSize = 0;
MyMutex* AudioManager::s_myMutex = nullptr;

std::shared_ptr<ProfilerBenchmark> audioBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> audioBenchmark2 = nullptr;

static_assert(buffSize % 16 == 0, "buffSize must be a multiple of 16");
static_assert(AUDIO_BUFFER_SIZE % 16 == 0, "AUDIO_BUFFER_SIZE must be a multiple of 16");

short MixSoundToBuffer(short bufferValue, short soundValue)
{
	int newVal = bufferValue + soundValue;
	// Clamp value
	if (newVal > INT16_MAX)
		newVal = INT16_MAX;
	else if (newVal < -32768)
		newVal = -32768;

	return newVal;
}

void AudioManager::FillChannelBuffer(short* buffer, int length, Channel* channel)
{
	// Reset buffer
	for (int i = 0; i < length; i++)
	{
		int leftBufferIndex = i * 2;
		int rightBufferIndex = 1 + i * 2;
		buffer[leftBufferIndex] = 0;
		buffer[rightBufferIndex] = 0;
	}

	AudioManager::s_myMutex->Lock();

	// For each sound, add it's buffer to the sound buffer and change seek of audio stream
	int playedSoundsCount = channel->m_playedSoundsCount;
	for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
	{
		PlayedSound* sound = channel->m_playedSounds[soundIndex];
#if defined(EDITOR)
		if (sound->m_isPlaying && ((sound->m_audioSource.lock() && sound->m_audioSource.lock()->m_isEditor) || GameplayManager::GetGameState() == GameState::Playing))
#else
		if (sound->m_isPlaying)
#endif
		{
			AudioClipStream* stream = sound->m_audioClipStream;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
			const float leftPan = std::max<float>(0.0f, std::min<float>(0.5f, 1 - sound->m_pan)) * 2;
			const float rightPan = std::max<float>(0.0f, std::min<float>(0.5f, sound->m_pan)) * 2;
#else
			const float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->m_pan)) * 2;
			const float rightPan = std::max(0.0f, std::min(0.5f, sound->m_pan)) * 2;
#endif
			const float leftVolume = sound->m_volume * leftPan;
			const float rightVolume = sound->m_volume * rightPan;
			const int channelCount = stream->GetChannelCount();
			short* rightBuf = nullptr;
			short* leftBuf = nullptr;
			const uint32_t frequency = stream->GetFrequency();
			const uint64_t sampleCount = stream->GetSampleCount();
			int leftBufferIndex = 0;
			int rightBufferIndex = 0;
			const short* soundBuffer = sound->m_buffer;

			bool deleteAudio = false;
			for (int i = 0; i < length; i++)
			{
				// Add audio clip stream buffer to the channel buffer
				leftBufferIndex = i * 2;
				rightBufferIndex = 1 + i * 2;
				leftBuf = &buffer[leftBufferIndex];
				rightBuf = &buffer[rightBufferIndex];
				if (channelCount == 2)
				{
					*leftBuf = MixSoundToBuffer(*leftBuf, (short)(soundBuffer[sound->m_seekPosition] * leftVolume));
					*rightBuf = MixSoundToBuffer(*rightBuf, (short)(soundBuffer[sound->m_seekPosition + 1] * rightVolume));
				}
				else
				{
					*leftBuf = MixSoundToBuffer(*leftBuf, (short)(soundBuffer[sound->m_seekPosition] * leftVolume));
					*rightBuf = MixSoundToBuffer(*rightBuf, (short)(soundBuffer[sound->m_seekPosition] * rightVolume));
				}

				sound->m_seekNext += frequency;

				// Check if the buffer seek position needs to be moved or reset
				while (sound->m_seekNext >= SOUND_FREQUENCY)
				{
					sound->m_seekNext -= SOUND_FREQUENCY;
					if (channelCount == 2)
					{
						sound->m_seekPosition += 2;
					}
					else
					{
						sound->m_seekPosition += 1;
					}

					if (stream->GetSeekPosition() >= sampleCount) // If the stream ends, reset the seek or stop the stream
					{
						if (sound->m_loop)
						{
							stream->ResetSeek();
							sound->m_seekPosition = 0;
						}
						else
						{
							deleteAudio = true;
							//break;
						}
					}
					else if (sound->m_seekPosition == halfBuffSize) // If the buffer seek reach the middle of the buffer, ask for a new stream read
					{
						sound->m_needFillFirstHalfBuffer = true;
					}
					else if (sound->m_seekPosition == buffSize) // If the buffer seek reach the end, reset the buffer seek and ask for a new stream read
					{
						sound->m_seekPosition = 0;
						sound->m_needFillSecondHalfBuffer = true;
					}
				}
				/*if (deleteAudio) // TODO: Why is working for short audio without this? Are we stopping the audio too early?
				{
					break;
				}*/
			}

			// If the played sound needs to be deleted
			if (deleteAudio)
			{
				delete sound;
				channel->m_playedSounds.erase(channel->m_playedSounds.begin() + soundIndex);
				channel->m_playedSoundsCount--;
				soundIndex--;
				playedSoundsCount--;
				continue;
			}
		}
	}
	AudioManager::s_myMutex->Unlock();
}

#if defined(__PSP__)
int audio_thread(SceSize args, void* argp)
{
	while (true)
	{
		if (sceAudioGetChannelRestLength(0) == 0)
		{
			//audioBenchmark2->Start();
			int16_t wave_buf[AUDIO_BUFFER_SIZE * 2] = { 0 };
			AudioManager::FillChannelBuffer((short*)wave_buf, AUDIO_BUFFER_SIZE, AudioManager::s_channel);
			sceAudioOutput(0, PSP_AUDIO_VOLUME_MAX, wave_buf);
			//audioBenchmark2->Stop();
		}
		sceKernelDelayThread(2);
	}
}
#endif

#if defined(__vita__)
int audio_thread(SceSize args, void* argp)
{
	while (true)
	{
		if (sceAudioOutGetRestSample(AudioManager::s_channel->m_port) == 0)
		{
			int16_t wave_buf[AUDIO_BUFFER_SIZE * 2] = { 0 };
			AudioManager::FillChannelBuffer((short*)wave_buf, AUDIO_BUFFER_SIZE, AudioManager::s_channel);
			sceAudioOutOutput(AudioManager::s_channel->m_port, wave_buf);
		}
	}
}
#endif

#if defined(_WIN32) || defined(_WIN64)
int audio_thread()
{
	while (true)
	{
		if (!Engine::IsRunning(true))
			return 0;

		WAVEHDR* w = &waveHdr[currentBuffer];
		// If the audio header needs audio data
		if (w->dwFlags & WHDR_DONE)
		{
			waveOutUnprepareHeader(hWaveOut, w, sizeof(WAVEHDR));

			// Select current buffer
			short* buffToUse = audioData;
			if (currentBuffer == 1)
				buffToUse = audioData2;

			// Send audio data
			AudioManager::FillChannelBuffer((short*)buffToUse, quarterBuffSize, AudioManager::s_channel);
			w->lpData = reinterpret_cast<LPSTR>(buffToUse);
			w->dwBufferLength = buffSize;
			w->dwFlags = 0;
			waveOutPrepareHeader(hWaveOut, w, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, w, sizeof(WAVEHDR));

			// Change buffer
			currentBuffer = (currentBuffer + 1) % 2;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
#endif

#if defined(__vita__) || defined(__PSP__)
int fillAudioBufferThread(SceSize args, void* argp)
#else
int fillAudioBufferThread()
#endif
{
	while (true)
	{
		if (!Engine::IsRunning(true))
			return 0;

		AudioManager::s_myMutex->Lock();
		const int playedSoundsCount = (int)AudioManager::s_channel->m_playedSoundsCount;
		// Fill each stream buffers
		for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
		{
			auto& sound = AudioManager::s_channel->m_playedSounds[soundIndex];

			AudioClipStream* stream = sound->m_audioClipStream;

			int bufferSizeToUse = quarterBuffSize;
			if (stream->GetChannelCount() == 1)
			{
				bufferSizeToUse = halfBuffSize;
			}

			if (sound->m_needFillFirstHalfBuffer)
			{
				audioBenchmark->Start();
				stream->FillBuffer(bufferSizeToUse, 0, sound->m_buffer);
				sound->m_needFillFirstHalfBuffer = false;
				audioBenchmark->Stop();
			}
			else if (sound->m_needFillSecondHalfBuffer)
			{
				audioBenchmark->Start();
				stream->FillBuffer(bufferSizeToUse, halfBuffSize, sound->m_buffer);
				sound->m_needFillSecondHalfBuffer = false;
				audioBenchmark->Stop();
			}
		}

		// Get audio sources values
		if (Engine::s_canUpdateAudio)
		{
			const int count = (int)AudioManager::s_channel->m_playedSoundsCount;
			for (int i = 0; i < count; i++)
			{
				const auto& playedSound = AudioManager::s_channel->m_playedSounds[i];
				std::shared_ptr<AudioSource> audioSource = playedSound->m_audioSource.lock();
				if (audioSource) {

					playedSound->m_volume = audioSource->GetVolume();
					playedSound->m_pan = audioSource->GetPanning();
					playedSound->m_isPlaying = audioSource->IsPlaying();
					playedSound->m_loop = audioSource->IsLooping();
				}
			}
		}

		AudioManager::s_myMutex->Unlock();

#if defined(__vita__) || defined(__PSP__)
		sceKernelDelayThread(16);
#elif !defined(__PS3__)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
	}
}


Channel::Channel()
{
#if defined(__vita__)
	// This will allow to open only one channel because of SCE_AUDIO_OUT_PORT_TYPE_BGM
	m_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, AUDIO_BUFFER_SIZE, SOUND_FREQUENCY, (SceAudioOutMode)m_mode);
	int volA[2] = { m_vol, m_vol };
	sceAudioOutSetVolume(m_port, (SceAudioOutChannelFlag)(SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), volA);
#endif
}

int AudioManager::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	audioBenchmark = std::make_shared<ProfilerBenchmark>("Audio", "Audio");
	audioBenchmark2 = std::make_shared<ProfilerBenchmark>("Audio", "Sub");
	halfBuffSize = buffSize / 2;
	quarterBuffSize = buffSize / 4;

	s_myMutex = new MyMutex("AudioMutex");
	s_channel = new Channel();

#if defined(__PSP__)
	pspAudioInit();
	sceAudioChReserve(0, AUDIO_BUFFER_SIZE, 0);
	SceUID thd_id = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x18, 0x10000, 0, NULL);
	XASSERT(thd_id >= 0, "[AudioManager::Init] thd_id is bad");
	if (thd_id >= 0)
	{
		sceKernelStartThread(thd_id, 0, 0);
	}

	SceUID thd_id2 = sceKernelCreateThread("audio_thread", audio_thread, 0x18, 0x10000, 0, NULL);
	XASSERT(thd_id2 >= 0, "[AudioManager::Init] thd_id2 is bad");
	if (thd_id2 >= 0)
	{
		sceKernelStartThread(thd_id2, 0, 0);
	}

#elif defined(__vita__)
	SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x10000, 0, 0, NULL);
	SceUID thd_id2 = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x40, 0x10000, 0, 0, NULL);
	XASSERT(thd_id >= 0, "[AudioManager::Init] thd_id is bad");
	XASSERT(thd_id2 >= 0, "[AudioManager::Init] thd_id2 is bad");
	if (thd_id >= 0 && thd_id2 >= 0)
	{
		sceKernelStartThread(thd_id2, 0, 0);
		sceKernelStartThread(thd_id, 0, 0);
	}
#elif defined(_WIN32) || defined(_WIN64)
	audioData = (short*)malloc(sizeof(short) * buffSize);
	audioData2 = (short*)malloc(sizeof(short) * buffSize);

	XASSERT(audioData != nullptr, "[AudioManager::Init] audioData is null");
	XASSERT(audioData2 != nullptr, "[AudioManager::Init] audioData2 is null");

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.nAvgBytesPerSec = 44100 * 2 * 2;
	waveFormat.nBlockAlign = 4;
	waveFormat.wBitsPerSample = 16;
	waveFormat.cbSize = 0;

	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, CALLBACK_NULL, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		return -1;
	}

	for (int i = 0; i < 2; i++)
	{
		waveHdr[i].dwBytesRecorded = 0;
		waveHdr[i].dwUser = 0;
		waveHdr[i].dwLoops = 0;
		waveHdr[i].dwFlags = WHDR_DONE;
	}

	std::thread sendAudioThread = std::thread(audio_thread);
	sendAudioThread.detach();
	std::thread fillBufferThread = std::thread(fillAudioBufferThread);
	fillBufferThread.detach();
#endif
	return 0;
}

void AudioManager::Stop()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(_WIN32) || defined(_WIN64)

	// Stop WaveOut
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr[0], sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &waveHdr[1], sizeof(WAVEHDR));
	waveOutClose(hWaveOut);

	free(audioData);
	free(audioData2);

#endif
}

PlayedSound::~PlayedSound()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	delete m_audioClipStream;
	free(m_buffer);
}

void AudioManager::PlayAudioSource(const std::shared_ptr<AudioSource>& audioSource)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(audioSource != nullptr, "[AudioManager::PlayAudioSource] audioSource is null");

	bool found = false;

	if (audioSource->GetAudioClip() == nullptr)
		return;

	AudioManager::s_myMutex->Lock();

	// Find if the audio source is already playing

	const int count = s_channel->m_playedSoundsCount;
	for (int i = 0; i < count; i++)
	{
		const auto& playedSound = s_channel->m_playedSounds[i];
		if (playedSound->m_audioSource.lock() == audioSource)
		{
			found = true;
			break;
		}
	}
	AudioManager::s_myMutex->Unlock();

	if (!found)
	{
		// create PlayedSound and copy audio source values
		PlayedSound* newPlayedSound = new PlayedSound();
		newPlayedSound->m_buffer = (short*)calloc((size_t)buffSize, sizeof(short));
		AudioClipStream* newAudioClipStream = new AudioClipStream();
		newPlayedSound->m_audioClipStream = newAudioClipStream;
		newAudioClipStream->OpenStream(*audioSource->GetAudioClip());
		newPlayedSound->m_audioSource = audioSource;
		newPlayedSound->m_seekPosition = 0;
		newPlayedSound->m_needFillFirstHalfBuffer = true;
		newPlayedSound->m_needFillSecondHalfBuffer = true;

		newPlayedSound->m_volume = audioSource->GetVolume();
		newPlayedSound->m_pan = audioSource->GetPanning();
		newPlayedSound->m_isPlaying = audioSource->IsPlaying();
		newPlayedSound->m_loop = audioSource->IsLooping();

		AudioManager::s_myMutex->Lock();
		s_channel->m_playedSounds.push_back(newPlayedSound);
		s_channel->m_playedSoundsCount++;
		AudioManager::s_myMutex->Unlock();
	}
}

void AudioManager::StopAudioSource(const std::shared_ptr<AudioSource>& audioSource)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(audioSource != nullptr, "[AudioManager::StopAudioSource] audioSource is null");

	AudioManager::s_myMutex->Lock();
	int audioSourceIndex = 0;
	bool found = false;

	// Find audio source index
	const int count = s_channel->m_playedSoundsCount;
	for (int i = 0; i < count; i++)
	{
		if (s_channel->m_playedSounds[i]->m_audioSource.lock() == audioSource)
		{
			audioSourceIndex = i;
			found = true;
			break;
		}
	}

	if (found)
	{
		delete s_channel->m_playedSounds[audioSourceIndex];
		s_channel->m_playedSounds.erase(s_channel->m_playedSounds.begin() + audioSourceIndex);
		s_channel->m_playedSoundsCount--;
	}

	AudioManager::s_myMutex->Unlock();
}

/// <summary>
/// Remove an audio source from the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::RemoveAudioSource(AudioSource* audioSource)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(audioSource != nullptr, "[AudioManager::RemoveAudioSource] audioSource is null");

	AudioManager::s_myMutex->Lock();
	int audioSourceIndex = 0;
	bool found = false;

	// Find audio source index

	const int count = s_channel->m_playedSoundsCount;
	for (int i = 0; i < count; i++)
	{
		if (s_channel->m_playedSounds[i]->m_audioSource.lock().get() == audioSource)
		{
			audioSourceIndex = i;
			found = true;
			break;
		}
	}

	if (found)
	{
		delete s_channel->m_playedSounds[audioSourceIndex];
		s_channel->m_playedSounds.erase(s_channel->m_playedSounds.begin() + audioSourceIndex);
		s_channel->m_playedSoundsCount--;
	}

	AudioManager::s_myMutex->Unlock();
}

MyMutex::MyMutex([[maybe_unused]] const std::string& mutexName)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(__vita__)
	mutexid = sceKernelCreateMutex(mutexName.c_str(), 0, 1, NULL);
	//#elif defined(__PSP__)
	//	sceKernelCreateLwMutex(&workarea, mutexName.c_str(), 0, 1, NULL);
#endif
}
