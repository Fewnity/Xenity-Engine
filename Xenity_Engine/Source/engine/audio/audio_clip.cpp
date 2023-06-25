#include "audio_clip.h"
#include "../../xenity.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include "stb_vorbis.c"

AudioClip::AudioClip(std::string filePath)
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
            Debug::Print("AUDIO ERROR" + filePath);
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
            Debug::Print("AUDIO ERROR: " + filePath);
        }
        else
        {
            type = Mp3;
            Debug::Print("Audio clip data: " + std::to_string(mp3.channels) + " " + std::to_string(mp3.sampleRate));
        }
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

AudioClip::~AudioClip()
{
    if (type == Mp3)
        drmp3_uninit(&mp3);
    else if (type == Wav)
        drwav_uninit(&wav);
}

void AudioClip::FillBuffer(int size, int bufferOffset, short *buff)
{
    if (type == Mp3)
        drmp3_read_pcm_frames_s16(&mp3, size, buff + (bufferOffset));
    else if (type == Wav)
        drwav_read_pcm_frames_s16(&wav, size, buff + (bufferOffset));
}

short *AudioClip::GetBuffer(int size)
{
    short *buff = (short *)malloc(size * sizeof(short));
    if (type == Mp3)
        drmp3_read_pcm_frames_s16(&mp3, size, buff);
    else if (type == Wav)
        drwav_read_pcm_frames_s16(&wav, size, buff);

    return buff;
}

int AudioClip::GetFrenquency()
{
    int rate = 0;
    if (type == Mp3)
        rate = mp3.sampleRate;
    else if (type == Wav)
        rate = wav.sampleRate;

    return rate;
}