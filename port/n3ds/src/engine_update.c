/* Fractum Halo's self-updater on the 3DS. A release's build (built with
 * --release: port/n3ds/build.py) asks GitHub for the latest release of
 * FractumSeraph/Fractum-Halo-3DS when the game starts, on a thread of its own
 * below the game's, once the console has a network address. When that
 * release is newer than this build (port/n3ds/VERSION), the menus ask the
 * player, with the system's keyboard applet between two frames as the invite
 * does (engine_invite_keyboard.c): Update, Not now, or Never (update.auto,
 * the fork's setting, written false).
 *
 * Update downloads the release's Halo3DS.3dsx and map_torrents.txt over
 * HTTPS (port/linux/src/posix_browser.c, its certificate checked against the
 * built-in roots: engine_tls_roots.h has GitHub's), each beside the file it
 * replaces as <name>.new, its size and SHA-256 checked against what GitHub's
 * API gives for the release's file (its "digest", which GitHub computes on
 * upload). Only then is each swapped in: the old one renamed <name>.old, the
 * new one renamed into its place, the old deleted (put back if the rename
 * fails). The running game is in memory (the Homebrew Launcher loaded it), so
 * its file can be replaced; the new version runs from the next start, which
 * the bottom screen says. A file that fails its check, or a download that
 * stops, leaves the installed game as it was. No signing key: the user chose
 * SHA-256 over HTTPS (2026-10-24).
 *
 * The bottom screen's menu lines show the update's state
 * (n3ds_update_help_line: engine_network_help.c). debug.update_answer
 * ("yes", "no", "never"; and "yes-bad-hash", which checks against a wrong
 * hash, for the updater's test) answers without asking. */
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psa/crypto.h"

void n3ds_log(const char *message);
int config_boolean(const char *name);
const char *config_string(const char *name);
int config_write_boolean(const char *name, int value);
int posix_browser_request_as(const char *url, const char *form, const char *content_type, const char *user_agent,
	char *response, int response_size, char *error, int error_size);
int posix_browser_download_open(const char *url, const char *user_agent, int *status, int *length, char *error,
	int error_size);
int posix_browser_download_read(int handle, void *buffer, int size);
void posix_browser_download_close(int handle);
extern int __system_argc;
extern char **__system_argv;

#define UPDATE_STRING_(x) #x
#define UPDATE_STRING(x) UPDATE_STRING_(x)
#ifdef HALO_N3DS_VERSION_NUMBER
#define HALO_N3DS_VERSION UPDATE_STRING(HALO_N3DS_VERSION_NUMBER)
#else
#define HALO_N3DS_VERSION "dev"
#endif
#define UPDATE_REPOSITORY "FractumSeraph/Fractum-Halo-3DS"
#define UPDATE_TAG_PREFIX "fractum-v"
#define UPDATE_DOWNLOAD_PREFIX "https://github.com/" UPDATE_REPOSITORY "/releases/download/"
#define UPDATE_USER_AGENT "FractumHalo-3DS/" HALO_N3DS_VERSION
#define UPDATE_GAME_PATH "sdmc:/3ds/Halo3DS/Halo3DS.3dsx"
#define UPDATE_MAPS_PATH "sdmc:/halo-source/map_torrents.txt"

enum
{
	_update_none,
	_update_checking,
	_update_available,
	_update_asked,
	_update_downloading,
	_update_done,
	_update_failed,
};

struct update_file
{
	const char *name;
	char url[384];
	unsigned long size;
	unsigned char sha256[32];
	int found;
};

static Thread update_thread;
static volatile int update_state;
static volatile int update_percent;
static char update_version[32];
static char update_message[96];
static struct update_file update_files[2] = { { "Halo3DS.3dsx" }, { "map_torrents.txt" } };
static int update_bad_hash;

/* ---------- versions */

/* a version's three numbers: X.Y.Z (a letter after, a hotfix, compared too) */
static int update_parse_version(const char *text, long numbers[3], const char **suffix)
{
	for (int part = 0; part < 3; part++)
	{
		char *end;

		if (*text < '0' || *text > '9')
			return 0;
		numbers[part] = strtol(text, &end, 10);
		text = end;
		if (part < 2 && *text++ != '.')
			return 0;
	}
	*suffix = text;
	return 1;
}

static int update_newer(const char *version)
{
	long latest[3], current[3];
	const char *latest_suffix, *current_suffix;

	if (!update_parse_version(version, latest, &latest_suffix) ||
		!update_parse_version(HALO_N3DS_VERSION, current, &current_suffix))
		return 0;
	for (int part = 0; part < 3; part++)
		if (latest[part] != current[part])
			return latest[part] > current[part];
	return strcmp(latest_suffix, current_suffix) > 0;
}

/* ---------- the release (GitHub's API) */

/* the string value after "key": in text (no escapes in the values read here) */
static int update_json_string(const char *text, const char *key, char *value, size_t size)
{
	char pattern[48];
	const char *at;
	size_t length;

	snprintf(pattern, sizeof(pattern), "\"%s\"", key);
	at = strstr(text, pattern);
	if (!at)
		return 0;
	at = strchr(at + strlen(pattern), ':');
	if (!at)
		return 0;
	at = strchr(at, '"');
	if (!at)
		return 0;
	at++;
	length = strcspn(at, "\"");
	if (length >= size)
		return 0;
	memcpy(value, at, length);
	value[length] = 0;
	return 1;
}

static int update_hex(const char *text, unsigned char *bytes, int count)
{
	for (int i = 0; i < count; i++)
	{
		unsigned int value;

		if (sscanf(text + i * 2, "%2x", &value) != 1)
			return 0;
		bytes[i] = (unsigned char)value;
	}
	return 1;
}

/* each file of the release: its download address, size and SHA-256, from
   the asset whose "name" is the file's (the asset's fields up to the next) */
static void update_release_files(const char *release)
{
	const char *assets = strstr(release, "\"assets\"");

	for (int f = 0; f < 2 && assets; f++)
	{
		struct update_file *file = &update_files[f];
		char pattern[64];
		const char *asset;
		char field[400];

		file->found = 0;
		snprintf(pattern, sizeof(pattern), "\"name\":\"%s\"", file->name);
		asset = strstr(assets, pattern);
		if (!asset)
		{
			snprintf(pattern, sizeof(pattern), "\"name\": \"%s\"", file->name);
			asset = strstr(assets, pattern);
		}
		if (!asset)
			continue;
		/* (the asset's object: from its "url" before the name to its end) */
		{
			const char *start = asset;
			const char *end = strstr(asset, "\"browser_download_url\"");
			char object[2048];
			size_t length;

			while (start > assets && *start != '{')
				start--;
			if (!end)
				continue;
			end = strchr(end + 22, '}');
			if (!end || (size_t)(end - start) >= sizeof(object))
				continue;
			length = (size_t)(end - start);
			memcpy(object, start, length);
			object[length] = 0;
			/* (only this repository's release files) */
			if (!update_json_string(object, "browser_download_url", file->url, sizeof(file->url)) ||
				strncmp(file->url, UPDATE_DOWNLOAD_PREFIX, strlen(UPDATE_DOWNLOAD_PREFIX)))
				continue;
			{
				const char *size = strstr(object, "\"size\"");

				size = size ? strchr(size + 6, ':') : NULL;
				file->size = size ? strtoul(size + 1, NULL, 10) : 0;
			}
			if (!update_json_string(object, "digest", field, sizeof(field)) || strncmp(field, "sha256:", 7) ||
				strlen(field) != 7 + 64 || !update_hex(field + 7, file->sha256, 32) || !file->size)
				continue;
			file->found = 1;
		}
	}
}

/* ---------- the check (the update's thread) */

static int update_wait_for_network(void)
{
	for (int seconds = 0; seconds < 120; seconds++)
	{
		if (gethostid() != 0)
			return 1;
		svcSleepThread(1000000000LL);
	}
	return 0;
}

static void update_check(void)
{
	static char response[65536];
	char error[256];
	char tag[48];
	int status;

	update_state = _update_checking;
	if (!update_wait_for_network())
	{
		n3ds_log("update: no network; not checked");
		update_state = _update_none;
		return;
	}
	status = posix_browser_request_as("https://api.github.com/repos/" UPDATE_REPOSITORY "/releases/latest", NULL, NULL,
		UPDATE_USER_AGENT, response, (int)sizeof(response), error, (int)sizeof(error));
	if (status != 200 || !update_json_string(response, "tag_name", tag, sizeof(tag)) ||
		strncmp(tag, UPDATE_TAG_PREFIX, strlen(UPDATE_TAG_PREFIX)))
	{
		char message[384];

		snprintf(message, sizeof(message), "update: could not check for a new version (HTTP %d%s%s)", status,
			error[0] ? ": " : "", error);
		n3ds_log(message);
		update_state = _update_none;
		return;
	}
	snprintf(update_version, sizeof(update_version), "%s", tag + strlen(UPDATE_TAG_PREFIX));
	if (!update_newer(update_version))
	{
		char message[128];

		snprintf(message, sizeof(message), "update: this is the latest version (%s; latest %s)", HALO_N3DS_VERSION,
			update_version);
		n3ds_log(message);
		update_state = _update_none;
		return;
	}
	update_release_files(response);
	if (!update_files[0].found)
	{
		char message[160];

		snprintf(message, sizeof(message), "update: %s is out, but its Halo3DS.3dsx (with its SHA-256) is not listed",
			update_version);
		n3ds_log(message);
		update_state = _update_none;
		return;
	}
	{
		char message[160];

		snprintf(message, sizeof(message), "update: Fractum Halo %s is available (this is %s)", update_version,
			HALO_N3DS_VERSION);
		n3ds_log(message);
	}
	update_state = _update_available;
}

/* ---------- the download (the update's thread) */

/* the file downloaded to <path>.new, its size and SHA-256 checked; 1 when it
   is there and right */
static int update_download_file(const struct update_file *file, const char *path, unsigned long *done,
	unsigned long total)
{
	char partial[256];
	char error[256];
	static unsigned char buffer[16384];
	psa_hash_operation_t hash = PSA_HASH_OPERATION_INIT;
	unsigned char digest[32];
	size_t digest_length = 0;
	unsigned long received = 0;
	int status = 0, length = -1, handle, read, good = 0;
	FILE *out;

	snprintf(partial, sizeof(partial), "%s.new", path);
	remove(partial);
	handle = posix_browser_download_open(file->url, UPDATE_USER_AGENT, &status, &length, error, (int)sizeof(error));
	if (!handle)
	{
		snprintf(update_message, sizeof(update_message), "Update failed: %s could not be downloaded", file->name);
		{
			char message[384];

			snprintf(message, sizeof(message), "update: %s: HTTP %d: %s", file->name, status, error);
			n3ds_log(message);
		}
		return 0;
	}
	out = fopen(partial, "wb");
	if (!out || psa_hash_setup(&hash, PSA_ALG_SHA_256) != PSA_SUCCESS)
	{
		if (out)
			fclose(out);
		posix_browser_download_close(handle);
		snprintf(update_message, sizeof(update_message), "Update failed: %s could not be written", file->name);
		return 0;
	}
	while (received <= file->size && (read = posix_browser_download_read(handle, buffer, (int)sizeof(buffer))) > 0)
	{
		if (fwrite(buffer, 1, (size_t)read, out) != (size_t)read || psa_hash_update(&hash, buffer, (size_t)read) != PSA_SUCCESS)
			break;
		received += (unsigned long)read;
		*done += (unsigned long)read;
		update_percent = total ? (int)((unsigned long long)*done * 100 / total) : 0;
	}
	posix_browser_download_close(handle);
	good = fclose(out) == 0 && received == file->size &&
		psa_hash_finish(&hash, digest, sizeof(digest), &digest_length) == PSA_SUCCESS && digest_length == 32;
	psa_hash_abort(&hash);
	{
		unsigned char expected[32];

		memcpy(expected, file->sha256, 32);
		if (update_bad_hash)
			expected[0] ^= 0xFF;
		if (good && memcmp(digest, expected, 32))
		{
			char message[160];

			snprintf(message, sizeof(message), "update: %s's SHA-256 is not the release's: not installed", file->name);
			n3ds_log(message);
			snprintf(update_message, sizeof(update_message), "Update failed: %s did not check out", file->name);
			good = 0;
		}
		else if (!good)
		{
			char message[160];

			snprintf(message, sizeof(message), "update: %s: %lu of %lu bytes downloaded: not installed", file->name,
				received, file->size);
			n3ds_log(message);
			snprintf(update_message, sizeof(update_message), "Update failed: %s's download stopped", file->name);
		}
	}
	if (!good)
		remove(partial);
	return good;
}

/* <path>.new put in place of path, the old one kept until it is */
static int update_swap(const char *path)
{
	char partial[256], old[256];

	snprintf(partial, sizeof(partial), "%s.new", path);
	snprintf(old, sizeof(old), "%s.old", path);
	remove(old);
	/* (an old file that cannot be moved aside: nothing replaced; none there
	   at all, the new one simply put in place) */
	if (rename(path, old))
	{
		FILE *existing = fopen(path, "rb");

		if (existing)
		{
			fclose(existing);
			return 0;
		}
	}
	if (rename(partial, path))
	{
		rename(old, path);
		return 0;
	}
	remove(old);
	return 1;
}

static const char *update_game_path(void)
{
	/* (the file the Homebrew Launcher started: its argv[0], an sdmc: path) */
	if (__system_argc > 0 && __system_argv && __system_argv[0] && !strncmp(__system_argv[0], "sdmc:/", 6) &&
		strlen(__system_argv[0]) > 5 && !strcmp(__system_argv[0] + strlen(__system_argv[0]) - 5, ".3dsx"))
		return __system_argv[0];
	return UPDATE_GAME_PATH;
}

static void update_download(void)
{
	const char *paths[2] = { update_game_path(), UPDATE_MAPS_PATH };
	unsigned long total = 0, done = 0;
	int got[2] = { 0, 0 };

	update_state = _update_downloading;
	update_percent = 0;
	psa_crypto_init();
	for (int f = 0; f < 2; f++)
		if (update_files[f].found)
			total += update_files[f].size;
	for (int f = 0; f < 2; f++)
	{
		if (!update_files[f].found)
			continue;
		got[f] = update_download_file(&update_files[f], paths[f], &done, total);
		if (!got[f])
		{
			update_state = _update_failed;
			return;
		}
	}
	/* (all downloaded and checked: then each put in place) */
	for (int f = 0; f < 2; f++)
	{
		if (got[f] && !update_swap(paths[f]))
		{
			char message[160];

			snprintf(message, sizeof(message), "update: %s could not be put in place of %s", update_files[f].name, paths[f]);
			n3ds_log(message);
			snprintf(update_message, sizeof(update_message), "Update failed: %s could not be replaced", update_files[f].name);
			update_state = _update_failed;
			return;
		}
	}
	{
		char message[160];

		snprintf(message, sizeof(message), "update: Fractum Halo %s installed (%s); it runs from the next start",
			update_version, paths[0]);
		n3ds_log(message);
	}
	snprintf(update_message, sizeof(update_message), "Updated to %s: restart Fractum Halo", update_version);
	update_state = _update_done;
}

static volatile int update_download_wanted;

static void update_worker(void *unused)
{
	(void)unused;
	update_check();
	while (update_state == _update_available || update_state == _update_asked)
	{
		if (update_download_wanted)
		{
			update_download();
			break;
		}
		svcSleepThread(100000000LL);
	}
}

/* ---------- the game thread's part */

/* at start-up (engine_main.c, after the socket service): a release's build
   with update.auto looks for a newer release */
void n3ds_update_start(void)
{
#ifdef HALO_N3DS_RELEASE
	s32 priority = 0x30;

	if (update_thread || !config_boolean("update.auto"))
		return;
	svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
	/* (a TLS handshake's stack: 64 KB) */
	update_thread = threadCreate(update_worker, NULL, 64 * 1024, priority < 0x3e ? priority + 2 : priority, -2, true);
	if (!update_thread)
		n3ds_log("update: no thread for the check");
#endif
}

/* between frames, in the menus (engine_frontend_boot.c): asks the player
   once a newer release is found */
void n3ds_update_poll(int in_menus)
{
	SwkbdState keyboard;
	SwkbdButton button;
	char hint[96];
	char text[8];
	const char *answer;

	if (update_state != _update_available || !in_menus)
		return;
	update_state = _update_asked;
	answer = config_string("debug.update_answer");
	if (answer && answer[0])
	{
		char message[96];

		snprintf(message, sizeof(message), "update: answering %s (debug.update_answer)", answer);
		n3ds_log(message);
		if (!strcmp(answer, "yes-bad-hash"))
			update_bad_hash = 1;
		if (!strncmp(answer, "yes", 3))
			update_download_wanted = 1;
		else
		{
			if (!strcmp(answer, "never"))
				config_write_boolean("update.auto", 0);
			update_state = _update_none;
		}
		return;
	}
	snprintf(hint, sizeof(hint), "Fractum Halo %s is out. You have %s.", update_version, HALO_N3DS_VERSION);
	swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 3, 1);
	swkbdSetHintText(&keyboard, hint);
	swkbdSetButton(&keyboard, SWKBD_BUTTON_LEFT, "Not now", false);
	swkbdSetButton(&keyboard, SWKBD_BUTTON_MIDDLE, "Never", false);
	swkbdSetButton(&keyboard, SWKBD_BUTTON_RIGHT, "Update", false);
	swkbdSetValidation(&keyboard, SWKBD_ANYTHING, 0, 0);
	text[0] = 0;
	button = swkbdInputText(&keyboard, text, sizeof(text));
	if (button == SWKBD_BUTTON_RIGHT)
	{
		n3ds_log("update: the player chose Update");
		update_download_wanted = 1;
	}
	else if (button == SWKBD_BUTTON_MIDDLE)
	{
		n3ds_log("update: the player chose Never (update.auto off)");
		config_write_boolean("update.auto", 0);
		update_state = _update_none;
	}
	else
	{
		n3ds_log("update: the player chose Not now");
		update_state = _update_none;
	}
}

/* the bottom screen's menu line for the update, if any (engine_network_help.c) */
int n3ds_update_help_line(char *line, int size)
{
	switch (update_state)
	{
	case _update_downloading:
		snprintf(line, (size_t)size, "Updating Fractum Halo: %d%%", update_percent);
		return 1;
	case _update_done:
	case _update_failed:
		snprintf(line, (size_t)size, "%s", update_message);
		return 1;
	default:
		return 0;
	}
}
