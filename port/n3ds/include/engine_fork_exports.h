/* The FractumSeraph/OpenCE fork's engine functions that Halo3DS's native
 * code calls, which the fork keeps static or no longer declares: public on
 * the 3DS (the fork's units, under HALO_N3DS), declared here (engine_prefix.h). */
#ifndef HALO_N3DS_ENGINE_FORK_EXPORTS_H
#define HALO_N3DS_ENGINE_FORK_EXPORTS_H

struct bitmap_data;
struct parse_string_state;
struct font_header;
struct font_character;
struct player_control;
struct file_reference;
struct file_last_modification_date;

void rasterizer_sort_external(void);
struct bitmap_data *hardware_character_cache_get_bitmap(void);
void rasterizer_draw_character(struct parse_string_state *state, struct font_header *font,
	struct font_character *font_character, unsigned long color, short x0, short y0, short x, short y,
	short dx, short dy);
struct player_control *player_control_get(short local_player_index);
unsigned char player_control_camera_control_is_active(void);
unsigned char file_rename(struct file_reference *file, const char *new_name);
unsigned char file_get_last_modification_date(struct file_reference *file,
	struct file_last_modification_date *date);

/* (the fork's 32-bit builds call these undeclared: ui_widget.c) */
struct widget_instance;
struct event_record;
unsigned char ui_widget_event_handler_function_invoke(struct widget_instance *widget,
	struct event_record *event, unsigned short function_index, unsigned char *widget_deleted);
void reset_last_player1_profile_index(void);
/* (rasterizer_environment.h's, which Halo3DS's code takes from rasterizer.h) */
void rasterizer_environment_diffuse_textures_begin(void);

/* (halo_map_families.h's, which declares it only with HALO_CUSTOM_EDITION:
 * the fork's network test calls it unguarded; this port finds no Custom
 * Edition maps until Phase 4, engine_fork_stubs.c) */
#ifndef HALO_CUSTOM_EDITION
unsigned char map_family_find(short family, char const *file, char *path, long size);
#endif

#endif
