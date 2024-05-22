// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "audio_clip_stream.h"

#include <engine/debug/debug.h>

#include <engine/file_system/file.h>
#include <engine/audio/audio_clip.h>

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

#include <stb_vorbis.c>
#include "audio_clip.h"
#include <engine/assertions/assertions.h>


void AudioClipStream::OpenStream(std::shared_ptr<AudioClip> audioFile)
{
	DXASSERT(audioFile != nullptr, "[AudioClipStream::OpenStream] audioFile is null")
	std::shared_ptr<File> file = audioFile->file;
	const std::string& path = file->GetPath();
	//Debug::Print("Loading audio clip: " + path, true);

	// To lower extention
	std::string lowerExt = file->GetFileExtension().substr(1);
	const size_t pathSize = lowerExt.size();
	for (size_t i = 0; i < pathSize; i++)
	{
		lowerExt[i] = tolower(lowerExt[i]);
	}

	const AudioClip::AudioMemory& audioMemory = audioFile->GetAudioMemory();
	if (lowerExt == "wav")
	{
		wav = new drwav();
		if ((audioFile->GetIsLoadedInMemory() && !drwav_init_memory(wav, audioMemory.data, audioMemory.dataLength, NULL)) || (!audioFile->GetIsLoadedInMemory() && !drwav_init_file(wav, path.c_str(), NULL)))
		{
			// Error opening WAV file.
			Debug::PrintError("[AudioClipStream::OpenStream] Cannot init wav file: " + path, true);
			delete wav;
		}
		else
		{
			type = AudioType::Wav;
			// Get informations
			channelCount = wav->channels;
			sampleCount = wav->totalPCMFrameCount;
			//Debug::Print("Audio clip data: " + std::to_string(wav->channels) + " " + std::to_string(wav->sampleRate), true);
		}
	}
	else if (lowerExt == "mp3")
	{
		mp3 = new drmp3();
		if ((audioFile->GetIsLoadedInMemory() && !drmp3_init_memory(mp3, audioMemory.data, audioMemory.dataLength, NULL)) || (!audioFile->GetIsLoadedInMemory() && !drmp3_init_file(mp3, path.c_str(), NULL)))
		{
			// Error opening MP3 file.
			Debug::PrintError("[AudioClipStream::OpenStream] Cannot init mp3 file: " + path, true);
			delete mp3;
		}
		else
		{
			type = AudioType::Mp3;
			// Get informations
			channelCount = mp3->channels;
			sampleCount = drmp3_get_pcm_frame_count(mp3);
			//Debug::Print("Audio clip data: " + std::to_string(mp3->channels) + " " + std::to_string(mp3->sampleRate), true);
		}
	}
	else
	{
		Debug::PrintError("[AudioClipStream::OpenStream] unknown file format: " + path, true);
	}

	//////////////////////////////////// OGG
	// int channels, sample_rate;
	// short *data;
	// stb_vorbis *stream = stb_vorbis_open_filename("Special_Needs_low.ogg", NULL, NULL);
	// stb_vorbis_info info = stb_vorbis_get_info(stream);
	// int samples = stb_vorbis_stream_length_in_samples(stream) * info.channels;
	// int chunk = 65536;
	// pDecodedInterleavedPCMFrames = (drmp3_int16 *)malloc(samples * info.channels * sizeof(drmp3_int16));
	// stb_vorbis_get_samples_short_interleaved(stream, info.channels, pDecodedInterleavedPCMFrames, chunk * 10);
	// // int frame = stb_vorbis_decode_filename("Special_Needs_low.ogg", &channels, &sample_rate, &data);
	// Debug::Print("DATA" + std::to_string(info.channels) + " " + std::to_string(info.sample_rate) + " " + " " + std::to_string(samples));
	// for (int i = 0; i < chunk; i++)
	// {
	// 	pDecodedInterleavedPCMFrames[i] = data[i];
	// }
}

AudioClipStream::~AudioClipStream()
{
	if (type == AudioType::Mp3)
	{
		drmp3_uninit(mp3);
		delete mp3;
	}
	else if (type == AudioType::Wav)
	{
		drwav_uninit(wav);
		delete wav;
	}
}

void AudioClipStream::FillBuffer(int size, int bufferOffset, short* buff)
{
	if (type == AudioType::Mp3)
		drmp3_read_pcm_frames_s16(mp3, size, buff + (bufferOffset));
	else if (type == AudioType::Wav)
		drwav_read_pcm_frames_s16(wav, size, buff + (bufferOffset));
}

int AudioClipStream::GetFrequency() const
{
	int rate = 0;
	if (type == AudioType::Mp3)
		rate = mp3->sampleRate;
	else if (type == AudioType::Wav)
		rate = wav->sampleRate;

	return rate;
}

int64_t AudioClipStream::GetSampleCount() const
{
	return sampleCount;
}

int64_t AudioClipStream::GetSeekPosition() const
{
	uint64_t seekPos = 0;
	if (type == AudioType::Mp3)
		seekPos = mp3->currentPCMFrame;
	else if (type == AudioType::Wav)
		seekPos = wav->readCursorInPCMFrames;

	return seekPos;
}

void AudioClipStream::ResetSeek()
{
	if (type == AudioType::Mp3)
		drmp3_seek_to_pcm_frame(mp3, 0);
	else if (type == AudioType::Wav)
		drwav_seek_to_pcm_frame(wav, 0);
}

void AudioClipStream::SetSeek(uint64_t seekPosition)
{
	if (type == AudioType::Mp3)
		drmp3_seek_to_pcm_frame(mp3, seekPosition);
	else if (type == AudioType::Wav)
		drwav_seek_to_pcm_frame(wav, seekPosition);
}
