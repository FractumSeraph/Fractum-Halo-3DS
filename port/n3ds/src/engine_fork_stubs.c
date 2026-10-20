/* Stand-ins for the FractumSeraph/OpenCE fork's port layer (port/linux),
 * which its engine calls, until its units are built for the 3DS: generated
 * from the fork's declarations (port/n3ds/tools-local/gen_stubs.py); each does
 * nothing and returns zero. Phase 1 of docs/MULTIPLAYER-PLAN.md. */
#include "cseries.h"
#include "math/real_math.h"
#include "math/integer_math.h"
#include <stddef.h>
#include <wchar.h>

/* port/linux/src\build_identity.h */
const char * build_identity(void)
{
	return NULL;
}
/* source\cache_files.h */
boolean cache_file_tags_are_ce(void)
{
	return 0;
}
/* port/linux/game\ce_hud.c */
boolean ce_hud_halo_pc_overlay_blend_functions(void)
{
	return 0;
}
/* port/linux/game\ce_hud.c */
boolean ce_hud_overlays_blocked(void)
{
	return 0;
}
/* port/linux/game\coop_enemies.h */
short coop_enemies_extra_count(long encounter_index, short count)
{
	return 0;
}
/* port/linux/game\coop_enemies.h */
boolean coop_enemies_fallback_position(real_point3d const *origin, short number, real_point3d *position)
{
	return 0;
}
/* port/linux/game\coop_enemies.h */
void coop_enemies_rider_seated(long vehicle_index, long unit_index)
{
}
/* port/linux/game\coop_enemies.h */
boolean coop_enemies_rider_unseated(long vehicle_index, long unit_index)
{
	return 0;
}
/* port/linux/game\coop_enemies.h */
boolean coop_enemies_spread_position(real_point3d const *origin, short number, long *taken, real_point3d *position)
{
	return 0;
}
/* port/linux/game\coop_scripts.h */
boolean coop_scripts_any_player_will_do(long object_list_index)
{
	return 0;
}
/* port/linux/game\coop_scripts.h */
boolean coop_scripts_arrive_at(long player_index, real_point3d const *destination, long const *arrived_unit_indices, short arrived_count)
{
	return 0;
}
/* port/linux/game\coop_scripts.h */
void coop_scripts_board_followers(long unit_index, long vehicle_index, char const *seat_name)
{
}
/* port/linux/game\coop_scripts.h */
void coop_scripts_exit_followers(long unit_index, long vehicle_index)
{
}
/* port/linux/game\coop_scripts.h */
void coop_scripts_gather_in_volume(short trigger_volume_index, long object_list_index)
{
}
/* port/linux/game\coop_scripts.h */
void coop_scripts_player_add_equipment(long unit_index, short starting_profile_index, boolean reset_equipment)
{
}
/* port/linux/game\coop_scripts.h */
boolean coop_scripts_safe_to_save(void)
{
	return 1;
}
/* port/linux/game\coop_scripts.h */
void coop_scripts_suspend_followers(long unit_index, boolean suspended)
{
}
/* port/linux/game\coop_scripts.h */
void coop_scripts_teleport_followers(long unit_index, real_point3d const *destination)
{
}
/* port/linux/game\coop_scripts.h */
boolean coop_scripts_unit_near(long unit_index, real_point3d const *destination)
{
	return 0;
}
/* port/linux/game\coop_spectate.h */
void coop_skip_vote_draw(short local_player_index)
{
}
/* port/linux/game\coop_spectate.h */
void coop_spectate_camera(struct dead_camera *camera)
{
}
/* port/linux/game\coop_spectate.h */
void coop_spectate_draw(short local_player_index)
{
}
/* port/linux/game\coop_spectate.h */
long coop_spectate_unit(short local_player_index)
{
	return -1;
}
/* port/linux/game\coop_spectate.h */
boolean coop_spectating(void)
{
	return 0;
}
/* port/linux/include\halo_port_limits.h */
int delta_legacy_announce(void)
{
	return HALO_PORT_NETWORK_VERSION;
}
/* port/linux/include\halo_port_limits.h */
int delta_legacy_maximum(void)
{
	return HALO_PORT_NETWORK_VERSION_MAXIMUM;
}
/* port/linux/include\halo_port_limits.h */
int delta_legacy_minimum(void)
{
	return HALO_PORT_NETWORK_VERSION_MINIMUM;
}
/* port/linux/game\network_distributed.h */
long distributed_player_ping(short player_index)
{
	return 0;
}
/* port/linux/game\network_distributed.h */
byte distributed_player_to_byte(long player_index)
{
	return 0;
}
/* port/linux/game\download_screen.c */
boolean download_screen_active(void)
{
	return 0;
}
/* port/linux/game\download_screen.c */
void download_screen_process(void)
{
}
/* port/linux/game\download_screen.c */
void download_screen_render(void)
{
}
/* source\sound_dsound_xbox.c */
void dsound_port_set_channel_stereo_position(short virtual_channel_index, boolean positioned, real pan, real distance, real minimum_distance, real distance_fade, real occlusion, real obstruction, boolean attenuate_direct_path)
{
}
/* port/linux/game\first_person_legs.c */
real_matrix4x3 const * first_person_legs_node_matrices(long object_index, real_matrix4x3 const *nodes)
{
	return nodes; /* (as it is: the fork's unit is not built yet) */
}
/* port/linux/game\first_person_legs.c */
void first_person_legs_reset(void)
{
}
/* port/linux/game\first_person_legs.c */
boolean first_person_legs_wanted(long object_index)
{
	return 0;
}
/* port/linux/game\follow_camera.h */
void follow_camera_draw(short local_player_index)
{
}
/* port/linux/game\follow_camera.h */
void follow_camera_player_killed(long killing_player_index, long dead_player_index)
{
}
/* port/linux/game\follow_camera.h */
long follow_camera_unit(short local_player_index)
{
	return -1; /* (NONE: the death camera) */
}
/* port/linux/include\halo_linux_source_fixups.h */
int halo_interpolation_enabled(void)
{
	return 0;
}
/* port/linux/include\halo_keyboard.h */
int halo_keyboard_action_shown(int action, char *shown, unsigned long size)
{
	return 0;
}
/* port/linux/include\halo_keyboard.h */
unsigned long halo_keyboard_actions(short controller_index)
{
	return 0;
}
/* port/linux/include\halo_keyboard.h */
int halo_keyboard_prompts(void)
{
	return 0;
}
/* port/linux/src\xinput_sdl.c */
int halo_linux_mouse_aiming(short gamepad_index)
{
	return 0;
}
/* port/linux/src\xinput_sdl.c */
int halo_linux_mouse_look(short gamepad_index, float *yaw, float *pitch)
{
	return 0;
}
/* port/linux/src\xinput_sdl.c */
int halo_linux_touch_aiming(short gamepad_index)
{
	return 0;
}
/* port/linux/src\xinput_sdl.c */
int halo_linux_touch_look(short gamepad_index, float *yaw, float *pitch, float *gyro_yaw, float *gyro_pitch)
{
	return 0;
}
/* port/linux/src\xinput_sdl.c */
int halo_linux_touch_move(short controller_index, float *forward, float *strafe)
{
	return 0;
}
/* port/linux/include\halo_ui_pointer.h */
int halo_scoreboard_pointer_update(int offered, struct halo_ui_pointer *pointer)
{
	return 0;
}
/* port/linux/include\halo_linux_source_fixups.h */
void halo_screen_ui_offset(unsigned char centered)
{
}
/* port/linux/include\halo_linux_source_fixups.h */
long halo_screen_width(void)
{
	return 640; /* (the Xbox's screen: Halo3DS draws its own) */
}
/* port/linux/include\halo_ui_pointer.h */
int halo_ui_pointer_update(int menus_active, struct halo_ui_pointer *pointer)
{
	return 0;
}
/* port/linux/game\hud_hires_tags.c */
boolean hud_hires_button_draw(struct icon_hud_element_definition const *icon, rectangle2d *cursor, pixel32 color)
{
	return 0;
}
/* port/linux/game\hud_hires_tags.c */
struct bitmap_data const * hud_hires_sprite_bitmap(struct bitmap_data const *bitmap, short sequence_index)
{
	return NULL;
}
/* port/linux/game\hud_hires_tags.c */
void hud_hires_tags_unloaded(void)
{
}
/* port/linux/src\log_address.h */
const char * log_address(const unsigned char *bytes, int length, int port, char *text, int size)
{
	return NULL;
}
/* port/linux/include\halo_map_families.h */
short map_family_from_wire_name(char const *wire, char *map, long size)
{
	return 0;
}
/* port/linux/include\halo_map_families.h */
void map_family_wire_name(char const *map, char *wire, long size)
{
}
/* port/linux/game\map_share.h */
boolean map_share_downloading(char const *level_name)
{
	return 0;
}
/* port/linux/game\map_share.h */
void map_share_handle_message(long machine_index, word const *message, word size, boolean from_stream)
{
}
/* port/linux/game\map_share.h */
boolean map_share_handles_message(word const *message, word size)
{
	return 0;
}
/* port/linux/game\map_share.h */
boolean map_share_holds_players(void)
{
	return 0;
}
/* port/linux/game\map_share.h */
void map_share_level_name_choose(char *level_name, long size)
{
}
/* port/linux/game\map_share.h */
boolean map_share_machine_downloading(long machine_index)
{
	return 0;
}
/* port/linux/game\map_share.h */
boolean map_share_machine_needs_map(long machine_index)
{
	return 0;
}
/* port/linux/src\map_torrents.h */
void map_torrents_playing(const char *level_name, unsigned long version, int hosting)
{
}
/* port/linux/game\menu_functions.c */
void menu_functions_text_box_drawn(struct widget_instance *widget, rectangle2d const *bounds)
{
}
/* port/linux/game\menu_style.c */
void menu_style_gametype_rules(long profile_index, wchar_t *text, long size)
{
}
/* port/linux/game\network_distributed.h */
void network_actors_drive(void)
{
}
/* port/linux/game\network_actors.c */
void network_actors_note_control(long unit_index, struct unit_control_data const *control_data)
{
}
/* port/linux/game\network_actors.c */
void network_actors_note_impulse(long unit_index, short animation_impulse, real_vector2d const *alignment_vector)
{
}
/* port/linux/game\network_actors.c */
void network_actors_note_leap(long unit_index, real_vector2d const *alignment_vector)
{
}
/* port/linux/game\network_actors.c */
void network_actors_note_melee(long unit_index, real_vector2d const *alignment_vector)
{
}
/* port/linux/game\network_actors.c */
void network_actors_note_speech(long unit_index, long sound_definition_index)
{
}
/* port/linux/game\network_actors.c */
void network_actors_note_user_animation(long unit_index, long animation_graph_index, short animation_index, boolean interpolate)
{
}
/* port/linux/game\network_coop.h */
boolean network_coop_active(void)
{
	return 0;
}
/* port/linux/game\network_coop.h */
boolean network_coop_devices_remote(void)
{
	return 0;
}
/* port/linux/game\network_coop.h */
void network_coop_note_attach(long parent_index, char const *parent_marker_name, long child_index, char const *child_marker_name)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_detach(long parent_index, long child_index)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_device_snap(short group_index)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_effect(long effect_definition_index, short cutscene_flag_index)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_hud(short kind, short value)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_nav_point(short kind, short nav_index, long target, long marker, real vertical_offset)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_object_effect(long effect_definition_index, long object_index, char const *marker_name)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_player_effect(short kind, real a, real b, real c)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_scenery_animation(long object_index, long animation_graph_index, short animation_index, short frame_index)
{
}
/* port/linux/game\network_distributed.h */
void network_coop_note_sound(short kind, long definition_index, long object_index, real scale)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_surface_broken(short breakable_surface_index, real_point3d const *epicenter)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_title(short title_index, real delay)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_unit_animation(long unit_index, long animation_graph_index, short animation_index, boolean interpolate)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_unit_animation_frame(long unit_index, short frame_index)
{
}
/* port/linux/game\network_coop.h */
void network_coop_note_unit_open(long unit_index, boolean open)
{
}
/* port/linux/game\network_coop.h */
boolean network_coop_player_collisions(void)
{
	return 1;
}
/* port/linux/game\network_coop.h */
boolean network_coop_player_has_structure_bsp(long player_index)
{
	return 1; /* (always, outside co-op) */
}
/* port/linux/game\network_coop.h */
void network_coop_reverted(long now)
{
}
/* port/linux/game\network_coop.h */
boolean network_coop_set_players_vitality(long unit_index, boolean maximum, real body, real shield)
{
	return 0;
}
/* port/linux/game\network_coop.h */
void network_coop_skip_done(void)
{
}
/* port/linux/game\network_coop.h */
boolean network_coop_skip_offered(void)
{
	return 0;
}
/* port/linux/game\network_coop.h */
void network_coop_vehicle_dropped(long vehicle_index, long carrier_index)
{
}
/* port/linux/game\network_coop.h */
boolean network_coop_vote_skip(void)
{
	return 0;
}
/* port/linux/game\network_damage.c */
void network_damage_aftermath(long object_index, struct damage_data const *damage, unsigned long being_damaged_flags, real shield_damage, real body_damage, real body_damage_multiplier, short body_part, short node_index, short region_index, short material_index, long victim_player_index)
{
}
/* port/linux/game\network_damage.c */
boolean network_damage_deals(struct damage_data const *damage, long object_index, short node_index, short region_index, short material_index, real_vector3d const *object_normal, boolean authorized)
{
	return 1; /* (this machine deals its damage) */
}
/* port/linux/game\network_damage.c */
boolean network_damage_killer_score(long player_index, long *score)
{
	return 0;
}
/* port/linux/game\network_distributed.h */
void network_damage_note_grenade(long unit_index, short grenade_type)
{
}
/* port/linux/game\network_damage.c */
void network_damage_player_effect(long player_index, struct damage_data const *damage, real total_damage)
{
}
/* port/linux/game\network_damage.c */
boolean network_damage_replaying_kill(void)
{
	return 0;
}
/* port/linux/game\network_distributed.c */
void network_distributed_ban(long machine_index, unsigned long address, char const *names)
{
}
/* port/linux/game\network_distributed.c */
boolean network_distributed_banned(unsigned long address, char const *hardware_id)
{
	return 0;
}
/* port/linux/game\network_distributed.c */
void network_distributed_handle_stream_message(long machine_index, word const *message, word size)
{
}
/* port/linux/game\network_distributed.c */
void network_distributed_kick(char const *names)
{
}
/* port/linux/game\network_distributed.c */
void network_distributed_new_game(void)
{
}
/* port/linux/game\network_distributed.c */
void network_distributed_player_killed(long *killing_player_index, long *killing_object_index, long dead_player_index, boolean *friendly_fire)
{
}
/* port/linux/game\network_distributed.c */
void network_distributed_player_picked_up(long player_index, short kind, long definition_index, short count)
{
}
/* port/linux/game\network_distributed.c */
void network_distributed_tick(void)
{
}
/* port/linux/game\network_objects.c */
boolean network_objects_creating_host_object(void)
{
	return 0;
}
/* port/linux/game\network_objects.c */
short network_objects_damage_animation(long unit_index, short type, short animation_index)
{
	return animation_index; /* (the game's own pick) */
}
/* port/linux/game\network_objects.c */
boolean network_objects_may_delete(long object_index)
{
	return 1;
}
/* port/linux/game\network_objects.c */
long network_objects_new_object_index(void)
{
	return -1; /* (NONE: the first free) */
}
/* port/linux/game\network_objects.c */
void network_objects_placed(void)
{
}
/* port/linux/game\network_voice.h */
boolean network_voice_available(void)
{
	return 0;
}
/* port/linux/game\network_voice.h */
void network_voice_draw_icon(rectangle2d const *bounds, boolean muted, real alpha)
{
}
/* port/linux/game\network_voice.h */
void network_voice_handle_message(long machine_index, word const *message, word size)
{
}
/* port/linux/game\network_voice.h */
boolean network_voice_handles_message(word const *message, word size)
{
	return 0;
}
/* port/linux/game\network_voice.h */
void network_voice_machine_joined(long machine_index)
{
}
/* port/linux/game\network_voice.h */
boolean network_voice_machine_muted(long machine_index)
{
	return 0;
}
/* port/linux/game\network_voice.h */
boolean network_voice_machine_speaking(long machine_index)
{
	return 0;
}
/* port/linux/game\network_voice.h */
void network_voice_mute_machine(long machine_index, boolean mute)
{
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_available(void)
{
	return 0;
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_get_status(struct network_votekick_status *status)
{
	return 0;
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_host(void)
{
	return 0;
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_host_kick(short player_index, boolean ban)
{
	return 0;
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_kept_out(unsigned long address, char const *hardware_id)
{
	return 0;
}
/* port/linux/game\network_votekick.h */
void network_votekick_machine_joined(long machine_index)
{
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_player_named(char const *text)
{
	return 0;
}
/* port/linux/game\network_votekick.h */
boolean network_votekick_request(short player_index)
{
	return 0;
}
/* port/linux/game\object_bounds_cache.c */
void object_bounds_cache_invalidate(void)
{
}
/* port/linux/game\object_bounds_cache.c */
boolean object_bounds_cache_out_of_reach(long object_index, real_point3d const *point, real radius)
{
	return 0;
}
/* port/linux/game\object_bounds_cache.c */
void object_bounds_cache_update(long object_index, real_point3d const *center, real radius)
{
}
/* port/linux/src\p2p.h */
void p2p_hardware_id_sanitize(char *destination, int size, const char *source)
{
}
/* port/linux/src\p2p.h */
void p2p_set_game_player_counts(int count, int maximum)
{
}
/* port/linux/src\p2p.h */
void p2p_set_hosting_allowed(int allowed)
{
}
/* port/linux/src\p2p.h */
void p2p_set_hosting_public(int public)
{
}
/* port/linux/game\pal_tags.c */
boolean pal_tags_first_person_advance(short local_player_index, long graph_index, short animation_index, short frame_index)
{
	return 1; /* (an NTSC map's animation advances every tick) */
}
/* port/linux/game\pal_tags.c */
real pal_tags_first_person_fraction(short local_player_index, long graph_index, short animation_index, short frame_index)
{
	return 0.0f;
}
/* port/linux/game\pal_tags.c */
short pal_tags_first_person_frames(long graph_index, short animation_index, short frames)
{
	return frames;
}
/* port/linux/game\menu_tags.c */
boolean pc_menu_button_key_cap(long bitmap_tag_index, short x0, short y0, short x1, short y1, float alpha)
{
	return 0;
}
/* port/linux/game\menu_functions.c */
boolean pc_menu_event_function_invoke(struct widget_instance *widget, struct event_record *event, long function_index, boolean *widget_deleted)
{
	return 0;
}
/* port/linux/game\menu_tags.c */
boolean pc_menu_frame_placement(struct bitmap_data const *bitmap, short *x, short *y, short *width, short *height, short *texel_y)
{
	return 0;
}
/* port/linux/game\menu_functions.c */
void pc_menu_game_data_function_invoke(struct widget_instance *widget, long function)
{
}
/* port/linux/game\menu_functions.c */
unsigned char pc_menu_split_players(void)
{
	return 0;
}
/* port/linux/game\menu_tags.c */
boolean pc_menu_tag(long tag_index)
{
	return 0;
}
/* port/linux/game\menu_functions.c */
boolean pc_menu_text_color(struct widget_instance const *widget, real_rgb_color *rgb)
{
	return 0;
}
/* port/linux/game\menu_tags.c */
char const * pc_menus_root_name(void)
{
	return "ui\\shell\\main_menu\\main_menu"; /* (the Xbox's) */
}
/* port/linux/include\halo_linux_source_fixups.h */
char const * pc_menus_screen(char const *name)
{
	return name; /* (as it is: the fork's unit is not built yet) */
}
/* source\physical_memory_map.h */
void * physical_memory_get_tag_cache_base_address(void)
{
	return NULL;
}
/* port/linux/src\platform.h */
void platform_log(const char *format, ...)
{
}
/* port/linux/include\halo_linux_source_fixups.h */
int platform_missing_map_message(const char *name, char *text, int size)
{
	return 0;
}
/* port/linux/src\sdl_platform.h */
void platform_scoreboard_scroll(int open, long *notches, long *pages)
{
}
/* port/linux/src\sdl_platform.h */
void platform_show_message(const char *title, const char *message)
{
}
/* port/linux/src\xinput_sdl.c */
void platform_text_typing(int typing)
{
}
/* port/linux/game\powerup_timers.h */
void powerup_timers_draw(short local_player_index)
{
}
/* source\rasterizer_text.h */
void rasterizer_text_flush(void)
{
}
/* source\rasterizer_transparent_geometry.h */
void rasterizer_transparent_geometry_order_models(short *order, long count)
{
}
/* port/linux/game\view_fov.h */
real render_fov_reticle_scale(short local_player_index)
{
	return 1.0f;
}
/* port/linux/game\view_fov.h */
real render_fov_vertical(short local_player_index, real native_vertical_field_of_view)
{
	return native_vertical_field_of_view; /* (the game's own) */
}
/* port/linux/include\halo_linux_source_fixups.h */
void render_interpolation_first_person(short local_player_index, struct real_matrix4x3 *node_matrices, short node_count, struct render_camera const *camera)
{
}
/* port/linux/include\halo_linux_source_fixups.h */
float render_interpolation_fraction(void)
{
	return 1.0f;
}
/* port/linux/include\halo_linux_source_fixups.h */
void render_interpolation_frame_begin(void)
{
}
/* port/linux/include\halo_linux_source_fixups.h */
void render_interpolation_frame_end(void)
{
}
/* port/linux/include\halo_linux_source_fixups.h */
struct real_matrix4x3 * render_interpolation_object_node_matrices(long object_index)
{
	return NULL;
}
/* port/linux/include\halo_linux_source_fixups.h */
void render_interpolation_reset(void)
{
}
/* port/linux/include\halo_linux_source_fixups.h */
void render_interpolation_tick(void)
{
}
/* port/linux/game\shield_color.c */
real_rgb_color const * shield_color_colors(long unit_index, real_rgb_color const *colors)
{
	return colors; /* (as it is: the fork's unit is not built yet) */
}
/* port/linux/game\shield_glow.c */
void shield_glow_update(void)
{
}
/* port/linux/game\sightjack.h */
void sightjack_camera_update(void *camera, void *controls, void *result)
{
}
/* port/linux/game\sightjack.h */
boolean sightjack_object_hidden(long object_index)
{
	return 0;
}
/* port/linux/game\sightjack.h */
long sightjack_watched_unit(short local_player_index)
{
	return -1;
}
/* port/linux/game\team_change.h */
boolean team_change_death(long dead_player_index)
{
	return 0;
}
/* port/linux/game\team_change.h */
void team_change_player_died(long dead_player_index)
{
}
/* port/linux/src\text_hires.h */
int text_hires_covers(long font, unsigned long code)
{
	return 0;
}
/* port/linux/src\text_hires.h */
int text_hires_enabled(void)
{
	return 0;
}
/* port/linux/src\text_hires.h */
long text_hires_font(char const *tag_name, float cap_height, short const *letter_widths, float oversample)
{
	return 0;
}
/* port/linux/src\text_hires.h */
int text_hires_glyph(long font, unsigned long code, struct text_hires_glyph *glyph)
{
	return 0;
}
/* port/linux/src\text_hires.h */
void text_hires_register_atlas(const unsigned long *texture, unsigned long width, unsigned long height)
{
}
/* port/linux/game\view_fov.h */
boolean viewmodel_draws_geometry(boolean first_person)
{
	return 1;
}
/* port/linux/game\view_fov.h */
boolean viewmodel_is_visible(void)
{
	return 1;
}
/* port/linux/game\view_fov.h */
void viewmodel_projection_begin(void)
{
}
/* port/linux/game\view_fov.h */
void viewmodel_projection_end(void)
{
}
/* port/linux/game\view_fov.h */
boolean viewmodel_shield_is_visible(void)
{
	return 1;
}

/* ---------- the settings: the fork's table's defaults (port/linux/src/port_config.c) */

#include <string.h>
#include <stdlib.h>

struct n3ds_fork_setting { const char *name; char kind; const char *value; };
static const struct n3ds_fork_setting n3ds_fork_settings[] = {
	{ "display.mode", 's', "" },
	{ "display.resolution", 's', "native" },
	{ "display.resolution_scaling", 's', "native" },
	{ "display.window_size", 's', "" },
	{ "display.window_scale", 'i', "2" },
	{ "display.screen_width", 'i', "0" },
	{ "display.texture_decode", 's', "auto" },
	{ "display.vsync", 'b', "true" },
	{ "display.max_fps", 'i', "0" },
	{ "display.anti_aliasing", 's', "off" },
	{ "display.interpolation", 'b', "true" },
	{ "display.direct_camera", 'b', "true" },
	{ "display.fov", 'r', "0.0" },
	{ "display.viewmodel_fov", 'r', "0.0" },
	{ "display.viewmodel_visible", 'b', "true" },
	{ "display.viewmodel_shield", 'b', "false" },
	{ "display.first_person_legs", 'b', "false" },
	{ "display.shield_glow", 'b', "false" },
	{ "display.shield_glow_intensity", 's', "default" },
	{ "display.shield_color", 's', "default" },
	{ "display.far_clip_distance", 'r', "1024.0" },
	{ "display.match_timer", 'b', "false" },
	{ "display.high_res_hud", 'b', "true" },
	{ "display.hud_button_style", 's', "classic" },
	{ "display.high_res_text", 'b', "true" },
	{ "display.high_res_font", 's', "" },
	{ "display.high_res_font_large", 's', "" },
	{ "display.version_watermark", 'b', "false" },
	{ "display.shadow_resolution", 'i', "128" },
	{ "display.menus", 's', "pc" },
	{ "display.menus", 's', "chupathingy" },
	{ "display.player_names", 's', "all" },
	{ "display.player_name_scale", 'r', "1.0" },
	{ "display.follow_camera", 's', "long_waits" },
	{ "display.follow_camera_after", 'r', "5.0" },
	{ "display.powerup_timers", 'b', "false" },
	{ "display.loading_screen_min_seconds", 'r', "0.0" },
	{ "display.scoreboard_team_layout", 's', "teams" },
	{ "display.scoreboard_background", 'b', "true" },
	{ "display.show_quit_players", 'b', "true" },
	{ "display.scoreboard_background_color", 's', "16, 16, 16, 150" },
	{ "display.pause_change_teams", 'b', "true" },
	{ "display.server_messages", 'b', "true" },
	{ "display.server_message_text_color", 's', "" },
	{ "display.server_message_border_color", 's', "" },
	{ "display.server_message_background_color", 's', "" },
	{ "display.per_pixel_lighting", 'b', "false" },
	{ "audio.enabled", 'b', "true" },
	{ "audio.volume", 'r', "1.0" },
	{ "audio.music_volume", 'r', "1.0" },
	{ "audio.effects_volume", 'r', "1.0" },
	{ "audio.reverb", 'b', "true" },
	{ "audio.voice_chat", 's', "push_to_talk" },
	{ "audio.voice_volume", 'r', "1.0" },
	{ "audio.output_device", 's', "default" },
	{ "audio.input_device", 's', "default" },
	{ "audio.resampling", 's', "sinc" },
	{ "input.touch_controls", 's', "on" },
	{ "input.touch_aim_assist", 'b', "true" },
	{ "input.mouse_sensitivity", 'r', "1.0" },
	{ "input.invert_mouse", 'b', "false" },
	{ "input.keyboard_prompts", 'b', "true" },
	{ "input.mouse_aim_assist", 'b', "false" },
	{ "input.mouse_vertical_sensitivity", 'r', "0.0" },
	{ "chat.enabled", 'b', "true" },
	{ "chat.show", 's', "all" },
	{ "chat.filter", 's', "mask" },
	{ "chat.filter_file", 's', "chat_filter.txt" },
	{ "chat.streamer_mode", 'b', "false" },
	{ "chat.quick_messages", 's', "Nice shot|Need backup|On my way|Thanks|Good game|Sorry" },
	{ "controls.move_forward", 's', "W" },
	{ "controls.move_backward", 's', "S" },
	{ "controls.strafe_left", 's', "A" },
	{ "controls.strafe_right", 's', "D" },
	{ "controls.jump", 's', "Space" },
	{ "controls.crouch", 's', "C" },
	{ "controls.fire", 's', "Mouse Left" },
	{ "controls.throw_grenade", 's', "Mouse Right, G" },
	{ "controls.melee", 's', "F, Mouse 4" },
	{ "controls.reload", 's', "R" },
	{ "controls.zoom", 's', "Z, Mouse Middle" },
	{ "controls.switch_weapon", 's', "Wheel, 1" },
	{ "controls.switch_grenade", 's', "X" },
	{ "controls.action", 's', "E" },
	{ "controls.flashlight", 's', "Q" },
	{ "controls.scoreboard", 's', "Tab" },
	{ "controls.pause", 's', "Escape" },
	{ "controls.screenshot", 's', "F10" },
	{ "controls.push_to_talk", 's', "V" },
	{ "controls.chat_all", 's', "T" },
	{ "controls.chat_team", 's', "Y" },
	{ "controls.chat_log", 's', "H" },
	{ "game.enhanced_animations", 'b', "true" },
	{ "game.console_log", 's', "important" },
	{ "game.language", 's', "" },
	{ "game.unlock_all_levels", 'b', "false" },
	{ "game.downloaded_maps", 's', "" },
	{ "game.bomb_spawns", 's', "" },
	{ "game.bomb_arm_time", 'r', "1.0" },
	{ "game.bomb_open_pitch_speed", 'i', "0" },
	{ "game.bomb_open_pitch_carrier_health", 'i', "0" },
	{ "game.bomb_open_pitch_carrier_resistance", 'i', "0" },
	{ "game.bomb_open_pitch_carrier_shields", 's', "none" },
	{ "game.bomb_open_pitch_fumble", 'b', "true" },
	{ "game.move_old_map_folders", 's', "ask" },
	{ "game.custom_edition", 'b', "true" },
	{ "paths.data", 's', "" },
	{ "paths.saves", 's', "" },
	{ "paths.custom_edition", 's', "" },
	{ "paths.logs", 's', "" },
	{ "paths.cache", 's', "" },
	{ "data.download_url", 's', "https://halo.fractumseraph.net/assets/maps/" },
	{ "network.address", 's', "" },
	{ "network.broadcast", 's', "" },
	{ "network.online", 'b', "true" },
	{ "network.lasting_invite", 'b', "false" },
	{ "network.shared_bans", 's', "" },
	{ "network.fair_play", 's', "refuse" },
	{ "network.record_games", 'b', "false" },
	{ "network.recordings_megabytes", 'i', "2048" },
	{ "network.record_public", 'b', "false" },
	{ "network.record_public_delay", 'i', "30" },
	{ "network.record_public_megabytes", 'i', "2048" },
	{ "network.join_from_clipboard", 's', "ask" },
	{ "network.map_downloads", 's', "ask" },
	{ "network.tunnel_port", 'i', "0" },
	{ "network.allow_upnp", 'b', "true" },
	{ "network.protocol", 's', "auto" },
	{ "network.chat", 's', "on" },
	{ "network.team_change", 's', "balanced" },
	{ "network.team_change_margin", 'i', "1" },
	{ "network.chat_filter", 'b', "true" },
	{ "network.share_profile", 'b', "false" },
	{ "network.platform_limits", 's', "on" },
	{ "network.host_platform_limits", 'b', "true" },
	{ "network.public_lobby", 'b', "true" },
	{ "network.ping_games", 'b', "true" },
	{ "network.host_public", 'b', "true" },
	{ "network.voice_lobby", 'b', "true" },
	{ "network.voice_mode", 's', "team_global_enemy_proximity" },
	{ "network.voice_kbps", 'i', "24" },
	{ "network.voice_proximity", 'r', "15.0" },
	{ "network.votekick", 'b', "true" },
	{ "network.votekick_minutes", 'i', "5" },
	{ "network.votekick_ban_minutes", 'i', "30" },
	{ "network.map_share", 'b', "true" },
	{ "network.map_share_upload", 'i', "1024" },
	{ "network.map_seed", 'b', "true" },
	{ "network.map_seed_upload", 'i', "256" },
	{ "network.map_download", 'b', "true" },
	{ "network.coop_public", 'b', "false" },
	{ "network.coop_friendly_fire", 's', "on" },
	{ "network.coop_player_collisions", 'b', "true" },
	{ "network.coop_enemies_mode", 's', "per_player" },
	{ "network.coop_enemies", 'i', "50" },
	{ "network.coop_enemies_multiplier", 'i', "2" },
	{ "network.brokers_file", 's', "brokers.txt" },
	{ "network.stun_servers", 's', "stun.l.google.com:19302,stun.cloudflare.com:3478" },
	{ "network.legacy_table", 's', "" },
	{ "network.browser_url", 's', "https://halo.milenko.org" },
	{ "network.list_hosted_games", 'b', "true" },
	{ "network.report_hosted_games", 'b', "true" },
	{ "network.report_joined_games", 'b', "true" },
	{ "network.report_events", 'b', "true" },
	{ "network.events_token", 's', "" },
	{ "network.events_positions", 'i', "2" },
	{ "network.events_limit", 'i', "40000" },
	{ "network.events_part_minutes", 'i', "30" },
	{ "network.events_folder", 's', "" },
	{ "discord.application_id", 's', "1560397272166957136" },
	{ "discord.presence", 's', "full" },
	{ "discord.show_server_name", 'b', "true" },
	{ "discord.show_join", 'b', "true" },
	{ "maps.torrents", 'b', "true" },
	{ "maps.download_screen", 'b', "true" },
	{ "maps.seed", 's', "host" },
	{ "maps.upload_limit", 'i', "512" },
	{ "maps.download_limit", 'i', "0" },
	{ "maps.torrent_port", 'i', "0" },
	{ "maps.dht", 'b', "true" },
	{ "maps.trackers", 's', "udp://halovps.fractumseraph.net:6969/announce,http://halovps.fractumseraph.net:6969/announce" },
	{ "maps.web_seeds", 's', "http://halomaps.fractumseraph.net/maps/" },
	{ "maps.archives", 's', "halonet,halomaps" },
	{ "maps.torrent_index", 's', "map_torrents.txt" },
	{ "update.auto", 'b', "true" },
	{ "crash_reports.upload", 's', "yes" },
	{ "debug.network_test", 's', "" },
	{ "debug.network_test_start", 'r', "15.0" },
	{ "debug.network_test_kill", 'r', "0.0" },
	{ "debug.network_test_team_change", 'r', "0.0" },
	{ "debug.network_test_pause", 'r', "0.0" },
	{ "debug.network_test_score", 'i', "0" },
	{ "debug.network_test_aim", 'b', "false" },
	{ "debug.network_test_cheat", 's', "" },
	{ "debug.network_test_watch", 'b', "false" },
	{ "debug.network_test_shoot", 'r', "0.0" },
	{ "debug.network_test_vehicle", 'r', "0.0" },
	{ "debug.network_test_pickup", 'r', "0.0" },
	{ "debug.network_test_local_players", 'i', "1" },
	{ "debug.network_test_bomb", 'r', "0.0" },
	{ "debug.network_test_bomb_in_goal", 'b', "false" },
	{ "debug.network_test_bomb_throw", 'b', "false" },
	{ "debug.network_test_bomb_keep", 'b', "false" },
	{ "debug.network_test_bomb_save", 'b', "false" },
	{ "debug.network_test_fall", 'r', "0.0" },
	{ "debug.network_test_melee", 'r', "0.0" },
	{ "debug.network_test_pickup_weapon", 's', "" },
	{ "debug.network_test_public", 'b', "false" },
	{ "debug.telnet_console", 'b', "false" },
	{ "debug.telnet_console_port", 'i', "2323" },
	{ "debug.log_addresses", 'b', "false" },
	{ "debug.touch_targets", 'b', "false" },
	{ "debug.solo_game", 'b', "false" },
	{ "debug.network_latency", 'r', "0.0" },
	{ "debug.network_loss", 'r', "0.0" },
	{ "debug.network_corrupt", 'r', "0.0" },
	{ "debug.network_corrupt_stream", 'r', "0.0" },
	{ "debug.network_corrupt_after", 'r', "0.0" },
	{ "debug.test_input", 's', "" },
	{ "debug.update_answer", 's', "" },
	{ "debug.chat_test", 's', "" },
	{ "debug.exit_after", 'r', "0.0" },
	{ "debug.hidden_window", 'b', "false" },
	{ "debug.voice_test", 'b', "false" },
	{ "debug.null_renderer", 'b', "false" },
	{ "debug.gl_debug", 'b', "false" },
	{ "debug.menu_open", 's', "" },
	{ "debug.gpu_flush_draws", 'i', "-1" },
	{ "debug.gpu_stats", 'b', "false" },
	{ "debug.gpu_trace_frame", 'i', "-1" },
	{ "debug.gpu_trace_constants", 'b', "false" },
	{ "debug.gpu_skip_vertex_shaders", 's', "" },
	{ "debug.gpu_dump_shaders", 's', "" },
	{ "debug.gpu_debug_expression", 's', "" },
	{ "debug.gpu_debug_texture0", 'b', "false" },
	{ "debug.gpu_debug_flat", 'b', "false" },
	{ "debug.screenshot_directory", 's', "" },
	{ "debug.screenshot_every", 'i', "0" },
	{ "debug.screenshot_window", 'b', "false" },
	{ "debug.texture_dump_directory", 's', "" },
	{ "debug.discord_log", 'b', "false" },
	{ "debug.texture_log", 'b', "false" },
	{ "debug.texture_no_cache", 'b', "false" },
	{ "debug.sample_seconds", 'r', "0.0" },
	{ "debug.memory_watch", 'b', "true" },
	{ "debug.profile_record", 'b', "false" },
	{ "debug.profile_record_when", 's', "start" },
	{ "debug.profile_memory", 'i', "256" },
};

static const char *n3ds_fork_setting(const char *name)
{
	unsigned int i;
	for (i = 0; i < sizeof(n3ds_fork_settings) / sizeof(n3ds_fork_settings[0]); i++)
		if (!strcmp(n3ds_fork_settings[i].name, name))
			return n3ds_fork_settings[i].value;
	return "";
}

int config_boolean(const char *name) { return !strcmp(n3ds_fork_setting(name), "true"); }
long config_integer(const char *name) { return strtol(n3ds_fork_setting(name), NULL, 0); }
double config_real(const char *name) { return strtod(n3ds_fork_setting(name), NULL); }
const char *config_string(const char *name) { return n3ds_fork_setting(name); }
unsigned long config_changes(void) { return 0; }
