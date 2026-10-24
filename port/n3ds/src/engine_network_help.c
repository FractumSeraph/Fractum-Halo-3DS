/* The bottom screen's lines at the menus for the FractumSeraph/OpenCE
 * fork's internet play (Halo3DS's n3ds_gpu_campaign_help draws them, in
 * place of its campaign help): while this console hosts a game on the
 * internet, its invite, which on the desktops is on the clipboard and here
 * is read off the screen (48 digits, in three rows); in Online Games, its
 * controls, the keyboard for an invite among them (engine_invite_keyboard.c). */
#include "cseries.h"

#include <stdio.h>
#include <string.h>

int p2p_hosting_invite(char *text, int size);
int p2p_hosting_listable_invite(char *text, int size);
boolean browser_screen_active(void);

enum { HELP_LINES = 6, HELP_WIDTH = 41 };

static char help[HELP_LINES][HELP_WIDTH];
int n3ds_update_help_line(char *line, int size); /* (engine_update.c) */

int n3ds_menu_network_help(const char **lines, int maximum)
{
	char invite[160];
	int count = 0;

	if (maximum > HELP_LINES)
		maximum = HELP_LINES;
	/* port: the self-updater's progress or result first (engine_update.c) */
	if (n3ds_update_help_line(help[count], HELP_WIDTH))
		count++;
	if (p2p_hosting_invite(invite, sizeof(invite)) > 0 && invite[0])
	{
		size_t length = strlen(invite);
		size_t row;

		snprintf(help[count++], HELP_WIDTH, "%s", p2p_hosting_listable_invite(NULL, 0) > 0 ?
			"Online: in Online Games, or by invite:" : "Private game: friends join by invite:");
		snprintf(help[count++], HELP_WIDTH, "halo://join/");
		/* (the digits in rows of 16: 40 characters a line here) */
		for (row = 0; row < length && count < maximum; row += 16)
			snprintf(help[count++], HELP_WIDTH, "%.16s", invite + row);
	}
	else if (browser_screen_active())
	{
		snprintf(help[count++], HELP_WIDTH, "A: join   X: refresh   Y: create game");
		snprintf(help[count++], HELP_WIDTH, "SELECT: join a game by its invite");
		snprintf(help[count++], HELP_WIDTH, "Tap a game to pick it, again to join");
	}
	for (int i = 0; i < count && i < maximum; i++)
		lines[i] = help[i];
	return count < maximum ? count : maximum;
}
