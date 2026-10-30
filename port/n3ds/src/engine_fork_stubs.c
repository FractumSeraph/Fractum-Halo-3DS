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
/* port/linux/include/halo_linux_source_fixups.h */
void halo_screen_ui_offset(unsigned char centered)
{
}
/* port/linux/include/halo_linux_source_fixups.h */
long halo_screen_width(void)
{
	return 640; /* (the Xbox's screen: Halo3DS draws its own) */
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
/* port/linux/game/menu_functions.c */
boolean pc_menu_event_function_invoke(struct widget_instance *widget, struct event_record *event, long function_index, boolean *widget_deleted)
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
/* port/linux/game/menu_functions.c */
boolean pc_menu_text_color(struct widget_instance const *widget, real_rgb_color *rgb)
{
	return 0;
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
/* ... and its names in the game's protocol (map_families.c's
   map_family_wire_names): an Xbox map's is its own; a Custom Edition map's
   custom_maps\<file>, HaloMD's maps_md\<file>.md, Halo PC's maps_pc\<file>.pc */
static const char *const n3ds_map_family_wire_folders[] = { "", "custom_maps\\", "maps_md\\", "maps_pc\\" };
static const char *const n3ds_map_family_wire_endings[] = { "", "", ".md", ".pc" };
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
/* port/linux/src/map_download.h */
#include "../../linux/src/map_download.h" /* (its progress structure) */
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
	/* port: the C library's own (cseries.h's malloc asserts): NULL when it
	   cannot be had, which the fork's callers check (ce_hud.c's ui.map
	   inflation wants 32 MB, more than a New 3DS has left with a map loaded) */
	return (malloc)((size_t)size);
}
void system_free(void *pointer)
{
	(free)(pointer);
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
/* ---------- the ChupathingyCE edition's pieces a 3DS client does without */
/* server/src/dedicated.c's: this is never the dedicated server */
int dedicated_server_active(void)
{
	return 0;
}
/* port/linux/src/updater.c's: 3DS builds are copied to the SD card by hand
   (the updater is not built), and name no updater version */
const char *updater_version(void)
{
	return "";
}
/* port/linux/src/sdl_platform.c's: no web browser to open a page in, nor a
   clipboard to take an invite from */
int platform_open_url(const char *url)
{
	(void)url;
	return 0;
}
/* port/linux/src/map_torrents.c's: no BitTorrent on the 3DS (nor Custom
   Edition maps yet) */
void map_torrents_cancel(void)
{
}
/* port/linux/src/posix_ui_font.c's JPEG decoding (the game list's map
   pictures): none yet; the browser draws its cards for the maps instead */
unsigned char *posix_ui_image_decode(const unsigned char *data, int size, int *width, int *height)
{
	(void)data;
	(void)size;
	if (width)
		*width = 0;
	if (height)
		*height = 0;
	return NULL;
}
void posix_ui_image_free(unsigned char *pixels)
{
	(void)pixels;
}
/* port/linux/src/map_download.c's Custom Edition map downloader: not on the
   3DS until it plays those maps (Phase 4): "never" downloads, none runs */
/* port/linux/src/map_torrents.c's: none */
int map_torrents_fetch(const char *level_name, unsigned long version, const char *files)
{
	(void)level_name;
	(void)version;
	(void)files;
	return 0;
}
int map_torrents_fetching(char *status, int size)
{
	if (status && size > 0)
		status[0] = 0;
	return 0;
}
int map_torrents_download_progress(float *fraction, char *name, int size)
{
	if (fraction)
		*fraction = 0;
	if (name && size > 0)
		name[0] = 0;
	return 0;
}
int map_torrents_take_ready(char *level_name, int size)
{
	if (level_name && size > 0)
		level_name[0] = 0;
	return 0;
}
/* port/linux/game/map_share_files.c's (map sharing comes with Phase 4):
   no map's hash known */
/* port/linux/game/menu_functions.c's (the PC menus, not this port's): no
   profile editor to open */
unsigned char pc_menu_profile_edit_begin(void)
{
	return 0;
}
/* port/linux/game/map_share.c's (map sharing's downloads, after Custom
   Edition maps): none starts */
/* port/linux/game/menu_functions.c's (not built: Halo3DS's menus join from
   Online Games, browser_screen.c, whose own download offer has no join
   waiting in the menus to let go) */
void ui_widget_port_map_fetch_cancel(void)
{
}

/* ---------- the fork's desktop screens after network version 38 (fork_update.py
 * 8ab6b1324..8bc228b04): the Delta profile screen, the controls screen, the PC
 * menus' marquee and controller capture, the loading screen's skip. Halo3DS's
 * frontend has its own (or none): never open here */
struct widget_instance;
struct game_variant;
struct game_variant_options;
struct halo_ui_pointer;
boolean profile_screen_active(void) { return FALSE; }
boolean profile_screen_available(void) { return FALSE; }
boolean profile_screen_dialog_only(void) { return FALSE; }
void profile_screen_open(boolean from_online_games) { (void)from_online_games; }
void profile_screen_process(void) {}
void profile_screen_pointer(struct halo_ui_pointer const *pointer) { (void)pointer; }
void profile_screen_render(void) {}
boolean controls_screen_active(void) { return FALSE; }
void controls_screen_update(boolean menu_open) { (void)menu_open; }
void controls_screen_process(void) {}
void controls_screen_pointer(struct halo_ui_pointer const *pointer) { (void)pointer; }
void controls_screen_render(void) {}
long pc_menu_text_marquee(struct widget_instance const *widget, boolean focus) { (void)widget; (void)focus; return 0; }
boolean pc_menu_delta_profile_open(void) { return FALSE; }
boolean pc_menu_controller_capture(void) { return FALSE; }
boolean loading_screen_skip_pressed(void) { return FALSE; }
/* (the fork's audio probe tool, a desktop test mode) */
boolean probe_active(void) { return FALSE; }
/* (the high-resolution HUD meter flag: the 3DS draws its meters natively) */
void halo_custom_edition_hud_meter(int active) { (void)active; }
/* (gametypes written to a folder as files: the fork's dedicated server's) */
void gametype_files_profile_saved(long profile_index, struct game_variant const *variant,
	struct game_variant_options const *options) { (void)profile_index; (void)variant; (void)options; }
void gametype_files_profile_deleted(long profile_index) { (void)profile_index; }

/* the fork's frame interpolation (port/linux/game/render_interpolation.c,
 * display.interpolation): the 3DS draws one frame a tick, so never between
 * ticks; these give what that file gives with it off */
#define N3DS_TICKS_PER_SECOND 30.0f
unsigned char render_interpolation_drawing(void) { return 0; }
float *render_interpolation_object_function_values(long object_index) { (void)object_index; return NULL; }
float render_interpolation_ticks_since(long now, long then) { return (float)(now - then); }
float render_interpolation_game_time_sec(long ticks) { return (float)ticks * (1.0f / N3DS_TICKS_PER_SECOND); }

/* port: fork 4a752e42f's callers of units the 3DS does not build. The
probe (halo --probe, network_test.c) has no command line to start it on the
3DS, so no game is a peer's to it; and a dedicated server's private files
(port_config.c: config_server_mode, never set here) need nothing */
boolean menu_advertised_game_from_peer(long index)
{
	(void)index;
	return 0;
}

int posix_server_private_file(const char *path)
{
	(void)path;
	return 1;
}

/* port: the fork's stencil modes (rasterizer_xbox.c), for light volumes on
the viewmodel: the 3DS's rasterizer has none, and draws those volumes in one
pass (light_volumes.c) */
short rasterizer_get_stencil_mode(void) { return 0; }
void rasterizer_set_stencil_mode(long stencil_mode) { (void)stencil_mode; }

/* port: fork 7da18311c's controller remapping (controller_remap_game.c,
controller_remap_input.c: SDL's pads) and the desktop's debug camera keys
(xinput_sdl.c): the 3DS reads its own buttons (Halo3DS's input), remapped by
nothing, and has no mouse or keyboard */
int controller_remap_playing(short port) { (void)port; return 0; }
void controller_remap_reset(void) {}
int halo_linux_debug_camera_input(int *middle, int *up, int *down, float *wheel)
{
	*middle = 0;
	*up = 0;
	*down = 0;
	*wheel = 0.0f;
	return 0;
}
/* (render_interpolation.c's: the 3DS draws one frame a tick, so a point
stays where the tick put it) */
void render_interpolation_object_point(long object_index, real_point3d *point) { (void)object_index; (void)point; }
int controller_remap_camera(short local_player, int available, int controlling,
	unsigned int *held, unsigned int *pressed)
{
	(void)local_player;
	(void)available;
	(void)controlling;
	*held = 0;
	*pressed = 0;
	return 0;
}
/* (network_chat.c's speaker: the 3DS's one player plays its one pad) */
short local_player_get_next(short local_player_index);
short controller_remap_local_player(short port)
{
	return port == 0 ? local_player_get_next(-1) : -1;
}
void controller_remap_movement(short port, float *forward, float *strafe) { (void)port; (void)forward; (void)strafe; }
struct game_input_preferences;
void controller_remap_prompt(short port, struct game_input_preferences *preferences) { (void)port; (void)preferences; }
/* (xinput_sdl.c's: the 3DS's player always plays its buttons, so the vote
prompts name the D-pad) */
int halo_gamepad_last_used(void) { return 1; }
/* (d3d8_gl.c's: 1, so the HUD's overlays stay point sampled for one player,
as they were on the 3DS before fork 7da18311c: hud_draw.c) */
float halo_screen_scale(void) { return 1.0f; }
/* (hud_hires_tags.c's, display.high_res_hud: not on the 3DS) */
struct bitmap_data;
struct bitmap_data const *hud_hires_crosshair_bitmap(struct bitmap_data const *bitmap, short sequence_index,
	short frame_index)
{
	(void)sequence_index;
	(void)frame_index;
	return bitmap;
}
/* (d3d8_gl.c's: the 3DS's screen is the Xbox's 640 columns, halo_screen_width
above, so no margins) */
long halo_screen_ui_offset_current(void) { return 0; }
/* (menu_functions.c's, the desktops' menus: the 3DS plays one controller, so
no screen reads or routes another's) */
struct widget_instance;
unsigned char pc_menu_screen_reads_every_controller(struct widget_instance const *widget) { (void)widget; return 0; }
unsigned char pc_menu_screen_routes_by_controller(struct widget_instance const *widget) { (void)widget; return 0; }
/* (ui_point.c's, which the 3DS does not build: which part of a prompt a
point is on, NONE for none) */
struct ui_point_prompt { short top, bottom, left, right, split; };
int ui_point_prompt_part(struct ui_point_prompt const *prompt, short margin, short x, short y)
{
	if (prompt->right <= prompt->left ||
		y < prompt->top - margin || y >= prompt->bottom + margin ||
		x < prompt->left - margin || x >= prompt->right + margin)
	{
		return -1;
	}
	return x < prompt->split || prompt->split >= prompt->right ? 0 : 1;
}
