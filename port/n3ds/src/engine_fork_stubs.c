/* Stand-ins for the FractumSeraph/OpenCE fork's port layer (port/linux),
 * which its engine calls, until its units are built for the 3DS: generated
 * from the fork's declarations (port/n3ds/tools-local/gen_stubs.py); each does
 * nothing and returns zero. Phase 1 of docs/MULTIPLAYER-PLAN.md. */
#include "cseries.h"
#include "math/real_math.h"
#include "math/integer_math.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>

void posix_random_bytes(void *buffer, unsigned long size); /* (posix.h's posix_ulong) */

/* port/linux/src/build_identity.h */
const char * build_identity(void)
{
	return "Halo3DS multiplayer (FractumSeraph/OpenCE engine)";
}
/* source/cache_files.h */
boolean cache_file_tags_are_ce(void)
{
	return 0;
}
/* port/linux/game/ce_hud.c */
boolean ce_hud_halo_pc_overlay_blend_functions(void)
{
	return 0;
}
/* port/linux/game/ce_hud.c */
boolean ce_hud_overlays_blocked(void)
{
	return 0;
}
/* port/linux/game/download_screen.c */
boolean download_screen_active(void)
{
	return 0;
}
/* port/linux/game/download_screen.c */
void download_screen_process(void)
{
}
/* port/linux/game/download_screen.c */
void download_screen_render(void)
{
}
/* source/sound_dsound_xbox.c */
void dsound_port_set_channel_stereo_position(short virtual_channel_index, boolean positioned, real pan, real distance, real minimum_distance, real distance_fade, real occlusion, real obstruction, boolean attenuate_direct_path)
{
}
/* port/linux/game/first_person_legs.c */
real_matrix4x3 const * first_person_legs_node_matrices(long object_index, real_matrix4x3 const *nodes)
{
	return nodes; /* (as it is: the fork's unit is not built yet) */
}
/* port/linux/game/first_person_legs.c */
void first_person_legs_reset(void)
{
}
/* port/linux/game/first_person_legs.c */
boolean first_person_legs_wanted(long object_index)
{
	return 0;
}
/* port/linux/include/halo_linux_source_fixups.h */
int halo_interpolation_enabled(void)
{
	return 0;
}
/* port/linux/include/halo_keyboard.h */
int halo_keyboard_action_shown(int action, char *shown, unsigned long size)
{
	return 0;
}
/* port/linux/include/halo_keyboard.h */
unsigned long halo_keyboard_actions(short controller_index)
{
	return 0;
}
/* port/linux/include/halo_keyboard.h */
int halo_keyboard_prompts(void)
{
	return 0;
}
/* port/linux/src/xinput_sdl.c */
int halo_linux_mouse_aiming(short gamepad_index)
{
	return 0;
}
/* port/linux/src/xinput_sdl.c */
int halo_linux_mouse_look(short gamepad_index, float *yaw, float *pitch)
{
	return 0;
}
/* port/linux/src/xinput_sdl.c */
int halo_linux_touch_aiming(short gamepad_index)
{
	return 0;
}
/* port/linux/src/xinput_sdl.c */
int halo_linux_touch_look(short gamepad_index, float *yaw, float *pitch, float *gyro_yaw, float *gyro_pitch)
{
	return 0;
}
/* port/linux/src/xinput_sdl.c */
int halo_linux_touch_move(short controller_index, float *forward, float *strafe)
{
	return 0;
}
/* port/linux/include/halo_ui_pointer.h */
int halo_scoreboard_pointer_update(int offered, struct halo_ui_pointer *pointer)
{
	return 0;
}
/* port/linux/include/halo_linux_source_fixups.h */
void halo_screen_ui_offset(unsigned char centered)
{
}
/* port/linux/include/halo_linux_source_fixups.h */
long halo_screen_width(void)
{
	return 640; /* (the Xbox's screen: Halo3DS draws its own) */
}
/* port/linux/include/halo_ui_pointer.h */
int halo_ui_pointer_update(int menus_active, struct halo_ui_pointer *pointer)
{
	return 0;
}
/* port/linux/game/hud_hires_tags.c */
boolean hud_hires_button_draw(struct icon_hud_element_definition const *icon, rectangle2d *cursor, pixel32 color)
{
	return 0;
}
/* port/linux/game/hud_hires_tags.c */
struct bitmap_data const * hud_hires_sprite_bitmap(struct bitmap_data const *bitmap, short sequence_index)
{
	return bitmap; /* (no high-res sprite: the map's own) */
}
/* port/linux/game/hud_hires_tags.c */
void hud_hires_tags_unloaded(void)
{
}
/* port/linux/game/map_share.h */
boolean map_share_downloading(char const *level_name)
{
	return 0;
}
/* port/linux/game/map_share.h */
void map_share_handle_message(long machine_index, word const *message, word size, boolean from_stream)
{
}
/* port/linux/game/map_share.h */
boolean map_share_handles_message(word const *message, word size)
{
	return 0;
}
/* port/linux/game/map_share.h */
boolean map_share_holds_players(void)
{
	return 0;
}
/* port/linux/game/map_share.h */
void map_share_level_name_choose(char *level_name, long size)
{
}
/* port/linux/game/map_share.h */
boolean map_share_machine_downloading(long machine_index)
{
	return 0;
}
/* port/linux/game/map_share.h */
boolean map_share_machine_needs_map(long machine_index)
{
	return 0;
}
/* port/linux/src/map_torrents.h */
void map_torrents_playing(const char *level_name, unsigned long version, int hosting)
{
}
/* port/linux/game/menu_functions.c */
void menu_functions_text_box_drawn(struct widget_instance *widget, rectangle2d const *bounds)
{
}
/* port/linux/game/menu_style.c */
void menu_style_gametype_rules(long profile_index, wchar_t *text, long size)
{
}
/* port/linux/game/network_voice.h */
boolean network_voice_available(void)
{
	return 0;
}
/* port/linux/game/network_voice.h */
void network_voice_draw_icon(rectangle2d const *bounds, boolean muted, real alpha)
{
}
/* port/linux/game/network_voice.h */
void network_voice_handle_message(long machine_index, word const *message, word size)
{
}
/* port/linux/game/network_voice.h */
boolean network_voice_handles_message(word const *message, word size)
{
	return 0;
}
/* port/linux/game/network_voice.h */
void network_voice_machine_joined(long machine_index)
{
}
/* port/linux/game/network_voice.h */
boolean network_voice_machine_muted(long machine_index)
{
	return 0;
}
/* port/linux/game/network_voice.h */
boolean network_voice_machine_speaking(long machine_index)
{
	return 0;
}
/* port/linux/game/network_voice.h */
void network_voice_mute_machine(long machine_index, boolean mute)
{
}
/* port/linux/game/pal_tags.c */
boolean pal_tags_first_person_advance(short local_player_index, long graph_index, short animation_index, short frame_index)
{
	return 1; /* (an NTSC map's animation advances every tick) */
}
/* port/linux/game/pal_tags.c */
real pal_tags_first_person_fraction(short local_player_index, long graph_index, short animation_index, short frame_index)
{
	return 0.0f;
}
/* port/linux/game/pal_tags.c */
short pal_tags_first_person_frames(long graph_index, short animation_index, short frames)
{
	return frames;
}
/* port/linux/game/menu_tags.c */
boolean pc_menu_button_key_cap(long bitmap_tag_index, short x0, short y0, short x1, short y1, float alpha)
{
	return 0;
}
/* port/linux/game/menu_functions.c */
boolean pc_menu_event_function_invoke(struct widget_instance *widget, struct event_record *event, long function_index, boolean *widget_deleted)
{
	return 0;
}
/* port/linux/game/menu_tags.c */
boolean pc_menu_frame_placement(struct bitmap_data const *bitmap, short *x, short *y, short *width, short *height, short *texel_y)
{
	return 0;
}
/* port/linux/game/menu_functions.c */
void pc_menu_game_data_function_invoke(struct widget_instance *widget, long function)
{
}
/* port/linux/game/menu_functions.c */
unsigned char pc_menu_split_players(void)
{
	return 0;
}
/* port/linux/game/menu_tags.c */
boolean pc_menu_tag(long tag_index)
{
	return 0;
}
/* port/linux/game/menu_functions.c */
boolean pc_menu_text_color(struct widget_instance const *widget, real_rgb_color *rgb)
{
	return 0;
}
/* port/linux/game/menu_tags.c */
char const * pc_menus_root_name(void)
{
	return "ui\\shell\\main_menu\\main_menu"; /* (the Xbox's) */
}
/* port/linux/include/halo_linux_source_fixups.h */
char const * pc_menus_screen(char const *name)
{
	return name; /* (as it is: the fork's unit is not built yet) */
}
/* source/physical_memory_map.h */
void * physical_memory_get_tag_cache_base_address(void)
{
	return NULL;
}
/* port/linux/src/platform.h */
void n3ds_log(const char *message);
/* (the fork's platform layer's log lines, to this port's log: the network
   layer's among them) */
void platform_log(const char *format, ...)
{
	char text[512];
	va_list arguments;

	va_start(arguments, format);
	vsnprintf(text, sizeof(text), format, arguments);
	va_end(arguments);
	n3ds_log(text);
}
/* port/linux/include/halo_linux_source_fixups.h */
int platform_missing_map_message(const char *name, char *text, int size)
{
	return 0;
}
/* port/linux/src/sdl_platform.h */
void platform_scoreboard_scroll(int open, long *notches, long *pages)
{
}
/* port/linux/src/sdl_platform.h */
void platform_show_message(const char *title, const char *message)
{
}
/* port/linux/src/xinput_sdl.c */
void platform_text_typing(int typing)
{
}
/* port/linux/game/powerup_timers.h */
void powerup_timers_draw(short local_player_index)
{
}
/* source/rasterizer_text.h */
void rasterizer_text_flush(void)
{
}
/* source/rasterizer_transparent_geometry.h */
void rasterizer_transparent_geometry_order_models(short *order, long count)
{
}
/* port/linux/game/view_fov.h */
real render_fov_reticle_scale(short local_player_index)
{
	return 1.0f;
}
/* port/linux/game/view_fov.h */
real render_fov_vertical(short local_player_index, real native_vertical_field_of_view)
{
	return native_vertical_field_of_view; /* (the game's own) */
}
/* port/linux/include/halo_linux_source_fixups.h */
void render_interpolation_first_person(short local_player_index, struct real_matrix4x3 *node_matrices, short node_count, struct render_camera const *camera)
{
}
/* port/linux/include/halo_linux_source_fixups.h */
float render_interpolation_fraction(void)
{
	return 1.0f;
}
/* port/linux/include/halo_linux_source_fixups.h */
void render_interpolation_frame_begin(void)
{
}
/* port/linux/include/halo_linux_source_fixups.h */
void render_interpolation_frame_end(void)
{
}
/* port/linux/include/halo_linux_source_fixups.h */
struct real_matrix4x3 * render_interpolation_object_node_matrices(long object_index)
{
	return NULL;
}
/* port/linux/include/halo_linux_source_fixups.h */
void render_interpolation_reset(void)
{
}
/* port/linux/include/halo_linux_source_fixups.h */
void render_interpolation_tick(void)
{
}
/* port/linux/game/shield_color.c */
real_rgb_color const * shield_color_colors(long unit_index, real_rgb_color const *colors)
{
	return colors; /* (as it is: the fork's unit is not built yet) */
}
/* port/linux/game/shield_glow.c */
void shield_glow_update(void)
{
}
/* port/linux/src/text_hires.h */
int text_hires_covers(long font, unsigned long code)
{
	return 0;
}
/* port/linux/src/text_hires.h */
int text_hires_enabled(void)
{
	return 0;
}
/* port/linux/src/text_hires.h */
long text_hires_font(char const *tag_name, float cap_height, short const *letter_widths, float oversample)
{
	return 0;
}
/* port/linux/src/text_hires.h */
int text_hires_glyph(long font, unsigned long code, struct text_hires_glyph *glyph)
{
	return 0;
}
/* port/linux/src/text_hires.h */
void text_hires_register_atlas(const unsigned long *texture, unsigned long width, unsigned long height)
{
}
/* port/linux/game/view_fov.h */
boolean viewmodel_draws_geometry(boolean first_person)
{
	return 1;
}
/* port/linux/game/view_fov.h */
boolean viewmodel_is_visible(void)
{
	return 1;
}
/* port/linux/game/view_fov.h */
void viewmodel_projection_begin(void)
{
}
/* port/linux/game/view_fov.h */
void viewmodel_projection_end(void)
{
}
/* port/linux/game/view_fov.h */
boolean viewmodel_shield_is_visible(void)
{
	return 1;
}


/* ---------- internet play (port/linux/src/p2p.c, Phase 3): no peers, so
   every address is not a peer's (0) and a broadcast reaches none */
/* port/linux/src/p2p.h: this machine's identifier, which its XNADDR carries:
   until p2p.c (whose identifier comes from its internet play key), a random
   one for each run, made like a locally administered unicast MAC address as
   the fork's is (p2p_identifier_from_hash) */
void posix_random_bytes(void *buffer, unsigned long size); /* (posix.h's posix_ulong) */
/* port/linux/game/map_share.h */
void map_share_update(void)
{
}
/* port/linux/src/p2p.h */
/* (until p2p.c, whose id is the machine's lasting one: one for each run,
   32 hex digits, as a host logs and its ban lists hold it) */
/* port/linux/game/network_voice.h */
void network_voice_update(void)
{
}
/* port/linux/game/map_families.c's map name parts, as there (the rest of
   that unit, with its map archives and hashes, comes with Phase 4's Custom
   Edition maps): a map's family by its name's ending, and its file's name */
static const char *const n3ds_map_family_suffixes[] = { "", "@ce", "@md", "@pc" };
static const char *const n3ds_map_family_badges[] = { "", "HALO PC", "HALOMD", "HALO PC RETAIL" };
int _stricmp(const char *a, const char *b);
short map_family_parse(char const *map, char *file, long size)
{
	char const *base = map;
	char const *cursor;
	long length;
	short family;

	for (cursor = map; *cursor; cursor++)
	{
		if (*cursor == '\\' || *cursor == '/')
			base = cursor + 1;
	}
	length = (long)strlen(base);
	for (family = 3; family > 0; family--)
	{
		long suffix_length = (long)strlen(n3ds_map_family_suffixes[family]);

		if (length > suffix_length && !_stricmp(base + length - suffix_length, n3ds_map_family_suffixes[family]))
		{
			length -= suffix_length;
			break;
		}
	}
	if (file && size > 0)
		snprintf(file, (size_t)size, "%.*s", (int)length, base);
	return family;
}
char const *map_family_suffix(short family)
{
	return family > 0 && family < 4 ? n3ds_map_family_suffixes[family] : "";
}
char const *map_family_badge(short family)
{
	return family > 0 && family < 4 ? n3ds_map_family_badges[family] : "";
}
/* ... and its names in the game's protocol (map_families.c's
   map_family_wire_names): an Xbox map's is its own; a Custom Edition map's
   custom_maps\<file>, HaloMD's maps_md\<file>.md, Halo PC's maps_pc\<file>.pc */
static const char *const n3ds_map_family_wire_folders[] = { "", "custom_maps\\", "maps_md\\", "maps_pc\\" };
static const char *const n3ds_map_family_wire_endings[] = { "", "", ".md", ".pc" };
void map_family_wire_name(char const *map, char *wire, long size)
{
	char file[64]; /* (map_families.c's MAP_FAMILY_FILE_LENGTH) */
	short family = map_family_parse(map, file, sizeof(file));

	if (size <= 0)
		return;
	if (family == 0)
		snprintf(wire, (size_t)size, "%s", map);
	else
		snprintf(wire, (size_t)size, "%s%s%s", n3ds_map_family_wire_folders[family], file,
			n3ds_map_family_wire_endings[family]);
}
static int n3ds_prefix_is(const char *text, const char *prefix, size_t length)
{
	size_t i;

	for (i = 0; i < length; i++)
	{
		char a = text[i] >= 'A' && text[i] <= 'Z' ? text[i] - 'A' + 'a' : text[i];
		char b = prefix[i] >= 'A' && prefix[i] <= 'Z' ? prefix[i] - 'A' + 'a' : prefix[i];

		if (a != b)
			return 0;
	}
	return 1;
}
short map_family_from_wire_name(char const *wire, char *map, long size)
{
	short family;

	for (family = 1; family < 4; family++)
	{
		const char *folder = n3ds_map_family_wire_folders[family];
		const char *ending = n3ds_map_family_wire_endings[family];
		size_t folder_length = strlen(folder);
		size_t ending_length = strlen(ending);
		size_t length = strlen(wire);
		const char *file = wire + folder_length;
		size_t file_length;

		if (length <= folder_length + ending_length || !n3ds_prefix_is(wire, folder, folder_length) ||
			!n3ds_prefix_is(wire + length - ending_length, ending, ending_length))
		{
			continue;
		}
		file_length = length - folder_length - ending_length;
		/* (a file's name alone: no folder in it, nor a family's suffix) */
		if (file_length >= 64 || memchr(file, '\\', file_length) || memchr(file, '/', file_length) ||
			memchr(file, '@', file_length))
		{
			continue;
		}
		if (size > 0)
			snprintf(map, (size_t)size, "%.*s%s", (int)file_length, file, n3ds_map_family_suffixes[family]);
		return family;
	}
	if (size > 0)
		snprintf(map, (size_t)size, "%s", wire);
	return map_family_parse(wire, NULL, 0);
}
/* port/linux/src/msvc_crt.c's: the MSVC name of strdup, which the fork's
   settings unit calls (port_config.c) */
char *_strdup(const char *text)
{
	size_t size = strlen(text) + 1;
	char *copy = malloc(size);

	if (copy)
		memcpy(copy, text, size);
	return copy;
}
/* port/linux/game/map_families.c's, for a build without Custom Edition maps
   (Phase 4 brings them): none is found */
unsigned char map_family_find(short family, char const *file, char *path, long size)
{
	(void)family;
	(void)file;
	if (path && size > 0)
		path[0] = 0;
	return 0;
}
/* port/linux/src/map_download.h */
#include "../../linux/src/map_download.h" /* (its progress structure) */
void map_download_get(struct map_download_progress *progress)
{
	/* (no download: MAP_DOWNLOAD_IDLE, nothing received) */
	if (progress)
		memset(progress, 0, sizeof(*progress));
}
/* port/linux/src/map_download.h */
int map_download_start(const char *name, unsigned int version)
{
	return 0;
}
/* port/linux/include/halo_map_families.h */
boolean map_is_downloaded(char const *map_name)
{
	return 0;
}
/* port/linux/game/network_recording.h */
void network_recording_hit(short shooter_index, short victim_index, real_point3d const *impact, byte verdict, real angle, real distance)
{
}
/* port/linux/game/network_recording.h */
void network_recording_host_tick(void)
{
}
/* port/linux/game/network_recording.h */
void network_recording_move(short player_index, byte kind, real amount)
{
}
/* port/linux/game/network_recording.h */
void network_recording_new_game(void)
{
}
/* port/linux/game/network_objects.c */
void render_interpolation_correct_object(long object_index, real_vector3d const *offset)
{
}
/* port/linux/game/network_test.c */
void test_input_hold_action(int hold)
{
}
/* port/linux/game/network_test.c */
void test_input_hold_fire(int hold)
{
}
/* port/linux/game/network_test.c */
void test_input_hold_melee(int hold)
{
}
/* port/linux/src/d3d8_gl.c's display.model_detail scale: the stock levels
   (Halo3DS's native renderer chooses its own reductions) */
float halo_model_detail_scale(int level)
{
	(void)level;
	return 1.0f;
}
int halo_model_detail_stock(void)
{
	return 1;
}
/* source/cseries/cseries_windows.c's (not in this port's build: Halo3DS
   has its own system layer), over the C library, for the fork's bots */
void *system_malloc(long size)
{
	return malloc((size_t)size);
}
void system_free(void *pointer)
{
	free(pointer);
}
/* port/linux/include/halo_keyboard.h */
int halo_chat_touch_layout(void)
{
	return 0;
}
/* port/linux/game/network_test.c */
void test_input_press(unsigned long buttons)
{
}
/* port/linux/src/platform.h's: the folder of the fork's own files that last
   (internet play's lasting invite key, the ban lists): this port's data
   folder on the SD card, where its settings are */
const char *platform_save_root(void)
{
	return "sdmc:/halo-source";
}
/* port/linux/src/discord_activity.h's: no Discord on the 3DS (a negative size:
   nothing to send) */
int discord_activity_json(char *json, int size, unsigned long nonce, unsigned long process_id,
	const void *game, const void *options, const char *party, const char *join_url)
{
	(void)nonce;
	(void)process_id;
	(void)game;
	(void)options;
	(void)party;
	(void)join_url;
	if (json && size > 0)
		json[0] = 0;
	return -1;
}
