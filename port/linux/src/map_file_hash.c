/*
MAP_FILE_HASH.C

Map files' whole-file hash prefixes (map_file_hash.h), each read once for
the run: a map is a few to a few hundred MB, and the game asks for its
file again whenever it looks for the map.
*/

#include "platform.h"
#include "map_archive.h"
#include "map_file_hash.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

/* ---------- constants */

enum
{
	/* the files whose prefixes are kept */
	MAP_FILE_HASHES_KEPT = 32,
	MAP_FILE_HASH_PATH_SIZE = 256,
};

/* ---------- globals */

struct map_file_hash_entry
{
	char path[MAP_FILE_HASH_PATH_SIZE];
	unsigned int size;
	unsigned int time_low;
	unsigned int time_high;
	unsigned int prefix;
#ifdef HALO_N3DS
	/* (the file's first and last 4 KB's FNV-1a: the 3DS's files may say no
	time they were written) */
	unsigned int sample;
#endif
};

static struct
{
	pthread_mutex_t lock;
	struct map_file_hash_entry entries[MAP_FILE_HASHES_KEPT];
	int count;
	/* the entry replaced next, once all are used */
	int next;
} map_file_hashes = { PTHREAD_MUTEX_INITIALIZER };

/* ---------- private code */

#ifdef HALO_N3DS
/* port: (the 3DS) the prefixes kept on the SD card too, so that a map is
read whole once, not once a run: reading and hashing one is seconds to
minutes there (a 14 MB map took 30 s in Azahar), on the game's thread, the
first time it is looked for or picked */
#define MAP_FILE_HASH_CACHE "sdmc:/halo-source/map_hashes.cache"
static const unsigned int map_file_hash_cache_magic = 0x48434D33; /* '3MCH' */
static int map_file_hashes_loaded;

static unsigned int file_sample(HANDLE file, unsigned int size)
{
	unsigned char buffer[4096];
	unsigned int hash = 2166136261U, part, index;

	for (part = 0; part < 2; part++)
	{
		unsigned int offset = part ? (size > sizeof(buffer) ? size - (unsigned int)sizeof(buffer) : 0) : 0;
		unsigned int bytes = size < sizeof(buffer) ? size : (unsigned int)sizeof(buffer);
		DWORD read = 0;

		if (SetFilePointer(file, (LONG)offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER ||
			!ReadFile(file, buffer, bytes, &read, NULL) || read != bytes)
		{
			return 0;
		}
		for (index = 0; index < bytes; index++)
			hash = (hash ^ buffer[index]) * 16777619U;
	}
	return hash ? hash : 1;
}

/* (under the lock) */
static void map_file_hashes_load(void)
{
	FILE *cache;
	unsigned int magic = 0, count = 0;

	if (map_file_hashes_loaded)
		return;
	map_file_hashes_loaded = 1;
	cache = fopen(MAP_FILE_HASH_CACHE, "rb");
	if (!cache)
		return;
	if (fread(&magic, sizeof(magic), 1, cache) == 1 && magic == map_file_hash_cache_magic &&
		fread(&count, sizeof(count), 1, cache) == 1 && count <= MAP_FILE_HASHES_KEPT &&
		fread(map_file_hashes.entries, sizeof(*map_file_hashes.entries), count, cache) == count)
	{
		unsigned int index;

		for (index = 0; index < count; index++)
			map_file_hashes.entries[index].path[MAP_FILE_HASH_PATH_SIZE - 1] = 0;
		map_file_hashes.count = (int)count;
		map_file_hashes.next = (int)(count % MAP_FILE_HASHES_KEPT);
	}
	fclose(cache);
}

/* (under the lock) */
static void map_file_hashes_save(void)
{
	FILE *cache = fopen(MAP_FILE_HASH_CACHE ".new", "wb");
	unsigned int count = (unsigned int)map_file_hashes.count;
	int written;

	if (!cache)
		return;
	written = fwrite(&map_file_hash_cache_magic, sizeof(map_file_hash_cache_magic), 1, cache) == 1 &&
		fwrite(&count, sizeof(count), 1, cache) == 1 &&
		fwrite(map_file_hashes.entries, sizeof(*map_file_hashes.entries), count, cache) == count;
	if (fclose(cache) != 0)
		written = 0;
	remove(MAP_FILE_HASH_CACHE);
	if (!written || rename(MAP_FILE_HASH_CACHE ".new", MAP_FILE_HASH_CACHE) != 0)
		remove(MAP_FILE_HASH_CACHE ".new");
}
#endif

static int file_read(void *context, unsigned int offset, void *buffer, unsigned int size)
{
	HANDLE file = *(HANDLE *)context;
	DWORD read = 0;

	if (SetFilePointer(file, (LONG)offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return 0;
	return ReadFile(file, buffer, size, &read, NULL) && read == size;
}

/* ---------- public code */

int map_file_hash_prefix(const char *path, unsigned int *prefix)
{
	unsigned char hash[MAP_ARCHIVE_HASH_SIZE];
	struct map_file_hash_entry entry;
	FILETIME written;
	DWORD high = 0;
	HANDLE file;
	int index;
	int ok;

	if (!path || strlen(path) >= MAP_FILE_HASH_PATH_SIZE)
		return 0;
	file = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return 0;
	memset(&entry, 0, sizeof(entry));
	snprintf(entry.path, sizeof(entry.path), "%s", path);
	entry.size = (unsigned int)GetFileSize(file, &high);
	memset(&written, 0, sizeof(written));
	GetFileTime(file, NULL, NULL, &written);
	entry.time_low = (unsigned int)written.dwLowDateTime;
	entry.time_high = (unsigned int)written.dwHighDateTime;
	/* (no map is 4 GB, nor more than map_archive's limit) */
	if (high || entry.size > MAP_ARCHIVE_MAXIMUM_SIZE)
	{
		CloseHandle(file);
		return 0;
	}
#ifdef HALO_N3DS
	entry.sample = file_sample(file, entry.size);
#endif
	pthread_mutex_lock(&map_file_hashes.lock);
#ifdef HALO_N3DS
	map_file_hashes_load();
#endif
	for (index = 0; index < map_file_hashes.count; index++)
	{
		struct map_file_hash_entry const *kept = &map_file_hashes.entries[index];

		if (!strcmp(kept->path, entry.path) && kept->size == entry.size && kept->time_low == entry.time_low &&
			kept->time_high == entry.time_high
#ifdef HALO_N3DS
			&& kept->sample == entry.sample && entry.sample
#endif
			)
		{
			*prefix = kept->prefix;
			pthread_mutex_unlock(&map_file_hashes.lock);
			CloseHandle(file);
			return 1;
		}
	}
	pthread_mutex_unlock(&map_file_hashes.lock);
	ok = map_archive_map_hash(file_read, NULL, &file, entry.size, hash);
	CloseHandle(file);
	if (!ok)
		return 0;
	entry.prefix = map_archive_hash_prefix(hash);
	*prefix = entry.prefix;
	pthread_mutex_lock(&map_file_hashes.lock);
	if (map_file_hashes.count < MAP_FILE_HASHES_KEPT)
		index = map_file_hashes.count++;
	else
	{
		index = map_file_hashes.next;
		map_file_hashes.next = (map_file_hashes.next + 1) % MAP_FILE_HASHES_KEPT;
	}
	map_file_hashes.entries[index] = entry;
#ifdef HALO_N3DS
	map_file_hashes_save();
#endif
	pthread_mutex_unlock(&map_file_hashes.lock);
	return 1;
}

int map_file_hash_version(const char *path, unsigned int *version)
{
	unsigned int prefix;

	if (!map_file_hash_prefix(path, &prefix))
		return 0;
	/* (0 is the version a client checks nothing for: map_archive_hash_version) */
	*version = prefix ? prefix : 1;
	return 1;
}
