/* Invites on the 3DS, which has no clipboard: the FractumSeraph/OpenCE
 * fork's desktops take an invite from the clipboard (invite_prompt.c). Here
 * it is typed: Online Games' SELECT (browser_screen.c, on the 3DS) asks for
 * the system's keyboard, which opens between two frames (it is an applet:
 * never in a frame), and the game typed is joined as a picked one is. */
#include <3ds.h>
#include <stdio.h>
#include <string.h>

void n3ds_log(const char *message);

enum { INVITE_TEXT = 160, PASSWORD_TEXT = 33 /* (invite_prompt.c's PASSWORD_LENGTH, 32) */ };

static volatile int keyboard_requested;

/* (browser_screen.c's SELECT) */
void n3ds_invite_keyboard_request(void)
{
    keyboard_requested = 1;
    n3ds_log("Internet play: the keyboard is asked for an invite");
}

/* a game's password, for a locked invite (invite_prompt.c's dialog, which
 * asks with platform_text_field: the desktops' typing) */
static volatile int password_requested;

void platform_text_field(int typing)
{
    password_requested = typing;
    if (typing)
        n3ds_log("Internet play: the keyboard is asked for a game's password");
}

static void password_keyboard(void)
{
    extern void invite_prompt_password_typed(const char *typed, unsigned char join);
    SwkbdState keyboard;
    char text[PASSWORD_TEXT];
    SwkbdButton button;

    password_requested = 0;
    swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 2, PASSWORD_TEXT - 1);
    swkbdSetHintText(&keyboard, "The game's password");
    swkbdSetPasswordMode(&keyboard, SWKBD_PASSWORD_HIDE_DELAY);
    swkbdSetButton(&keyboard, SWKBD_BUTTON_LEFT, "Leave", false);
    swkbdSetButton(&keyboard, SWKBD_BUTTON_RIGHT, "Join", true);
    swkbdSetValidation(&keyboard, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    text[0] = 0;
    button = swkbdInputText(&keyboard, text, sizeof(text));
    n3ds_log(button == SWKBD_BUTTON_RIGHT && text[0] ? "Internet play: a password was typed" :
        "Internet play: no password typed");
    /* (a wrong one asks again: invite_prompt.c) */
    invite_prompt_password_typed(text, button == SWKBD_BUTTON_RIGHT && text[0]);
}

/* (the frame loop, engine_frontend_boot.c, between frames) */
void n3ds_invite_keyboard_poll(void)
{
    SwkbdState keyboard;
    char text[INVITE_TEXT];
    SwkbdButton button;

    if (password_requested)
        password_keyboard();
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
