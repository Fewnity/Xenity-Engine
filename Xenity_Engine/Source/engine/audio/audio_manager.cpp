#include "audio_manager.h"

#include "audio_clip.h"
#include "audio_clip_stream.h"

#include <engine/engine.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/debug/debug.h>

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
#endif

#include <cstring>


bool AudioManager::isAdding = false;
Channel* AudioManager::channel;
int buffSize = 1024 * 16;
int halfBuffSize = 0;
int quarterBuffSize = 0;
MyMutex* AudioManager::myMutex = nullptr;

std::shared_ptr<ProfilerBenchmark> audioBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> audioBenchmark2 = nullptr;

#define audiosize 2048

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

void FillChannelBuffer(short* buffer, int length, Channel* channel)
{
	// Reset buffer
	for (int i = 0; i < length; i++)
	{
		int leftBufferIndex = i * 2;
		int rightBufferIndex = 1 + i * 2;
		buffer[leftBufferIndex] = 0;
		buffer[rightBufferIndex] = 0;
	}

	AudioManager::myMutex->Lock();

	// For each sound, add it's buffer to the sound buffer and change seek of audio stream
	int playedSoundsCount = (int)channel->playedSoundsCount;
	for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
	{
		const std::shared_ptr<PlayedSound>& sound = channel->playedSounds[soundIndex];
#if defined(EDITOR)
		if (sound->isPlaying && ((sound->audioSource.lock() && sound->audioSource.lock()->isEditor) || GameplayManager::GetGameState() == GameState::Playing))
#else
		if (sound->isPlaying)
#endif
		{
			AudioClipStream* stream = sound->audioClipStream;
#if defined(_WIN32) || defined(_WIN64)
			const float leftPan = std::max<float>(0.0f, std::min<float>(0.5f, 1 - sound->pan)) * 2;
			const float rightPan = std::max<float>(0.0f, std::min<float>(0.5f, sound->pan)) * 2;
#else
			const float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->pan)) * 2;
			const float rightPan = std::max(0.0f, std::min(0.5f, sound->pan)) * 2;
#endif
			const float leftVolume = sound->volume * leftPan;
			const float rightVolume = sound->volume * rightPan;
			const int channelCount = stream->GetChannelCount();
			short* rightBuf = nullptr;
			short* leftBuf = nullptr;
			const int frequency = stream->GetFrequency();
			const uint64_t sampleCount = stream->GetSampleCount();
			int leftBufferIndex = 0;
			int rightBufferIndex = 0;
			const short* soundBuffer = sound->buffer;

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
					*leftBuf = MixSoundToBuffer(*leftBuf, (short)(soundBuffer[sound->seekPosition] * leftVolume));
					*rightBuf = MixSoundToBuffer(*rightBuf, (short)(soundBuffer[sound->seekPosition + 1] * rightVolume));
				}
				else
				{
					*leftBuf = MixSoundToBuffer(*leftBuf, (short)(soundBuffer[sound->seekPosition] * leftVolume));
					*rightBuf = MixSoundToBuffer(*rightBuf, (short)(soundBuffer[sound->seekPosition] * rightVolume));
				}

				sound->seekNext += frequency;


				// Check if the buffer seek position needs to be moved or reset
				while (sound->seekNext >= SOUND_FREQUENCY)
				{
					sound->seekNext -= SOUND_FREQUENCY;
					if (channelCount == 2)
					{
						sound->seekPosition += 2;
					}
					else
					{
						sound->seekPosition += 1;
					}

					if (stream->GetSeekPosition() >= sampleCount) // If the stream ends, reset the seek or stop the stream
					{
						if (sound->loop)
						{
							stream->ResetSeek();
							sound->seekPosition = 0;
						}
						else
						{
							deleteAudio = true;
							//break;
						}
					}
					else if (sound->seekPosition == halfBuffSize) // If the buffer seek reach the middle of the buffer, ask for a new stream read
					{
						sound->needFillFirstHalfBuffer = true;
					}
					else if (sound->seekPosition == buffSize) // If the buffer seek reach the end, reset the buffer seek and ask for a new stream read
					{
						sound->seekPosition = 0;
						sound->needFillSecondHalfBuffer = true;
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
				channel->playedSounds.erase(channel->playedSounds.begin() + soundIndex);
				channel->playedSoundsCount--;
				soundIndex--;
				playedSoundsCount--;
				continue;
			}
		}
	}
	AudioManager::myMutex->Unlock();
}

#if defined(__PSP__)
int audio_thread(SceSize args, void* argp)
{
	while (true)
	{
		if (sceAudioGetChannelRestLength(0) == 0) 
		{
			//audioBenchmark2->Start();
			int16_t wave_buf[audiosize * 2] = { 0 };
			FillChannelBuffer((short*)wave_buf, audiosize, AudioManager::channel);
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
		if (sceAudioOutGetRestSample(AudioManager::channel->port) == 0)
		{
			int16_t wave_buf[audiosize * 2] = { 0 };
			FillChannelBuffer((short*)wave_buf, audiosize, AudioManager::channel);
			sceAudioOutOutput(AudioManager::channel->port, wave_buf);
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
			FillChannelBuffer((short*)buffToUse, quarterBuffSize, AudioManager::channel);
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

		AudioManager::myMutex->Lock();
		const int playedSoundsCount = (int)AudioManager::channel->playedSoundsCount;
		// Fill each stream buffers
		for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
		{
			auto& sound = AudioManager::channel->playedSounds[soundIndex];

			AudioClipStream* stream = sound->audioClipStream;

			int bufferSizeToUse = quarterBuffSize;
			if (stream->GetChannelCount() == 1)
			{
				bufferSizeToUse = halfBuffSize;
			}

			if (sound->needFillFirstHalfBuffer)
			{
				audioBenchmark->Start();
				stream->FillBuffer(bufferSizeToUse, 0, sound->buffer);
				sound->needFillFirstHalfBuffer = false;
				audioBenchmark->Stop();
			}
			else if (sound->needFillSecondHalfBuffer)
			{
				audioBenchmark->Start();
				stream->FillBuffer(bufferSizeToUse, halfBuffSize, sound->buffer);
				sound->needFillSecondHalfBuffer = false;
				audioBenchmark->Stop();
			}
		}

		// Get audio sources values
		if (Engine::canUpdateAudio)
		{
			const int count = (int)AudioManager::channel->playedSoundsCount;
			for (int i = 0; i < count; i++)
			{
				const auto& playedSound = AudioManager::channel->playedSounds[i];
				std::shared_ptr<AudioSource> audioSource = playedSound->audioSource.lock();
				if (audioSource) {

					playedSound->volume = audioSource->GetVolume();
					playedSound->pan = audioSource->GetPanning();
					playedSound->isPlaying = audioSource->GetIsPlaying();
					playedSound->loop = audioSource->GetIsLooping();
				}
			}
		}

		AudioManager::myMutex->Unlock();

#if defined(__vita__) || defined(__PSP__)
		sceKernelDelayThread(16);
#else
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
	}
}


Channel::Channel()
{
#if defined(__vita__)
	// This will allow to open only one channel because of SCE_AUDIO_OUT_PORT_TYPE_BGM
	port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, audiosize, SOUND_FREQUENCY, (SceAudioOutMode)mode);
	int volA[2] = { vol, vol };
	sceAudioOutSetVolume(port, (SceAudioOutChannelFlag)(SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), volA);
#endif
}

int AudioManager::Init()
{
	audioBenchmark = std::make_shared<ProfilerBenchmark>("Audio", "Audio");
	audioBenchmark2 = std::make_shared<ProfilerBenchmark>("Audio", "Sub");
	halfBuffSize = buffSize / 2;
	quarterBuffSize = buffSize / 4;

	myMutex = new MyMutex("AudioMutex");
	channel = new Channel();

#if defined(__PSP__)
	pspAudioInit();
	sceAudioChReserve(0, audiosize, 0);
	SceUID thd_id = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x18, 0x10000, 0, NULL);
	if (thd_id >= 0)
	{
		sceKernelStartThread(thd_id, 0, 0);
	}

	SceUID thd_id2 = sceKernelCreateThread("audio_thread", audio_thread, 0x18, 0x10000, 0, NULL);
	if (thd_id2 >= 0)
	{
		sceKernelStartThread(thd_id2, 0, 0);
	}

#elif defined(__vita__)
	SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x10000, 0, 0, NULL);
	SceUID thd_id2 = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x40, 0x10000, 0, 0, NULL);
	if (thd_id >= 0 && thd_id2 >= 0)
	{
		sceKernelStartThread(thd_id2, 0, 0);
		sceKernelStartThread(thd_id, 0, 0);
	}
#elif defined(_WIN32) || defined(_WIN64)
	audioData = (short*)malloc(sizeof(short) * buffSize);
	audioData2 = (short*)malloc(sizeof(short) * buffSize);

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
	delete audioClipStream;
	free(buffer);
}

void AudioManager::PlayAudioSource(const std::shared_ptr<AudioSource>& audioSource)
{
	//return;

	if (!audioSource)
		return;

	bool found = false;

	if (audioSource->GetAudioClip() == nullptr)
		return;

	AudioManager::myMutex->Lock();

	// Find if the audio source is already playing

	const int count = channel->playedSoundsCount;
	for (int i = 0; i < count; i++)
	{
		const auto& playedSound = channel->playedSounds[i];
		if (playedSound->audioSource.lock() == audioSource)
		{
			found = true;
			break;
		}
	}
	AudioManager::myMutex->Unlock();

	if (!found)
	{
		// create PlayedSound and copy audio source values
		std::shared_ptr<PlayedSound> newPlayedSound = std::make_shared<PlayedSound>();
		newPlayedSound->buffer = (short*)calloc((size_t)buffSize, sizeof(short));
		AudioClipStream* newAudioClipStream = new AudioClipStream();
		newPlayedSound->audioClipStream = newAudioClipStream;
		newAudioClipStream->OpenStream(audioSource->GetAudioClip());
		newPlayedSound->audioSource = audioSource;
		newPlayedSound->seekPosition = 0;
		newPlayedSound->needFillFirstHalfBuffer = true;
		newPlayedSound->needFillSecondHalfBuffer = true;

		newPlayedSound->volume = audioSource->GetVolume();
		newPlayedSound->pan = audioSource->GetPanning();
		newPlayedSound->isPlaying = audioSource->GetIsPlaying();
		newPlayedSound->loop = audioSource->GetIsLooping();

		AudioManager::myMutex->Lock();
		channel->playedSounds.push_back(newPlayedSound);
		channel->playedSoundsCount++;
		AudioManager::myMutex->Unlock();
	}
}

void AudioManager::StopAudioSource(const std::shared_ptr<AudioSource>& audioSource)
{
	AudioManager::myMutex->Lock();
	int audioSourceIndex = 0;
	bool found = false;

	// Find audio source index
	if (audioSource)
	{
		const int count = channel->playedSoundsCount;
		for (int i = 0; i < count; i++)
		{
			if (channel->playedSounds[i]->audioSource.lock() == audioSource)
			{
				audioSourceIndex = i;
				found = true;
				break;
			}
		}
	}

	if (found)
	{
		channel->playedSounds.erase(channel->playedSounds.begin() + audioSourceIndex);
		channel->playedSoundsCount--;
	}
	AudioManager::myMutex->Unlock();
}

/// <summary>
/// Remove an audio source from the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::RemoveAudioSource(AudioSource* audioSource)
{
	AudioManager::myMutex->Lock();
	int audioSourceIndex = 0;
	bool found = false;

	// Find audio source index
	if (audioSource)
	{
		const int count = channel->playedSoundsCount;
		for (int i = 0; i < count; i++)
		{
			if (channel->playedSounds[i]->audioSource.lock().get() == audioSource)
			{
				audioSourceIndex = i;
				found = true;
				break;
			}
		}
	}

	if (found)
	{
		channel->playedSounds.erase(channel->playedSounds.begin() + audioSourceIndex);
		channel->playedSoundsCount--;
	}
	AudioManager::myMutex->Unlock();
}

MyMutex::MyMutex(const std::string& mutexName)
{
#if defined(__vita__)
	mutexid = sceKernelCreateMutex(mutexName.c_str(), 0, 1, NULL);
//#elif defined(__PSP__)
//	sceKernelCreateLwMutex(&workarea, mutexName.c_str(), 0, 1, NULL);
#endif
}
