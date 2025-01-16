// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine
#include "audio_clip_stream.h"

#include <memory>
//#include <stb_vorbis.c>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

#include <engine/debug/debug.h>
#include <engine/file_system/file.h>
#include "audio_clip.h"
#include <engine/assertions/assertions.h>


void AudioClipStream::OpenStream(const AudioClip& audioFile)
{
	const std::shared_ptr<File>& file = audioFile.m_file;
	const std::string& path = file->GetPath();
	//Debug::Print("Loading audio clip: " + path, true);

	// To lower extention
	std::string lowerExt = file->GetFileExtension().substr(1);
	const size_t pathSize = lowerExt.size();
	for (size_t i = 0; i < pathSize; i++)
	{
		lowerExt[i] = tolower(lowerExt[i]);
	}

	const AudioClip::AudioMemory& audioMemory = audioFile.GetAudioMemory();
	if (lowerExt == "wav")
	{
		m_wavStream = new drwav();
		if ((audioFile.IsStoredInMemory() && !drwav_init_memory(m_wavStream, audioMemory.m_data, audioMemory.m_dataLength, NULL)) || (!audioFile.IsStoredInMemory() && !drwav_init_file(m_wavStream, path.c_str(), NULL)))
		{
			// Error opening WAV file.
			Debug::PrintError("[AudioClipStream::OpenStream] Cannot init wav file: " + path, true);
			delete m_wavStream;
		}
		else
		{
			m_type = AudioType::Wav;
			// Get informations
			m_channelCount = m_wavStream->channels;
			m_sampleCount = m_wavStream->totalPCMFrameCount;
			//Debug::Print("Audio clip data: " + std::to_string(m_wavStream->channels) + " sampleRate: " + std::to_string(m_wavStream->sampleRate) + " m_sampleCount: "+ std::to_string(m_sampleCount), true);
		}
	}
	else if (lowerExt == "mp3")
	{
		m_mp3Stream = new drmp3();
		if ((audioFile.IsStoredInMemory() && !drmp3_init_memory(m_mp3Stream, audioMemory.m_data, audioMemory.m_dataLength, NULL)) || (!audioFile.IsStoredInMemory() && !drmp3_init_file(m_mp3Stream, path.c_str(), NULL)))
		{
			// Error opening MP3 file.
			Debug::PrintError("[AudioClipStream::OpenStream] Cannot init mp3 file: " + path, true);
			delete m_mp3Stream;
		}
		else
		{
			m_type = AudioType::Mp3;
			// Get informations
			m_channelCount = m_mp3Stream->channels;
			m_sampleCount = drmp3_get_pcm_frame_count(m_mp3Stream);
			//Debug::Print("Audio clip data: " + std::to_string(m_mp3Stream->channels) + " sampleRate: " + std::to_string(m_mp3Stream->sampleRate) + " m_sampleCount: " + std::to_string(m_sampleCount), true);
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

AudioClipStream::AudioClipStream()
{
}

AudioClipStream::~AudioClipStream()
{
	if (m_type == AudioType::Mp3)
	{
		drmp3_uninit(m_mp3Stream);
		delete m_mp3Stream;
	}
	else if (m_type == AudioType::Wav)
	{
		drwav_uninit(m_wavStream);
		delete m_wavStream;
	}
}

uint64_t AudioClipStream::FillBuffer(uint64_t amount, short* buff, bool loop)
{
	uint64_t remainingFrames = amount;
	uint64_t tempFrameReadCount = 0;
	uint32_t loopCount = 0;
	while (remainingFrames != 0)
	{
		loopCount++;
		if (m_type == AudioType::Mp3)
		{
			tempFrameReadCount = drmp3_read_pcm_frames_s16(m_mp3Stream, remainingFrames, buff + (amount - remainingFrames));
		}
		else if (m_type == AudioType::Wav)
		{	
			tempFrameReadCount = drwav_read_pcm_frames_s16(m_wavStream, remainingFrames, buff + (amount - remainingFrames));
		}

		// If the stream ends and not looping stop the stream
		if (!loop)
		{
			break;
		}

		// If stream reaches the end, reset the seek and continue to read
		if (tempFrameReadCount != remainingFrames)
		{
			ResetSeek();
		}
		remainingFrames -= tempFrameReadCount;
	}

	if (loopCount > 1)
	{
		return GetSeekPosition(); // Return the new seek position
	}
	else
	{
		return 0;
	}
}

uint32_t AudioClipStream::GetFrequency() const
{
	uint32_t rate = 0;
	if (m_type == AudioType::Mp3)
	{	
		rate = m_mp3Stream->sampleRate;
	}
	else if (m_type == AudioType::Wav)
	{
		rate = m_wavStream->sampleRate;
	}

	return rate;
}

uint64_t AudioClipStream::GetSampleCount() const
{
	return m_sampleCount;
}

uint64_t AudioClipStream::GetSeekPosition() const
{
	uint64_t seekPos = 0;
	if (m_type == AudioType::Mp3)
	{	
		seekPos = m_mp3Stream->currentPCMFrame;
	}
	else if (m_type == AudioType::Wav)
	{	
		seekPos = m_wavStream->readCursorInPCMFrames;
	}

	return seekPos;
}

void AudioClipStream::ResetSeek()
{
	// Move cursor beginning of the file
	SetSeek(0);
}

void AudioClipStream::SetSeek(uint64_t seekPosition)
{
	// Move cursor to the new seek position
	if (m_type == AudioType::Mp3)
	{	
		drmp3_seek_to_pcm_frame(m_mp3Stream, seekPosition);
	}
	else if (m_type == AudioType::Wav)
	{	
		drwav_seek_to_pcm_frame(m_wavStream, seekPosition);
	}
}
