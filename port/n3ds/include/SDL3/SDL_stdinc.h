/*
SDL3/SDL_STDINC.H (the Nintendo 3DS's stand-in): SDL's integer types and
the string helpers the kept units use (SDL3/SDL.h says what this is).
*/

#ifndef __HALO_N3DS_SDL_STDINC_H
#define __HALO_N3DS_SDL_STDINC_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t Sint8;
typedef uint8_t Uint8;
typedef int16_t Sint16;
typedef uint16_t Uint16;
typedef int32_t Sint32;
typedef uint32_t Uint32;
typedef int64_t Sint64;
typedef uint64_t Uint64;

#define SDLCALL

void SDL_free(void *mem);
int SDL_strncasecmp(const char *str1, const char *str2, size_t maxlen);

#ifdef __cplusplus
}
#endif

#endif
