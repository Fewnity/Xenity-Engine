// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <cstdint>
#include <dr_mp3.h>
#include <dr_wav.h>

class Channel;
class File;
class AudioClip;

enum class AudioType
{
	Null,
	Wav,
	Mp3,
};

class AudioClipStream
{
public:
	AudioClipStream();
	~AudioClipStream();
	AudioClipStream(const AudioClipStream& other) = delete;
	AudioClipStream& operator=(const AudioClipStream&) = delete;

	/**
	* @brief Open audio clip stream
	* @param audioFile AudioClip to open
	*/
	void OpenStream(const AudioClip& audioFile);

	/**
	* @brief Fill the given audio buffer
	* @param amount Amount to fill
	* @param offset Offset in the buffer
	* @param buff Buffer to fill
	*/
	void FillBuffer(uint64_t amount, short* buff);

	/**
	* @brief Get audio clip frequency in Hz
	*/
	uint32_t GetFrequency() const;

	/**
	* @brief Get audio clip sample count
	*/
	uint64_t GetSampleCount() const;

	/**
	* @brief Reset seek position
	*/
	void ResetSeek();

	/**
	* @brief Set seek position
	* @param seekPosition Seek position
	*/
	void SetSeek(uint64_t seekPosition);

	/**
	* @brief Get seek position
	*/
	uint64_t GetSeekPosition() const;

	/**
	* @brief Get channel count
	*/
	inline uint32_t GetChannelCount() const
	{
		return m_channelCount;
	}

	/**
	* @brief Get audio type (Mp3, Wav...)
	*/
	inline AudioType GetAudioType() const
	{
		return m_type;
	}

private:
	uint64_t m_sampleCount = 0;
	drmp3* m_mp3Stream = nullptr;
	drwav* m_wavStream = nullptr;
	AudioType m_type = AudioType::Null;
	uint32_t m_channelCount = 0;
};