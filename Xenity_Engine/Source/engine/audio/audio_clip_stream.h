// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <dr_mp3.h>
#include <dr_wav.h>

#include <string>
#include <memory>

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
	~AudioClipStream();

	/**
	* @brief Open audio clip stream
	* @param audioFile AudioClip to open
	*/
	void OpenStream(std::shared_ptr<AudioClip> audioFile);

	/**
	* @brief Fill the given audio buffer
	* @param amount Amount to fill
	* @param offset Offset in the buffer
	* @param buff Buffer to fill
	*/
	void FillBuffer(int amount, int offset, short* buff);

	/**
	* @brief Get audio clip frequency in Hz
	*/
	int GetFrequency() const;

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
	inline int GetChannelCount() const
	{
		return channelCount;
	}

	/**
	* @brief Get audio type (Mp3, Wav...)
	*/
	inline AudioType GetAudioType() const
	{
		return type;
	}

private:
	int64_t sampleCount = 0;
	drmp3* mp3 = nullptr;
	drwav* wav = nullptr;
	AudioType type = AudioType::Null;
	int channelCount = 0;
};