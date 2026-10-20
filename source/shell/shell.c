/*
SHELL.C
*/

/* ---------- headers */

#include "cseries.h"
#include "shell.h"
#include "errors.h"
#include "tag_files.h"
#include "files.h"
#include "real_math.h"
#include "game_state.h"
#include "rasterizer.h"
#include "integer_math.h"
#include "input.h"
#ifdef HALO_3DS
/* port: the 3DS's watchdog (port/3ds/include/n3ds_watch.h) */
#include "n3ds_watch.h"
#else
#define N3DS_STEP(what) ((void)0)
#endif
#include "sound_manager.h"

/* ---------- globals */

static boolean application_paused;

/* ---------- public code */

boolean shell_initialize(
	void)
{
	boolean success = FALSE;
	boolean platform_initialized = FALSE;
	
	cseries_initialize();
	platform_initialized = shell_platform_initialize();
	
	if (platform_initialized)
	{
		boolean rasterizer_initialized;
		
		/* port: (each step named for the 3DS's watchdog) */
		N3DS_STEP("errors_initialize");
		errors_initialize();
		N3DS_STEP("tag_files_open");
		tag_files_open();
		/* port: Custom Edition maps live in custom_maps, beside maps.
		Make that folder when this launch does not already have it. */
		N3DS_STEP("custom_maps folder");
		directory_create("custom_maps");
		real_math_initialize();
		N3DS_STEP("game_state_initialize");
		game_state_initialize();
		
		N3DS_STEP("rasterizer_initialize");
		rasterizer_initialized = rasterizer_initialize();
		
		if (rasterizer_initialized)
		{
			N3DS_STEP("input_initialize");
			input_initialize();
			N3DS_STEP("sound_initialize");
			sound_initialize();
			N3DS_STEP("shell initialized");
			
			success = TRUE;
		}
			
		shell_platform_verify();
	}
	
	return success;
}

void shell_dispose(
	void)
{
	sound_dispose();
	input_dispose();
	rasterizer_dispose();
	real_math_dispose();
	tag_files_close();
	errors_dispose();
	shell_platform_dispose();
	cseries_dispose();

	return;
}

boolean shell_application_is_paused(
	void)
{
	return application_paused;
}

void shell_application_pause(
	boolean paused)
{
	if (application_paused!=paused)
	{
		application_paused = paused;
		shell_screen_pause(paused);
	}

	return;
}

boolean shell_running_import_tool(void)
{
	return FALSE;
}
