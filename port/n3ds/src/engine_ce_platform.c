/* What the FractumSeraph/OpenCE fork's Custom Edition code asks of its
 * platform layer (port/linux/src/xbox_memory.c, xbox_files.c, d3d8_*.c),
 * on the 3DS: whether Halo PC maps' tag cache can be had (the New 3DS's
 * memory holds it: engine_ce_cache.c), the room for converted models beyond
 * it, the physical memory converted vertices go in (the console's heap), and
 * Direct3D's resource registration (none: Halo3DS's renderer reads the
 * vertex data itself). */
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

/* (xbox_memory.c's; set on a New 3DS at start-up: engine_main.c) */
int platform_ce_tag_cache_ready = 0;
char platform_ce_tag_cache_error[160] = "Halo PC maps need a New 3DS";
unsigned int platform_ce_conversion_room = 0;

/* (xbox_files.c's: paths.custom_edition, a Halo PC install's maps folder:
 * none on the console; the maps are in the data folder's custom_maps or
 * maps_ce: map_families.c) */
const char *platform_custom_edition_root(void)
{
    return "";
}

/* (the SDK's: contiguous physical memory, here the heap, page-aligned) */
void *XPhysicalAlloc(unsigned long size, unsigned long highest, unsigned long alignment,
    unsigned long protect)
{
    void *memory;

    (void)highest;
    (void)protect;
    if (alignment < 16)
        alignment = 16;
    memory = memalign(alignment > 4096 ? alignment : 4096, size ? size : 1);
    return memory;
}

void XPhysicalFree(void *memory)
{
    free(memory);
}

/* (Direct3D's: a resource's registration with its data; nothing here) */
void D3DResource_Register(void *resource, void *data)
{
    (void)resource;
    (void)data;
}

/* (xbox_texture_cache.c's, for the menus' map pictures, ui_map_list.c:
 * Halo3DS draws them its own way; no Direct3D format) */
long bitmap_format_to_d3d_format(short format, unsigned short flags)
{
    (void)format;
    (void)flags;
    return 0;
}

/* (port/linux/game/mods.c's: no mods on the 3DS) */
void mods_enable(unsigned char enabled)
{
    (void)enabled;
}
void mods_list(void)
{
}
unsigned char mods_memory_contains(unsigned long address, long size)
{
    (void)address;
    (void)size;
    return 0;
}
void mods_reload(void)
{
}
