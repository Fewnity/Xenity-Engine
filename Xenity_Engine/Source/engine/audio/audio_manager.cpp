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
#elif defined(__PS3__)
#include <audio/audio.h>
#include <sys/thread.h>
#include <unistd.h>
u64 snd_key;
sys_event_queue_t snd_queue;
audioPortParam params;
audioPortConfig config;
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
#include <engine/tools/endian_utils.h>

bool AudioManager::s_isAdding = false;
Channel* AudioManager::s_channel;
constexpr int buffSize = 1024 * 16;
int halfBuffSize = 0;
int quarterBuffSize = 0;
MyMutex* AudioManager::s_myMutex = nullptr;

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

void AudioManager::FillChannelBuffer(short* buffer, uint64_t length, Channel* channel)
{
	// Reset buffer
	for (uint64_t i = 0; i < length; i++)
	{
		uint64_t leftBufferIndex = i * 2;
		uint64_t rightBufferIndex = 1 + i * 2;
		buffer[leftBufferIndex] = 0;
		buffer[rightBufferIndex] = 0;
	}

	AudioManager::s_myMutex->Lock();

	// For each sound, add it's buffer to the sound buffer and change seek of audio stream
	size_t playedSoundsCount = channel->m_playedSoundsCount;
	for (size_t soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
	{
		PlayedSound* sound = channel->m_playedSounds[soundIndex];

		// Security checks
		if(sound->m_bufferSeekPosition < halfBuffSize && sound->m_needFillFirstHalfBuffer)
		{
			continue;
		}
		else if(sound->m_bufferSeekPosition > halfBuffSize && sound->m_needFillSecondHalfBuffer)
		{
			continue;
		}

#if defined(EDITOR)
		if (sound->m_isPlaying && ((sound->m_audioSource.lock() && sound->m_audioSource.lock()->m_isEditor) || GameplayManager::GetGameState() == GameState::Playing))
#else
		if (sound->m_isPlaying)
#endif
		{
			const std::unique_ptr<AudioClipStream>& stream = sound->m_audioClipStream;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
			const float leftPan = std::max<float>(0.0f, std::min<float>(0.5f, 1 - sound->m_pan)) * 2;
			const float rightPan = std::max<float>(0.0f, std::min<float>(0.5f, sound->m_pan)) * 2;
#else
			const float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->m_pan)) * 2;
			const float rightPan = std::max(0.0f, std::min(0.5f, sound->m_pan)) * 2;
#endif
			const float leftVolume = sound->m_volume * leftPan;
			const float rightVolume = sound->m_volume * rightPan;
			const uint32_t channelCount = stream->GetChannelCount();
			short* rightBuf = nullptr;
			short* leftBuf = nullptr;
			const uint32_t frequency = stream->GetFrequency();
			const uint64_t sampleCount = stream->GetSampleCount();
			int leftBufferIndex = 0;
			int rightBufferIndex = 0;
			const short* soundBuffer = sound->m_buffer;

			bool deleteAudio = false;
			for (uint64_t i = 0; i < length; i++)
			{
				// Add audio clip stream buffer to the channel buffer
				leftBufferIndex = i * 2;
				rightBufferIndex = 1 + i * 2;
				leftBuf = &buffer[leftBufferIndex];
				rightBuf = &buffer[rightBufferIndex];
				if (channelCount == 2)
				{
					*leftBuf = MixSoundToBuffer(*leftBuf, (short)(soundBuffer[sound->m_bufferSeekPosition] * leftVolume));
					*rightBuf = MixSoundToBuffer(*rightBuf, (short)(soundBuffer[sound->m_bufferSeekPosition + 1] * rightVolume));
				}
				else
				{
					*leftBuf = MixSoundToBuffer(*leftBuf, (short)(soundBuffer[sound->m_bufferSeekPosition] * leftVolume));
					*rightBuf = MixSoundToBuffer(*rightBuf, (short)(soundBuffer[sound->m_bufferSeekPosition] * rightVolume));
				}

				sound->m_seekNext += frequency;

				// Check if the buffer seek position needs to be moved or reset
				while (sound->m_seekNext >= SOUND_FREQUENCY)
				{
					sound->m_seekNext -= SOUND_FREQUENCY;
					if (channelCount == 2)
					{
						sound->m_bufferSeekPosition += 2;
						sound->m_audioSeekPosition += 2;
					}
					else
					{
						sound->m_bufferSeekPosition += 1;
						sound->m_audioSeekPosition += 1;
					}

					if (sound->m_audioSeekPosition >= sampleCount) // If the stream ends, reset the seek or stop the stream
					{
						if (sound->m_loop)
						{
							stream->ResetSeek();
							sound->m_bufferSeekPosition = 0;
							sound->m_audioSeekPosition = 0;
						}
						else
						{
							deleteAudio = true;
							break;
						}
					}
					else if (sound->m_bufferSeekPosition == halfBuffSize) // If the buffer seek reach the middle of the buffer, ask for a new stream read
					{
						sound->m_needFillFirstHalfBuffer = true;
					}
					else if (sound->m_bufferSeekPosition == buffSize) // If the buffer seek reach the end, reset the buffer seek and ask for a new stream read
					{
						sound->m_bufferSeekPosition = 0;
						sound->m_needFillSecondHalfBuffer = true;
					}
				}
				if (deleteAudio) // TODO: Why is working for short audio without this? Are we stopping the audio too early?
				{
					break;
				}
			}

			//If the played sound needs to be deleted
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

#if defined(__PS3__)
void audio_thread(void *arg)
{
	while (true)
	{
		u64 current_block = *(u64*)((u64)config.readIndex);
		f32 *dataStart = (f32*)((u64)config.audioDataStart);
		u32 audio_block_index = (current_block + 1)%config.numBlocks;

		sys_event_t event;
		s32 ret = sysEventQueueReceive(snd_queue, &event, 20*1000);
		f32* buf = dataStart + config.channelCount*AUDIO_BLOCK_SAMPLES*audio_block_index;

		int16_t wave_buf[AUDIO_BLOCK_SAMPLES * 2] = { 0 };
		AudioManager::FillChannelBuffer((short*)wave_buf, AUDIO_BLOCK_SAMPLES, AudioManager::s_channel);
		for (int i2 = 0; i2 < AUDIO_BLOCK_SAMPLES*2; i2++)
		{
			buf[i2] = (wave_buf[i2]) / 35535.0f;
		}
		usleep(2);
	}
	
	sysThreadExit(0);
}
#endif

#if defined(__PSP__)
int audio_thread(SceSize args, void* argp)
{
	while (true)
	{
		if (sceAudioGetChannelRestLength(0) == 0)
		{
			int16_t wave_buf[AUDIO_BUFFER_SIZE * 2] = { 0 };
			AudioManager::FillChannelBuffer((short*)wave_buf, AUDIO_BUFFER_SIZE, AudioManager::s_channel);
			sceAudioOutput(0, PSP_AUDIO_VOLUME_MAX, wave_buf);
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
#elif defined(__PS3__)
void fillAudioBufferThread(void* args)
#else
int fillAudioBufferThread()
#endif
{
	while (true)
	{
		if (!Engine::IsRunning(true))
		{
		#if defined(__PS3__)
			return;
		#else
			return 0;	
		#endif
		}

		AudioManager::s_myMutex->Lock();
		const size_t playedSoundsCount = AudioManager::s_channel->m_playedSoundsCount;

		// Fill each stream buffers
		for (size_t soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
		{
			auto& sound = AudioManager::s_channel->m_playedSounds[soundIndex];

			const std::unique_ptr<AudioClipStream>& stream = sound->m_audioClipStream;

			size_t bufferSizeToUse = quarterBuffSize;
			if (stream->GetChannelCount() == 1)
			{
				bufferSizeToUse = halfBuffSize;
			}

			if (sound->m_needFillFirstHalfBuffer)
			{
				AudioManager::s_myMutex->Unlock();
				stream->FillBuffer(bufferSizeToUse, sound->m_buffer);
				AudioManager::s_myMutex->Lock();
				sound->m_needFillFirstHalfBuffer = false;
			}
			else if (sound->m_needFillSecondHalfBuffer)
			{
				AudioManager::s_myMutex->Unlock();
				stream->FillBuffer(bufferSizeToUse, sound->m_buffer + halfBuffSize);
				AudioManager::s_myMutex->Lock();
				sound->m_needFillSecondHalfBuffer = false;
			}
		}

		// Get audio sources values
		if (Engine::s_canUpdateAudio)
		{
			const size_t count = AudioManager::s_channel->m_playedSoundsCount;
			for (size_t i = 0; i < count; i++)
			{
				const auto& playedSound = AudioManager::s_channel->m_playedSounds[i];
				const std::shared_ptr<AudioSource> audioSource = playedSound->m_audioSource.lock();
				if (audioSource) 
				{
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
#elif defined(__PS3__)
		sysThreadYield();
		usleep(16);
#else
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
#elif (__PS3__)
	int ret = audioInit();
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] audioInit error code: " + std::to_string(ret));
		return 1;
	}
	u32 portNum;

	//set some parameters we want
	//either 2 or 8 channel
	params.numChannels = AUDIO_PORT_2CH;
	//8 16 or 32 block buffer
	params.numBlocks = AUDIO_BLOCK_8;
	//extended attributes
	params.attrib = 0;
	//sound level (1 is default)
	params.level = 1;

	ret = audioPortOpen(&params, &portNum);
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] audioPortOpen " + std::to_string(ret));
		return 1;
	}

	ret = audioGetPortConfig(portNum, &config);
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] audioGetPortConfig " + std::to_string(ret));
		return 1;
	}

	ret = audioCreateNotifyEventQueue(&snd_queue, &snd_key);
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] audioCreateNotifyEventQueue " + std::to_string(ret));
		return 1;
	}

	ret = audioSetNotifyEventQueue(snd_key);
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] audioSetNotifyEventQueue " + std::to_string(ret));
		return 1;
	}

	ret = sysEventQueueDrain(snd_queue);
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] sysEventQueueDrain " + std::to_string(ret));
		return 1;
	}

	ret = audioPortStart(portNum);
	if (ret != 0)
	{
		Debug::PrintError("[AudioManager::Init] audioPortStart " + std::to_string(ret));
		return 1;
	}

	sys_ppu_thread_t id;
	u64 prio = 1500;
	size_t stacksize = 0x10000;
	char *audioThreadname = (char*) malloc(13*sizeof(char));

	strcpy(audioThreadname, "audio_thread");
	void *threadarg = (void*)0x1337;
	int tret = sysThreadCreate(&id, audio_thread, threadarg, prio, stacksize, THREAD_JOINABLE, audioThreadname);

	sys_ppu_thread_t id2;
	u64 prio2 = 1500;
	size_t stacksize2 = 0x100000;
	char *audioBufferThreadname = (char*) malloc(22*sizeof(char));
	strcpy(audioBufferThreadname, "fillAudioBufferThread");
	int tret2 = sysThreadCreate(&id2, fillAudioBufferThread, threadarg, prio2, stacksize2, THREAD_JOINABLE, audioBufferThreadname);
	sysThreadDetach(id2);
	sysThreadDetach(id);
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
#elif (__PS3__)
	int ret=audioQuit();
#endif
}

PlayedSound::PlayedSound()
{
}

PlayedSound::~PlayedSound()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

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

	const size_t count = s_channel->m_playedSoundsCount;
	for (size_t i = 0; i < count; i++)
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
		newPlayedSound->m_audioClipStream = std::make_unique<AudioClipStream>();
		newPlayedSound->m_audioClipStream->OpenStream(*audioSource->GetAudioClip());
		newPlayedSound->m_audioSource = audioSource;
		newPlayedSound->m_bufferSeekPosition = 0;
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
	const size_t count = s_channel->m_playedSoundsCount;
	for (size_t i = 0; i < count; i++)
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

	const size_t count = s_channel->m_playedSoundsCount;
	for (size_t i = 0; i < count; i++)
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
#elif defined(__PS3__)
	sysMutexAttrInitialize(mattr);
	sysMutexCreate(&mutex, &mattr);
#endif
}
