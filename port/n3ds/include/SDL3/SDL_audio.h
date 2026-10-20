/*
SDL3/SDL_AUDIO.H (the Nintendo 3DS's stand-in): the audio stream the sound
mixer plays through (dsound_sdl.c), over the 3DS's DSP
(port/3ds/src/n3ds_sdl.c). SDL3/SDL.h says what this is.
*/

#ifndef __HALO_N3DS_SDL_AUDIO_H
#define __HALO_N3DS_SDL_AUDIO_H

#include <SDL3/SDL_stdinc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Uint32 SDL_AudioDeviceID;

#define SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK ((SDL_AudioDeviceID)0xFFFFFFFFu)
#define SDL_AUDIO_DEVICE_DEFAULT_RECORDING ((SDL_AudioDeviceID)0xFFFFFFFEu)

typedef enum SDL_AudioFormat
{
	SDL_AUDIO_UNKNOWN = 0x0000u,
	SDL_AUDIO_S16 = 0x8010u,
	SDL_AUDIO_F32 = 0x8120u
} SDL_AudioFormat;

typedef struct SDL_AudioSpec
{
	SDL_AudioFormat format;
	int channels;
	int freq;
} SDL_AudioSpec;

typedef struct SDL_AudioStream SDL_AudioStream;

/* the device asks the callback for additional_amount bytes (of total_amount
it holds), which the callback gives it with SDL_PutAudioStreamData */
typedef void (SDLCALL *SDL_AudioStreamCallback)(void *userdata, SDL_AudioStream *stream, int additional_amount,
	int total_amount);

SDL_AudioStream *SDL_OpenAudioDeviceStream(SDL_AudioDeviceID devid, const SDL_AudioSpec *spec,
	SDL_AudioStreamCallback callback, void *userdata);
bool SDL_PutAudioStreamData(SDL_AudioStream *stream, const void *buf, int len);
bool SDL_ResumeAudioStreamDevice(SDL_AudioStream *stream);
void SDL_DestroyAudioStream(SDL_AudioStream *stream);

#ifdef __cplusplus
}
#endif

#endif
