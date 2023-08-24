#include "audio_clip_stream.h"
#include "../../xenity.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include "stb_vorbis.c"

void AudioClipStream::OpenStream(std::string filePath)
{
#if defined(__vita__)
    filePath = "ux0:" + filePath;
#endif
    Debug::Print("Loading audio clip: " + filePath);
    if (filePath.substr(filePath.size() - 3) == "wav")
    {
        if (!drwav_init_file(&wav, filePath.c_str(), NULL))
        {
            // Error opening WAV file.
            Debug::PrintError("AUDIO ERROR" + filePath);
        }
        else
        {
            type = Wav;
            Debug::Print("Audio clip data: " + std::to_string(wav.channels) + " " + std::to_string(wav.sampleRate));
        }
    }
    else if (filePath.substr(filePath.size() - 3) == "mp3")
    {
        if (!drmp3_init_file(&mp3, filePath.c_str(), NULL))
        {
            // Error opening MP3 file.
            Debug::PrintError("AUDIO ERROR: " + filePath);
        }
        else
        {
            type = Mp3;
            Debug::Print("Audio clip data: " + std::to_string(mp3.channels) + " " + std::to_string(mp3.sampleRate));
        }
    }

    if (type == Mp3)
        sampleCount = drmp3_get_pcm_frame_count(&mp3);
    else if (type == Wav)
        sampleCount = wav.totalPCMFrameCount;

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
    if (type == Mp3)
        drmp3_uninit(&mp3);
    else if (type == Wav)
        drwav_uninit(&wav);
}

void AudioClipStream::FillBuffer(int size, int bufferOffset, short *buff)
{
    if (type == Mp3)
        drmp3_read_pcm_frames_s16(&mp3, size, buff + (bufferOffset));
    else if (type == Wav)
        drwav_read_pcm_frames_s16(&wav, size, buff + (bufferOffset));
}

int AudioClipStream::GetFrequency()
{
    int rate = 0;
    if (type == Mp3)
        rate = mp3.sampleRate;
    else if (type == Wav)
        rate = wav.sampleRate;

    return rate;
}

int64_t AudioClipStream::GetSampleCount()
{
    return sampleCount;
}

int64_t AudioClipStream::GetSeekPosition()
{
    uint64_t seekPos = 0;
    if (type == Mp3)
        seekPos = mp3.currentPCMFrame;
    else if (type == Wav)
        seekPos = wav.readCursorInPCMFrames;

    return seekPos;
}

void AudioClipStream::ResetSeek()
{
    if (type == Mp3)
        drmp3_seek_to_pcm_frame(&mp3, 0);
    else if (type == Wav)
        drwav_seek_to_pcm_frame(&wav, 0);
}