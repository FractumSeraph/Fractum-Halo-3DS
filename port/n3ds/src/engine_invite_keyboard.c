/* Invites on the 3DS, which has no clipboard: the FractumSeraph/OpenCE
 * fork's desktops take an invite from the clipboard (invite_prompt.c). Here
 * it is typed: Online Games' SELECT (browser_screen.c, on the 3DS) asks for
 * the system's keyboard, which opens between two frames (it is an applet:
 * never in a frame), and the game typed is joined as a picked one is. */
#include <3ds.h>
#include <stdio.h>
#include <string.h>

void n3ds_log(const char *message);

enum { INVITE_TEXT = 160 };

static volatile int keyboard_requested;

/* (browser_screen.c's SELECT) */
void n3ds_invite_keyboard_request(void)
{
    keyboard_requested = 1;
    n3ds_log("Internet play: the keyboard is asked for an invite");
}

/* (the frame loop, engine_frontend_boot.c, between frames) */
void n3ds_invite_keyboard_poll(void)
{
    SwkbdState keyboard;
    char text[INVITE_TEXT];
    SwkbdButton button;

    if (!keyboard_requested)
        return;
    keyboard_requested = 0;
    n3ds_log("Internet play: the keyboard opens");
    swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 2, INVITE_TEXT - 1);
    swkbdSetHintText(&keyboard, "An invite: halo://join/... or its digits");
    swkbdSetButton(&keyboard, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&keyboard, SWKBD_BUTTON_RIGHT, "Join", true);
    swkbdSetValidation(&keyboard, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    text[0] = 0;
    button = swkbdInputText(&keyboard, text, sizeof(text));
    if (button != SWKBD_BUTTON_RIGHT || !text[0])
    {
        char message[96];

        snprintf(message, sizeof(message), "Internet play: no invite typed (keyboard button %d, result %d)", (int)button,
            (int)swkbdGetResult(&keyboard));
        n3ds_log(message);
        return;
    }
    /* (joined from Online Games, as a picked game is: browser_screen.c) */
    {
        extern unsigned char browser_screen_join_typed_invite(const char *typed);

        n3ds_log("Internet play: an invite was typed");
        browser_screen_join_typed_invite(text);
    }
}

/* port/linux/src/sdl_platform.c's: the clipboard's invite, which the
 * invite prompt asks about; none on the 3DS (a typed one is joined
 * directly, above) */
int platform_take_clipboard_invite(char *invite, int size)
{
    if (invite && size > 0)
        invite[0] = 0;
    return 0;
}
