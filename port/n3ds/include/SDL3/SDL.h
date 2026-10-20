/*
SDL3/SDL.H (the Nintendo 3DS's stand-in)

The 3DS build has no SDL. A few of the platform units it keeps from the
Linux build call SDL for small things: the sound mixer's audio stream
(dsound_sdl.c), the settings' and mods' file helpers (port_config.c,
port_mods.c, menu_files.c). This header declares that subset, with SDL3's
own names and meanings, and port/n3ds/src/engine_fork_sdl.c implements what this port uses of it over
newlib and libctru, so those units compile unchanged. Nothing else of SDL
is here: a unit that needs more is one the 3DS build should not keep
(the fork's port/3ds; here, port/n3ds/sources.json).
*/

#ifndef __HALO_N3DS_SDL_H
#define __HALO_N3DS_SDL_H

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_audio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- errors and hints */

const char *SDL_GetError(void);
bool SDL_SetHint(const char *name, const char *value);
#define SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES "SDL_AUDIO_DEVICE_SAMPLE_FRAMES"

/* ---------- time */

Uint64 SDL_GetTicks(void);

/* ---------- files and folders (SDL_filesystem.h, SDL_iostream.h) */

const char *SDL_GetBasePath(void);
bool SDL_CreateDirectory(const char *path);

typedef enum SDL_PathType
{
	SDL_PATHTYPE_NONE,
	SDL_PATHTYPE_FILE,
	SDL_PATHTYPE_DIRECTORY,
	SDL_PATHTYPE_OTHER
} SDL_PathType;

typedef struct SDL_PathInfo
{
	SDL_PathType type;
	Uint64 size;
	/* nanoseconds since the Unix epoch */
	Sint64 create_time;
	Sint64 modify_time;
	Sint64 access_time;
} SDL_PathInfo;

bool SDL_GetPathInfo(const char *path, SDL_PathInfo *info);
bool SDL_RenamePath(const char *oldpath, const char *newpath);
bool SDL_RemovePath(const char *path);
/* the names in path that match pattern (SDL's glob: * and ?), with
flags 0, NULL-terminated, in one block to SDL_free; count receives how many */
char **SDL_GlobDirectory(const char *path, const char *pattern, Uint32 flags, int *count);

/* a file's bytes, in a block to SDL_free, with a terminating zero after
them; datasize receives their count */
void *SDL_LoadFile(const char *file, size_t *datasize);
bool SDL_SaveFile(const char *file, const void *data, size_t datasize);

typedef struct SDL_IOStream SDL_IOStream;

typedef enum SDL_IOWhence
{
	SDL_IO_SEEK_SET,
	SDL_IO_SEEK_CUR,
	SDL_IO_SEEK_END
} SDL_IOWhence;

SDL_IOStream *SDL_IOFromFile(const char *file, const char *mode);
Sint64 SDL_SeekIO(SDL_IOStream *context, Sint64 offset, SDL_IOWhence whence);
size_t SDL_ReadIO(SDL_IOStream *context, void *ptr, size_t size);
bool SDL_CloseIO(SDL_IOStream *context);

#ifdef __cplusplus
}
#endif

#endif
