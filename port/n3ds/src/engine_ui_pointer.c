/* The menus' pointer on the 3DS (port/linux/include/halo_ui_pointer.h), as
 * the FractumSeraph/OpenCE fork's Android build makes it of its touchscreen
 * (port/linux/src/touch_input.c, with its touch_menu.c): a tap clicks where
 * it went down, a drag scrolls. The menus are on the top screen and the
 * touchscreen is the bottom one, so the bottom screen's 320x240 stands for
 * the menus' 640x480 (twice as large): a tap goes where the same place is on
 * the top screen. The fork's screens over the menus (Online Games' list,
 * its prompts) and the menus' own items take it. */
#include "../../linux/include/halo_ui_pointer.h"
#include "../../linux/src/touch_menu.h"
#include "engine_input.h"

#include <stdio.h>
#include <string.h>

void n3ds_log(const char *message);

void n3ds_engine_input_last(struct native_input_sample *sample);

enum
{
    POINTER_SCALE = 2,
    POINTER_FINGER = 1,
};

static struct touch_menu menu;
static int menu_ready;
static int menu_active;
static int finger_down;
static float finger_x, finger_y;

int halo_ui_pointer_update(int menus_active, struct halo_ui_pointer *pointer)
{
    struct native_input_sample sample;
    struct touch_menu_output output;
    int touching;
    float x, y;

    if (!menu_ready)
    {
        struct touch_menu_settings settings;

        memset(&settings, 0, sizeof(settings));
        /* (in the menus' units: a tap moves at most 6 of the bottom
         * screen's pixels; 20 of them are a wheel step) */
        settings.slop = 12.0f;
        settings.step = 40.0f;
        settings.steps_per_read = 1;
        settings.width = 640.0f;
        settings.height = 480.0f;
        touch_menu_init(&menu, &settings);
        menu_ready = 1;
    }
    /* (what a finger began before a menu opened or closed must not click
     * in it) */
    if (!menus_active != !menu_active)
    {
        touch_menu_reset(&menu);
        menu_active = menus_active;
        finger_down = 0;
    }
    if (!menus_active || !pointer)
        return 0;
    n3ds_engine_input_last(&sample);
    touching = (sample.held & NATIVE_KEY_TOUCH) != 0 && sample.touch_x < 320 && sample.touch_y < 240;
    x = (float)(sample.touch_x * POINTER_SCALE);
    y = (float)(sample.touch_y * POINTER_SCALE);
    if (touching && !finger_down)
        touch_menu_down(&menu, POINTER_FINGER, x, y);
    else if (touching)
        touch_menu_move(&menu, POINTER_FINGER, x, y);
    else if (finger_down)
        touch_menu_up(&menu, POINTER_FINGER, finger_x, finger_y);
    finger_down = touching;
    if (touching)
    {
        finger_x = x;
        finger_y = y;
    }
    touch_menu_read(&menu, &output);
    /* (each tap, in the menus' units, for checking where taps land) */
    if (output.clicks)
    {
        char text[96];

        snprintf(text, sizeof(text), "TOUCH: tap at %d,%d of the menus' 640x480", (int)output.click_x, (int)output.click_y);
        n3ds_log(text);
    }
    pointer->x = (short)output.x;
    pointer->y = (short)output.y;
    pointer->click_x = (short)output.click_x;
    pointer->click_y = (short)output.click_y;
    pointer->moved = output.moved ? 1 : 0;
    pointer->left_clicks = (unsigned char)output.clicks;
    pointer->right_clicks = 0;
    pointer->wheel_steps = (signed char)output.wheel_steps;
    pointer->touch = 1;
    pointer->downs = (unsigned char)output.downs;
    pointer->down_x = (short)output.down_x;
    pointer->down_y = (short)output.down_y;
    return 1;
}

/* the open scoreboard's pointer: none here, as on Android (the scoreboard is
 * turned with the buttons) */
int halo_scoreboard_pointer_update(int offered, struct halo_ui_pointer *pointer)
{
    (void)offered;
    (void)pointer;
    return -1;
}
