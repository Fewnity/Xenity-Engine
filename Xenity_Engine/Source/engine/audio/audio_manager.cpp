#include "audio_manager.h"
#include "../../xenity.h"

#ifdef __PSP__
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspkernel.h>
#include <psppower.h>
#endif

#ifdef __vita__
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#endif

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#include "stb_vorbis.c"

drmp3 mp3;
short *pDecodedInterleavedPCMFrames;
int seekPosition = 0;
bool needNewReed = false;
bool needNewReed2 = false;

#if defined(__vita__)
int size = 2048;
int freq = 7;
int mode = SCE_AUDIO_OUT_MODE_STEREO;
int vol = SCE_AUDIO_VOLUME_0DB;
int port;
#endif

typedef struct
{
    short l, r;
} sample_t;

int buffSize = 1024 * 16;

void audioCallback(void *buf, unsigned int length, void *userdata)
{
    sample_t *ubuf = (sample_t *)buf;

    for (int i = 0; i < length; i++)
    {
        ubuf[i].l = pDecodedInterleavedPCMFrames[seekPosition];
        seekPosition++;
        ubuf[i].r = pDecodedInterleavedPCMFrames[seekPosition];
        seekPosition++;

        if (seekPosition == buffSize / 2)
        {
            needNewReed = true;
        }
        else if (seekPosition == buffSize)
        {
            seekPosition = 0;
            needNewReed2 = true;
        }
    }
}
#if defined(__PSP__) || defined(__vita__)
int audio_thread(SceSize args, void *argp)
{
    while (true)
    {
#if defined(__PSP__)
        if (needNewReed)
        {
            drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, buffSize / 4, pDecodedInterleavedPCMFrames);
            needNewReed = false;
        }
        else if (needNewReed2)
        {
            drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, buffSize / 4, pDecodedInterleavedPCMFrames + (buffSize / 2));
            needNewReed2 = false;
        }
        sceKernelDelayThread(10);
#elif defined(__vita__)
        if (sceAudioOutGetRestSample(port) == 0)
        {
            int16_t wave_buf[SCE_AUDIO_MAX_LEN] = {0};

            for (int i = 0; i < size; i++)
            {
                wave_buf[i * 2] = pDecodedInterleavedPCMFrames[seekPosition];
                seekPosition++;
                wave_buf[1 + i * 2] = pDecodedInterleavedPCMFrames[seekPosition];
                seekPosition++;
                if (seekPosition == buffSize / 2)
                {
                    needNewReed = true;
                }
                else if (seekPosition == buffSize)
                {
                    seekPosition = 0;
                    needNewReed2 = true;
                }
            }
            sceAudioOutOutput(port, wave_buf);
        }
#endif
    }
}

int audio_thread2(SceSize args, void *argp)
{
    while (true)
    {
#if defined(__vita__)
        if (needNewReed)
        {
            drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, buffSize / 4, pDecodedInterleavedPCMFrames);
            needNewReed = false;
        }
        else if (needNewReed2)
        {
            drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, buffSize / 4, pDecodedInterleavedPCMFrames + (buffSize / 2));
            needNewReed2 = false;
        }
#endif
    }
}
#endif

void AudioManager::Update()
{
}

void AudioManager::Init()
{
    std::string filePath = "";
#if defined(__vita__)
    filePath += "ux0:";
#endif
    filePath += "Special_Needs_low.mp3";
    if (!drmp3_init_file(&mp3, filePath.c_str(), NULL))
    {
        // Failed to open file
        Debug::Print("AUDIO ERROR");
    }
    // int frameCount = drmp3_get_pcm_frame_count(&mp3);
    //  pDecodedInterleavedPCMFrames = (drmp3_int16 *)malloc(frameCount * mp3.channels * sizeof(drmp3_int16));
    pDecodedInterleavedPCMFrames = (short *)calloc(buffSize / 2 * mp3.channels, sizeof(short));
    drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, buffSize / 2, pDecodedInterleavedPCMFrames);

    // drwav wav;
    // if (!drwav_init_file(&wav, "Special_Needs.wav", NULL))
    // {
    // 	Debug::Print("AUDIO ERROR");
    // 	// Error opening WAV file.
    // }
    // pDecodedInterleavedPCMFrames = (drwav_int16 *)malloc(wav.totalPCMFrameCount * wav.channels * sizeof(drwav_int16));
    // size_t numberOfSamplesActuallyDecoded = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pDecodedInterleavedPCMFrames);

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
    // if (data)
    // {
    // }
    // else
    // {
    // 	Debug::Print("NO DATA");
    // }
    // for (int i = 0; i < chunk; i++)
    // {
    // 	pDecodedInterleavedPCMFrames[i] = data[i];
    // }

#if defined(__PSP__)
    pspAudioInit();
    pspAudioSetChannelCallback(0, audioCallback, NULL);
    // pspAudioSetVolume(0, 0x8000, 0x8000);
    SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x18, 0x10000, 0, NULL);
    if (thd_id >= 0)
    {
        sceKernelStartThread(thd_id, 0, 0);
    }
#elif defined(__vita__)
    int freqs[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
    port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, size, freqs[freq], (SceAudioOutMode)mode);
    int volA[2] = {vol, vol};
    sceAudioOutSetVolume(port, (SceAudioOutChannelFlag)(SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), volA);
    SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x400000, 0, 0, NULL);
    SceUID thd_id2 = sceKernelCreateThread("audio_thread2", audio_thread2, 0x40, 0x400000, 0, 0, NULL);
    if (thd_id >= 0 && thd_id2 >= 0)
    {
        sceKernelStartThread(thd_id2, 0, 0);
        sceKernelStartThread(thd_id, 0, 0);
    }
#endif
}