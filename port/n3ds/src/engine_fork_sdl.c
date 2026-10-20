/* The few SDL functions the FractumSeraph/OpenCE fork's settings unit
 * (port/linux/src/port_config.c) calls, over newlib, with SDL3's names and
 * meanings (port/n3ds/include/SDL3/SDL.h): the fork's own 3DS port's
 * (port/3ds/src/n3ds_posix_sdl.c), for this port's folder. */
#include <SDL3/SDL.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

static char sdl_error[128];

static void set_error(const char *what)
{
    snprintf(sdl_error, sizeof(sdl_error), "%s: %s", what, strerror(errno));
}

const char *SDL_GetError(void)
{
    return sdl_error;
}

/* (the folder of this port's data on the SD card, which Halo3DS's setup
 * fills: its settings file is there, config.toml) */
const char *SDL_GetBasePath(void)
{
    return "sdmc:/halo-source/";
}

bool SDL_CreateDirectory(const char *path)
{
    /* every folder on the way, as SDL makes them */
    char copy[1024];
    size_t index;

    snprintf(copy, sizeof(copy), "%s", path);
    for (index = 1; copy[index]; index++)
    {
        if (copy[index] == '/')
        {
            copy[index] = 0;
            if (strchr(copy, ':') != copy + strlen(copy) - 1)
                mkdir(copy, 0777);
            copy[index] = '/';
        }
    }
    if (mkdir(copy, 0777) != 0 && errno != EEXIST)
    {
        set_error("mkdir");
        return false;
    }
    return true;
}

bool SDL_GetPathInfo(const char *path, SDL_PathInfo *info)
{
    struct stat st;

    if (stat(path, &st) != 0)
    {
        set_error("stat");
        if (info)
            memset(info, 0, sizeof(*info));
        return false;
    }
    if (info)
    {
        info->type = S_ISDIR(st.st_mode) ? SDL_PATHTYPE_DIRECTORY : S_ISREG(st.st_mode) ? SDL_PATHTYPE_FILE :
            SDL_PATHTYPE_OTHER;
        info->size = (Uint64)st.st_size;
        info->create_time = (Sint64)st.st_ctime * 1000000000LL;
        info->modify_time = (Sint64)st.st_mtime * 1000000000LL;
        info->access_time = (Sint64)st.st_atime * 1000000000LL;
    }
    return true;
}

void *SDL_LoadFile(const char *file, size_t *datasize)
{
    FILE *handle = fopen(file, "rb");
    long size;
    char *data;

    if (datasize)
        *datasize = 0;
    if (!handle)
    {
        set_error("fopen");
        return NULL;
    }
    if (fseek(handle, 0, SEEK_END) != 0 || (size = ftell(handle)) < 0 || fseek(handle, 0, SEEK_SET) != 0)
    {
        set_error("fseek");
        fclose(handle);
        return NULL;
    }
    data = malloc((size_t)size + 1);
    if (!data)
    {
        set_error("malloc");
        fclose(handle);
        return NULL;
    }
    if (fread(data, 1, (size_t)size, handle) != (size_t)size)
    {
        set_error("fread");
        free(data);
        fclose(handle);
        return NULL;
    }
    fclose(handle);
    data[size] = 0;
    if (datasize)
        *datasize = (size_t)size;
    return data;
}

bool SDL_SaveFile(const char *file, const void *data, size_t datasize)
{
    FILE *handle = fopen(file, "wb");
    bool written;

    if (!handle)
    {
        set_error("fopen");
        return false;
    }
    written = fwrite(data, 1, datasize, handle) == datasize;
    if (!written)
        set_error("fwrite");
    fclose(handle);
    return written;
}

void SDL_free(void *mem)
{
    free(mem);
}

int SDL_strncasecmp(const char *str1, const char *str2, size_t maxlen)
{
    return strncasecmp(str1, str2, maxlen);
}
