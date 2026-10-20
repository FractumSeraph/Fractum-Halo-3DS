#ifndef HALO_N3DS_ENGINE_CACHE_H
#define HALO_N3DS_ENGINE_CACHE_H
struct n3ds_cache_view;
struct scenario_structure_bsp_reference;
long n3ds_engine_tags_load(const char *name);
/* Validate prepared SD tags/relocations without binding or changing live state.
 * BSP payloads continue to be validated by their loader when requested. */
boolean n3ds_engine_map_available(const char *name);
/* Lightweight installed-header check for lists; not full load validation. */
boolean n3ds_engine_map_installed(const char *name);
void n3ds_engine_tags_unload(void);
boolean n3ds_engine_bsp_load(struct scenario_structure_bsp_reference *reference);
void n3ds_engine_bsp_unload(struct scenario_structure_bsp_reference *reference);
boolean n3ds_engine_cache_read(unsigned long offset, unsigned long bytes, void *destination);
/* A bitmap's pixels: the map's, or (a Halo PC map's) the resource map its
 * tag's are in (bitmaps.map). */
struct bitmap_data;
struct sound_permutation;
/* ... and a sound's samples, the same way (sounds.map, or the decoded
 * Ogg Vorbis sounds' file). */
boolean n3ds_engine_sound_read(const struct sound_permutation *permutation, unsigned long offset,
    unsigned long bytes, void *destination);
boolean n3ds_engine_bitmap_read(const struct bitmap_data *bitmap, unsigned long offset, unsigned long bytes,
    void *destination);
int n3ds_engine_cache_stream_tests(void);
int n3ds_engine_cache_stream_live(void);
/* Read-only diagnostics for bounds/lifetime verification. */
const struct n3ds_cache_view *n3ds_engine_cache_view(int bsp);
unsigned int n3ds_engine_cache_generation(void);
unsigned int n3ds_engine_tag_generation(void);
int halo_engine_relocation_tests(void);
#endif
