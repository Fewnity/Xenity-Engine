#include "audio_manager.h"
#include "audio_clip.h"
#include "audio_clip_stream.h"

#include "../../xenity.h"

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
#endif

bool AudioManager::isAdding = false;
int AudioManager::channelCount = 0;
std::vector<Channel*> AudioManager::channels;
int buffSize = 1024 * 16;
int halfBuffSize = 0;
int quarterBuffSize = 0;
MyMutex* AudioManager::myMutex = nullptr;

ProfilerBenchmark* audioBenchmark = nullptr;
ProfilerBenchmark* audioBenchmark2 = nullptr;

#define audiosize 2048

short MixSoundToBuffer(short bufferValue, short soundValue)
{
	int newVal = bufferValue + soundValue;
	if (newVal > 32767)
		newVal = 32767;
	else if (newVal < -32768)
		newVal = -32768;

	return newVal;
}

void FillChannelBuffer(short* buffer, int length, Channel* channel)
{
	for (int i = 0; i < length; i++)
	{
		int leftBufferIndex = i * 2;
		int rightBufferIndex = 1 + i * 2;
		buffer[leftBufferIndex] = 0;
		buffer[rightBufferIndex] = 0;
	}

	AudioManager::myMutex->Lock();

	int playedSoundsCount = (int)channel->playedSounds.size();
	for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
	{
		auto sound = channel->playedSounds[soundIndex];
#if defined(_WIN32) || defined(_WIN64)
		float leftPan = std::max<float>(0.0f, std::min<float>(0.5f, 1 - sound->pan)) * 2;
		float rightPan = std::max<float>(0.0f, std::min<float>(0.5f, sound->pan)) * 2;
#else
		float leftPan = std::max(0.0f, std::min(0.5f, 1 - sound->pan)) * 2;
		float rightPan = std::max(0.0f, std::min(0.5f, sound->pan)) * 2;
#endif
		float leftVolume = sound->volume * leftPan;
		float rightVolume = sound->volume * rightPan;
		bool isPlaying = sound->isPlaying;

		if (isPlaying)
		{
			for (int i = 0; i < length; i++)
			{
				int leftBufferIndex = i * 2;
				int rightBufferIndex = 1 + i * 2;

				buffer[leftBufferIndex] = MixSoundToBuffer(buffer[leftBufferIndex], (short)(sound->buffer[sound->seekPosition] * leftVolume));
				buffer[rightBufferIndex] = MixSoundToBuffer(buffer[rightBufferIndex], (short)(sound->buffer[sound->seekPosition + 1] * rightVolume));

				sound->seekNext += sound->audioClipStream->GetFrequency();

				while (sound->seekNext >= SOUND_FREQUENCY)
				{
					sound->seekNext -= SOUND_FREQUENCY;
					sound->seekPosition += 2;

					if (sound->audioClipStream->GetSeekPosition() >= sound->audioClipStream->GetSampleCount())
					{
						sound->audioClipStream->ResetSeek();
						sound->seekPosition = 0;
					}
					else if (sound->seekPosition == halfBuffSize)
					{
						sound->needNewRead = true;
					}
					else if (sound->seekPosition == buffSize)
					{
						sound->seekPosition = 0;
						sound->needNewRead2 = true;
					}
				}
			}
		}
	}
	AudioManager::myMutex->Unlock();
}

#if defined(__PSP__)
void audioCallback(void* buf, unsigned int length, void* userdata)
{
	audioBenchmark2->Start();
	Channel* channel = AudioManager::channels[(int)userdata];
	FillChannelBuffer((short*)buf, length, channel);
	audioBenchmark2->Stop();
}
#endif

#if defined(__vita__)
int audio_thread(SceSize args, void* argp)
{
	while (true)
	{
		for (int i = 0; i < AudioManager::channelCount; i++)
		{
			Channel* channel = AudioManager::channels[i];
			if (sceAudioOutGetRestSample(channel->port) == 0)
			{
				int16_t wave_buf[audiosize * 2] = { 0 };
				FillChannelBuffer((short*)wave_buf, audiosize, channel);
				sceAudioOutOutput(channel->port, wave_buf);
			}
		}
	}
}
#endif

#if defined(_WIN32) || defined(_WIN64)
int audio_thread()
{
	while (true)
	{
		if (!Engine::IsRunning())
			return 0;

		WAVEHDR* w = &waveHdr[currentBuffer];
		if (w->dwFlags & WHDR_DONE)
		{
			waveOutUnprepareHeader(hWaveOut, w, sizeof(WAVEHDR));
			if (audioData == nullptr) 
			{
				audioData = (short*)malloc(sizeof(short) * buffSize);
				audioData2 = (short*)malloc(sizeof(short) * buffSize);
			}
			short* buffToUse = audioData;
			if(currentBuffer == 1)
				buffToUse = audioData2;
			
			Channel* channel = AudioManager::channels[0];
			FillChannelBuffer((short*)buffToUse, quarterBuffSize, channel);
			w->lpData = reinterpret_cast<LPSTR>(buffToUse); // audioData est votre tableau d'échantillons audio
			w->dwBufferLength = buffSize; // dataSize est la taille des données audio
			w->dwFlags = 0;
			waveOutPrepareHeader(hWaveOut, w, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, w, sizeof(WAVEHDR));

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
		for (int i = 0; i < AudioManager::channelCount; i++)
		{
			Channel* channel = AudioManager::channels[i];

			if (!Engine::IsRunning())
				return 0;
			AudioManager::myMutex->Lock();
			int playedSoundsCount = (int)channel->playedSounds.size();
			for (int soundIndex = 0; soundIndex < playedSoundsCount; soundIndex++)
			{
				auto sound = channel->playedSounds[soundIndex];
				if (sound->needNewRead)
				{
					audioBenchmark->Start();
					sound->audioClipStream->FillBuffer(quarterBuffSize, 0, sound->buffer);
					sound->needNewRead = false;
					audioBenchmark->Stop();
				}
				else if (sound->needNewRead2)
				{
					audioBenchmark->Start();
					sound->audioClipStream->FillBuffer(quarterBuffSize, halfBuffSize, sound->buffer);
					sound->needNewRead2 = false;
					audioBenchmark->Stop();
				}
			}
			AudioManager::myMutex->Unlock();
		}

		if (Engine::canUpdateAudio)
		{
			AudioManager::myMutex->Lock();
			Channel* channel = AudioManager::channels[0];
			int count = (int)channel->playedSounds.size();
			for (int i = 0; i < count; i++)
			{
				auto playedSound = channel->playedSounds[i];
				playedSound->volume = playedSound->audioSource->GetVolume();
				playedSound->pan = playedSound->audioSource->GetPanning();
				playedSound->isPlaying = playedSound->audioSource->GetIsPlaying();
			}
			AudioManager::myMutex->Unlock();
		}
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
	audioBenchmark = new ProfilerBenchmark("Audio", "Audio");
	audioBenchmark2 = new ProfilerBenchmark("Audio", "Sub");
	halfBuffSize = buffSize / 2;
	quarterBuffSize = buffSize / 4;

	myMutex = new MyMutex();
#if defined(__vita__)
	myMutex->mutexid = sceKernelCreateMutex("MyMutex", 0, 1, NULL);
#endif
	Channel* newChannel = new Channel();
	channels.push_back(newChannel);
	channelCount++;

#if defined(__PSP__)
	pspAudioInit();
	for (int i = 0; i < channelCount; i++)
	{
		pspAudioSetChannelCallback(i, audioCallback, (void*)i);
	}

	SceUID thd_id = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x18, 0x10000, 0, NULL);
	if (thd_id >= 0)
	{
		sceKernelStartThread(thd_id, 0, 0);
	}
#elif defined(__vita__)
	SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x400000, 0, 0, NULL);
	SceUID thd_id2 = sceKernelCreateThread("fillAudioBufferThread", fillAudioBufferThread, 0x40, 0x400000, 0, 0, NULL);
	if (thd_id >= 0 && thd_id2 >= 0)
	{
		sceKernelStartThread(thd_id2, 0, 0);
		sceKernelStartThread(thd_id, 0, 0);
	}
#elif defined(_WIN32) || defined(_WIN64)
	std::thread sendAudioThread = std::thread(audio_thread);
	sendAudioThread.detach();
	std::thread fillBufferThread = std::thread(fillAudioBufferThread);
	fillBufferThread.detach();

	// Ouvrir le fichier audio WAV
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

#endif
	return 0;
}

void AudioManager::Stop()
{
#if defined(_WIN32) || defined(_WIN64)
	if(audioData)
		free (audioData);
	if (audioData2)
		free(audioData2);
#endif
}

PlayedSound::~PlayedSound()
{
	delete audioClipStream;
	if (buffer)
		free(buffer);
}

void AudioManager::PlayAudioSource(std::weak_ptr<AudioSource> audioSource)
{
	AudioManager::myMutex->Lock();
	Channel* channel = channels[0];
	bool found = false;
	if (auto as = audioSource.lock())
	{
		if (as->audioClip == nullptr)
			return;

		int count = (int)channel->playedSounds.size();
		for (int i = 0; i < count; i++)
		{
			auto playedSound = channel->playedSounds[i];
			if (playedSound->audioSource == as)
			{
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		std::shared_ptr<PlayedSound> newPlayedSound = std::make_shared<PlayedSound>();
		newPlayedSound->buffer = (short*)calloc((size_t)buffSize, sizeof(short));
		AudioClipStream* newAudioClipStream = new AudioClipStream();
		newPlayedSound->audioClipStream = newAudioClipStream;
		newAudioClipStream->OpenStream(audioSource.lock()->audioClip->filePath);
		newPlayedSound->audioSource = audioSource.lock();
		newPlayedSound->seekPosition = 0;
		newPlayedSound->needNewRead = true;
		newPlayedSound->needNewRead2 = true;

		newPlayedSound->volume = newPlayedSound->audioSource->GetVolume();
		newPlayedSound->pan = newPlayedSound->audioSource->GetPanning();
		newPlayedSound->isPlaying = newPlayedSound->audioSource->GetIsPlaying();
		newPlayedSound->loop = true;
		channels[0]->playedSounds.push_back(newPlayedSound);
	}
	AudioManager::myMutex->Unlock();
}

void AudioManager::StopAudioSource(std::weak_ptr<AudioSource> audioSource)
{
	AudioManager::myMutex->Lock();
	int audioSourceIndex = 0;
	bool found = false;
	Channel* channel = channels[0];

	if (auto as = audioSource.lock())
	{
		int count = (int)channel->playedSounds.size();
		for (int i = 0; i < count; i++)
		{
			if (channel->playedSounds[i]->audioSource == as)
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
	}
	AudioManager::myMutex->Unlock();
}

/// <summary>
/// Remove an audio source from the audio source list
/// </summary>
/// <param name="light"></param>
void AudioManager::RemoveAudioSource(std::weak_ptr<AudioSource> audioSource)
{
	AudioManager::myMutex->Lock();
	int audioSourceIndex = 0;
	bool found = false;
	Channel* channel = channels[0];

	if (auto as = audioSource.lock())
	{
		int count = (int)channel->playedSounds.size();
		for (int i = 0; i < count; i++)
		{
			if (channel->playedSounds[i]->audioSource == as)
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
	}
	AudioManager::myMutex->Unlock();
}