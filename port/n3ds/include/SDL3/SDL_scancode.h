/*
SDL3/SDL_SCANCODE.H (the Nintendo 3DS's stand-in): the keyboard's size, which
sdl_platform.h sizes its key state by (SDL3/SDL.h says what this is). The
3DS has no keyboard; the scancodes themselves are not named here.
*/

#ifndef __HALO_N3DS_SDL_SCANCODE_H
#define __HALO_N3DS_SDL_SCANCODE_H

typedef enum SDL_Scancode
{
	SDL_SCANCODE_UNKNOWN = 0,
	SDL_SCANCODE_COUNT = 512
} SDL_Scancode;

#endif
