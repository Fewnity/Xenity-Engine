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
	* Open audio clip stream
	* @param file File
	*/
	void OpenStream(std::shared_ptr<File> audioFile);

	/**
	* Fill audio buffer
	* @param size Buffer size
	* @param bufferOffset Buffer offset
	* @param buff Buffer
	*/
	void FillBuffer(int size, int bufferOffset, short* buff);

	/**
	* Get audio clip frequency
	*/
	int GetFrequency() const;

	/**
	* Get audio clip sample count
	*/
	int64_t GetSampleCount() const;

	/**
	* Reset seek position
	*/
	void ResetSeek();

	/**
	* Set seek position
	* @param seekPosition Seek position
	*/
	void SetSeek(uint64_t seekPosition);

	/**
	* Get seek position
	*/
	int64_t GetSeekPosition() const;

	/**
	* Get channel count
	*/
	int GetChannelCount() const
	{
		return channelCount;
	}

	/**
	* Get audio type
	*/
	AudioType GetAudioType() const
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