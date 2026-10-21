/* Halo PC maps on the 3DS (engine_cache.c includes this): Custom Edition's
 * (and HaloMD's) maps, played as the FractumSeraph/OpenCE fork plays them.
 *
 * Such a map is linked to its own tag cache, 0x40440000 to CE_TAG_CACHE_SIZE
 * past it, with each structure BSP at its own address there. That space is
 * one block of the console's memory here (the "space"), with room past it
 * for the fork's converted models and level (CE_CONVERSION_ROOM). Loading:
 *   1. the map's tags are read to the space's start, as the fork reads them
 *      to its tag cache;
 *   2. the fork's Custom Edition code converts them, as its own load does
 *      (cache_files.c's cache_files_bind_native_ce: the resource maps'
 *      bitmaps, sounds and strings copied in, the repairs, gbxmodels made the
 *      Xbox's models, the functions, shaders and HUD), reading and writing
 *      the map's addresses, translated to the space meanwhile
 *      (cseries/xbox_address.h);
 *   3. every pointer into the space is made native, once
 *      (engine_ce_relocate.c), and the tags are this engine's, as an Xbox
 *      map's are once Halo3DS's .nrl tables have relocated them.
 * A BSP is read to its address in the space, converted (its vertices
 * compressed: ce_bsp.c) and made native the same way.
 *
 * The maps are where the fork's are, in the data folder's custom_maps (or
 * maps_ce), with the resource maps (bitmaps.map, sounds.map, loc.map):
 * sdmc:/halo-source/custom_maps/. The space is a New 3DS's: about 31 MB. */
#ifdef HALO_CUSTOM_EDITION
#include "engine_ce_relocate.h"
#include "halo_map_families.h"

void *memalign(size_t alignment, size_t size); /* (newlib's malloc.h, which the engine's headers do not reach) */

enum
{
	/* the room past the tag cache for the fork's converted models and level
	   (ce_resources_allocate) */
	N3DS_CE_CONVERSION_ROOM = 8 * 1024 * 1024,
	N3DS_CE_SPACE_SIZE = CE_TAG_CACHE_SIZE + N3DS_CE_CONVERSION_ROOM,
};

/* (cseries/xbox_address.h's) */
unsigned char *n3ds_ce_space_native;
unsigned long n3ds_ce_space_size;
int n3ds_ce_space_converting;

extern int platform_ce_tag_cache_ready;
extern unsigned int platform_ce_conversion_room;
extern char platform_ce_tag_cache_error[160];
long cache_files_bind_native_ce(const struct cache_file_header *header, void *tags, const byte *model_data);
void cache_files_ce_bsp_loaded(struct cache_file_structure_bsp_header *bsp);
void n3ds_xbox_path(const char *in, char *out, size_t size);

static struct n3ds_ce_space native_ce_relocation;
static int native_is_ce;

/* the space, made once and kept while a Halo PC map is loaded */
static int ce_space_prepare(void)
{
	if (n3ds_ce_space_native)
		return 1;
	{
		extern unsigned long n3ds_cache_heap_headroom(void);
		extern unsigned int __ctru_heap_size, __ctru_linear_heap_size;
		char text[160];

		snprintf(text, sizeof(text), "CE MAP: heap %u bytes (%lu free), linear heap %u; the space needs %u",
			__ctru_heap_size, n3ds_cache_heap_headroom(), __ctru_linear_heap_size, (unsigned)N3DS_CE_SPACE_SIZE);
		n3ds_log(text);
	}
	n3ds_ce_space_native = memalign(4096, N3DS_CE_SPACE_SIZE);
	if (!n3ds_ce_space_native)
	{
		snprintf(platform_ce_tag_cache_error, sizeof(platform_ce_tag_cache_error),
			"no memory for a Halo PC map's tags (%u bytes)", (unsigned)N3DS_CE_SPACE_SIZE);
		n3ds_log("CE MAP: no memory for the tag cache's space");
		return 0;
	}
	n3ds_ce_space_size = N3DS_CE_SPACE_SIZE;
	platform_ce_conversion_room = N3DS_CE_CONVERSION_ROOM;
	if (!n3ds_ce_space_create(&native_ce_relocation, n3ds_ce_space_native, CE_TAG_CACHE_BASE, N3DS_CE_SPACE_SIZE))
	{
		free(n3ds_ce_space_native);
		n3ds_ce_space_native = NULL;
		n3ds_ce_space_size = 0;
		return 0;
	}
	return 1;
}

static void ce_space_release(void)
{
	n3ds_ce_space_dispose(&native_ce_relocation);
	free(n3ds_ce_space_native);
	n3ds_ce_space_native = NULL;
	n3ds_ce_space_size = 0;
}

/* a map name's Halo PC map file (its family's folders: map_families.c), on
   the SD card; 0 if the name is the Xbox's or none is there */
static int ce_map_path(const char *name, char *path, size_t size, char *file, size_t file_size)
{
	char xbox_path[256];
	short family;

	if (!name)
		return 0;
	family = map_family_parse(name, file, (long)file_size);
	if (family == _map_family_xbox || !map_family_find(family, file, xbox_path, sizeof(xbox_path)))
		return 0;
	n3ds_xbox_path(xbox_path, path, size);
	return 1;
}

static int ce_header_read(const char *path, struct cache_file_header *header)
{
	FILE *file = fopen(path, "rb");
	long length;
	int read;

	if (!file)
		return 0;
	read = fread(header, 1, sizeof(*header), file) == sizeof(*header) && !fseek(file, 0, SEEK_END);
	length = ftell(file);
	fclose(file);
	/* (a map built by Invader may give its length as 0: the file's own then,
	as the fork's ce_map_open takes it: beavercreek_rev_beta's) */
	if (read && !header->file_length && length > 0)
		header->file_length = length;
	return read && header->header_signature == 'head' && header->footer_signature == 'foot' &&
		CACHE_FILE_VERSION_IS_PC(header->version) && memchr(header->name, 0, 32) &&
		header->tag_data_offset >= 2048 && header->tag_data_size >= 40 &&
		(unsigned long)header->tag_data_size <= CE_TAG_CACHE_SIZE &&
		header->file_length <= length && header->tag_data_offset <= header->file_length - header->tag_data_size;
}

boolean n3ds_engine_ce_map_installed(const char *name)
{
	struct cache_file_header header;
	char path[256], file[64];

	/* (none where there is no room for one's tag cache: engine_main.c) */
	if (!platform_ce_tag_cache_ready)
		return FALSE;
	return ce_map_path(name, path, sizeof(path), file, sizeof(file)) && ce_header_read(path, &header);
}

/* a bitmap's pixels from the resource map its tag's are in (bitmaps.map:
   ce_resources_file_for_tag): nonzero if read, 0 if not, -1 if they are the
   map's own (n3ds_engine_cache_read) */
static int ce_pixels_read(long tag_index, unsigned long offset, unsigned long bytes, void *destination)
{
	extern void *ce_resources_file_for_tag(long tag_index);
	extern int n3ds_win32_read_at(void *handle, unsigned long offset, unsigned long bytes, void *destination);
	void *file = ce_resources_file_for_tag(tag_index);
	int read;

	if (!file)
		return -1;
	if (!destination || !bytes)
		return 0;
	n3ds_files_lock();
	read = n3ds_win32_read_at(file, offset, bytes, destination);
	n3ds_files_unlock();
	return read;
}

/* the map's bitmaps made ready for Halo3DS's texture loader
   (engine_textures.c), as its setup leaves an Xbox map's: cached (their
   pixels' offsets are already the files'), their tag's, no texture yet; and
   their pixels' layout Halo PC's (N3DS_BITMAP_PC_LAYOUT_FLAG), a raw one's
   swizzled flag set to pass the Xbox's checks */
static void ce_bitmaps_ready(long tag_count)
{
	long tag, bitmaps = 0;

	for (tag = 0; tag < tag_count; tag++)
	{
		struct cache_file_tag_instance *instance = &global_tag_instances[tag];
		byte *group = instance->base_address;
		struct bitmap_data *data;
		long count, index;

		if (instance->group_tag != 'bitm' || !n3ds_cache_resolve(&native_tags, group, 0x6c))
			continue;
		memcpy(&count, group + 0x60, 4);
		memcpy(&data, group + 0x64, 4);
		if (count <= 0 || count > 0x10000 || !n3ds_cache_resolve(&native_tags, data, (unsigned long)count * sizeof(*data)))
			continue;
		for (index = 0; index < count; index++)
		{
			struct bitmap_data *bitmap = &data[index];
			int raw = bitmap->format < 14 || bitmap->format > 16;

			bitmap->flags |= 0x80 | N3DS_BITMAP_PC_LAYOUT_FLAG | (raw ? 8 : 0);
			bitmap->tag_index = instance->tag_index;
			bitmap->cache_block_index = NONE;
			bitmap->hardware_format = NULL;
			bitmap->base_address = NULL;
			bitmaps++;
		}
	}
	{
		char message[96];

		snprintf(message, sizeof(message), "CE MAP: %ld bitmaps made ready", bitmaps);
		n3ds_log(message);
	}
}

/* ... and its sounds' permutations given their tag (their samples' file
   is found by it: ce_resources_file_for_tag), as the Xbox's are */
static void ce_sounds_ready(long tag_count)
{
	long tag, permutations = 0;

	for (tag = 0; tag < tag_count; tag++)
	{
		struct cache_file_tag_instance *instance = &global_tag_instances[tag];
		struct sound_definition *sound = instance->base_address;
		long range, index;

		if (instance->group_tag != 'snd!' || !n3ds_cache_resolve(&native_tags, sound, sizeof(*sound)))
			continue;
		for (range = 0; range < sound->pitch_ranges.count; range++)
		{
			struct sound_pitch_range *pitch_range = TAG_BLOCK_GET_ELEMENT(&sound->pitch_ranges, range,
				struct sound_pitch_range);

			for (index = 0; index < pitch_range->permutations.count; index++)
			{
				TAG_BLOCK_GET_ELEMENT(&pitch_range->permutations, index, struct sound_permutation)->unknown3 =
					(unsigned long)instance->tag_index;
				permutations++;
			}
		}
	}
	{
		char message[96];

		snprintf(message, sizeof(message), "CE MAP: %ld sound permutations given their tags", permutations);
		n3ds_log(message);
	}
}

static void ce_tags_unload(void);

/* port: a Halo PC map's load that failed after its space was had: the space
   (31 MB) given back, and what the conversion had kept of the map, or the
   Xbox map loaded next would go without the memory (native_is_ce is not set
   yet, so the unload at engine_cache.c's would not do it) */
static long ce_tags_load_failed(const char *why)
{
	if (why)
		n3ds_log(why);
	cache_files_bind_native(NULL, NULL);
	ce_tags_unload();
	return NONE;
}

static long ce_tags_load(const char *name)
{
	struct cache_file_header header;
	struct cache_file_ce_tag_header *ce_header;
	char path[256], file[64], message[256];
	byte *model_data = NULL;
	FILE *stream;
	long scenario_index, relocated_scenario;
	int read;

	if (!ce_map_path(name, path, sizeof(path), file, sizeof(file)) || !ce_header_read(path, &header))
		return NONE;
	/* (the fork's checks of every offset, count and size in it the
	conversion reads, before any is converted: a map shared or downloaded
	is anyone's (ce_map_checks.c, through cache_files_windows.c's
	ce_map_open, which keeps the map's file open for its own sounds too).
	Before the space is had: the check reads the tag cache into an image of
	its own, let go of after) */
	{
		extern boolean cache_files_ce_map_refused(char const *map_name, char *text, long size);
		char why[384];

		if (cache_files_ce_map_refused(name, why, sizeof(why)))
		{
			snprintf(message, sizeof(message), "CE MAP: %s refused: %.200s", name, why);
			n3ds_log(message);
			return NONE;
		}
	}
	if (!ce_space_prepare())
		return NONE;
	snprintf(message, sizeof(message), "CE MAP: %s (%s), version %ld, %ld bytes of tags", name, path,
		(long)header.version, (long)header.tag_data_size);
	n3ds_log(message);
	memset(n3ds_ce_space_native, 0xCD, n3ds_ce_space_size);
	memset(native_ce_relocation.done, 0, (n3ds_ce_space_size >> 5) + 1);
	native_ce_relocation.rewritten = native_ce_relocation.bad_blocks = 0;
	native_ce_relocation.unknown_groups = native_ce_relocation.indexed_left = 0;
	stream = fopen(path, "rb");
	if (!stream)
		return ce_tags_load_failed("CE MAP: its file could not be opened");
	read = !fseek(stream, header.tag_data_offset, SEEK_SET) &&
		n3ds_cache_read_region(stream, n3ds_ce_space_native, (unsigned long)header.tag_data_size, "Reading tags");
	ce_header = (struct cache_file_ce_tag_header *)n3ds_ce_space_native;
	if (read && ce_header->signature == 'tags' && ce_header->model_data_size > 0 &&
		ce_header->model_data_file_offset >= 2048 && ce_header->model_data_size <= header.file_length &&
		ce_header->model_data_file_offset <= header.file_length - ce_header->model_data_size)
	{
		model_data = malloc((size_t)ce_header->model_data_size);
		if (model_data && (fseek(stream, ce_header->model_data_file_offset, SEEK_SET) ||
			!n3ds_cache_read_region(stream, model_data, (unsigned long)ce_header->model_data_size, "Reading models")))
		{
			free(model_data);
			model_data = NULL;
		}
		/* port: a map whose models are not had is not played (its models'
		   vertices would be the 0xCD the space was filled with) */
		if (!model_data)
		{
			fclose(stream);
			snprintf(message, sizeof(message), "CE MAP: its models (%ld bytes) could not be read or had no memory",
				(long)ce_header->model_data_size);
			snprintf(platform_ce_tag_cache_error, sizeof(platform_ce_tag_cache_error),
				"no memory for the map's models (%ld bytes)", (long)ce_header->model_data_size);
			return ce_tags_load_failed(message);
		}
	}
	fclose(stream);
	if (!read || ce_header->signature != 'tags')
		return ce_tags_load_failed("CE MAP: its tags could not be read");
	/* (the fork's conversion, in the map's addresses) */
	n3ds_ce_space_converting = 1;
	scenario_index = cache_files_bind_native_ce(&header, n3ds_ce_space_native, model_data);
	n3ds_ce_space_converting = 0;
	free(model_data);
	if (scenario_index == NONE)
		return ce_tags_load_failed(NULL);
	/* (then native) */
	if (!n3ds_ce_relocate_tags(&native_ce_relocation, (unsigned long)header.tag_data_size, &relocated_scenario))
		return ce_tags_load_failed("CE MAP: its tags could not be made native");
	snprintf(message, sizeof(message),
		"CE MAP: %lu pointers made native; %lu blocks outside the space, %lu tags of unknown groups, %lu indexed left",
		native_ce_relocation.rewritten, native_ce_relocation.bad_blocks, native_ce_relocation.unknown_groups,
		native_ce_relocation.indexed_left);
	n3ds_log(message);
	memset(&native_tags, 0, sizeof(native_tags));
	native_tags.header = header;
	native_tags.tags = n3ds_ce_space_native;
	native_tags.tag_bytes = n3ds_ce_space_size;
	native_tags.xbox_base = (unsigned long)n3ds_ce_space_native;
	native_tags.tag_header = (struct cache_file_tag_header *)n3ds_ce_space_native;
	native_tags.instances = global_tag_instances;
	native_is_ce = 1;
	ce_bitmaps_ready(ce_header->tag_count);
	ce_sounds_ready(ce_header->tag_count);
	cache_generation_advance();
	++native_tag_generation;
	snprintf(native_map, sizeof(native_map), "%s", path);
	snprintf(native_name, sizeof(native_name), "%.*s", (int)sizeof(native_name) - 1, file);
	native_stream_opens = native_stream_reads = native_stream_bytes = 0;
	return scenario_index;
}

static boolean ce_bsp_load(struct scenario_structure_bsp_reference *reference, long index)
{
	unsigned long address = (unsigned long)reference->base_address;
	unsigned long bytes = (unsigned long)reference->file_size;
	unsigned long offset = address - CE_TAG_CACHE_BASE;
	struct cache_file_structure_bsp_header *header;
	unsigned long word;
	FILE *stream;
	int read;
	char message[160];

	if (address < CE_TAG_CACHE_BASE || bytes < 24 || offset > CE_TAG_CACHE_SIZE || bytes > CE_TAG_CACHE_SIZE - offset ||
		reference->file_offset < 2048 || (unsigned long)reference->file_offset > (unsigned long)native_tags.header.file_length - bytes)
		return FALSE;
	stream = fopen(native_map, "rb");
	if (!stream)
		return FALSE;
	read = !fseek(stream, reference->file_offset, SEEK_SET) &&
		n3ds_cache_read_region(stream, n3ds_ce_space_native + offset, bytes, "Reading scenery");
	fclose(stream);
	header = (struct cache_file_structure_bsp_header *)(n3ds_ce_space_native + offset);
	if (!read || header->signature != 'sbsp')
		return FALSE;
	/* (its words rewritten afresh: a BSP read again is the map's again; and
	the tag cache's free room's, where the conversion puts its compressed
	vertices and their buffers (ce_bsp.c): an earlier BSP's room, given back
	when it unloaded, is this one's now, and its words marked rewritten
	would be left as the map's addresses, which the renderer cannot read) */
	for (word = offset; word < offset + bytes; word += 4)
		native_ce_relocation.done[word >> 5] &= (unsigned char)~(1u << ((word >> 2) & 7));
	{
		extern unsigned long ce_resources_free_mark(void);
		unsigned long mark = ce_resources_free_mark();

		if (mark >= CE_TAG_CACHE_BASE && mark - CE_TAG_CACHE_BASE < N3DS_CE_SPACE_SIZE)
		{
			for (word = (mark - CE_TAG_CACHE_BASE) & ~3UL; word < N3DS_CE_SPACE_SIZE; word += 4)
			{
				if (word >= offset && word < offset + bytes)
					continue;
				native_ce_relocation.done[word >> 5] &= (unsigned char)~(1u << ((word >> 2) & 7));
			}
		}
	}
	n3ds_ce_space_converting = 1;
	cache_files_ce_bsp_loaded(header);
	n3ds_ce_space_converting = 0;
	{
		unsigned long before = native_ce_relocation.rewritten;

		if (!n3ds_ce_relocate_bsp(&native_ce_relocation, address, bytes))
		{
			n3ds_log("CE MAP: a BSP could not be made native");
			return FALSE;
		}
		snprintf(message, sizeof(message), "CE BSP %ld: %lu bytes, %lu pointers made native", index, bytes,
			native_ce_relocation.rewritten - before);
		n3ds_log(message);
	}
	memset(&native_bsp, 0, sizeof(native_bsp));
	native_bsp.header = native_tags.header;
	/* (the whole space: the fork's conversion puts the BSP's compressed
	   vertices, and their buffers, in the tag cache's room, outside the BSP's
	   own bytes: ce_bsp.c) */
	native_bsp.tags = n3ds_ce_space_native;
	native_bsp.tag_bytes = N3DS_CE_SPACE_SIZE;
	native_bsp.xbox_base = (unsigned long)n3ds_ce_space_native;
	native_reference = reference;
	cache_generation_advance();
	cache_files_bind_native_bsp(header, reference->structure_bsp.index);
	return TRUE;
}

static void ce_bsp_unload(void)
{
	extern void ce_bsp_unloaded(void);

	ce_bsp_unloaded();
	memset(&native_bsp, 0, sizeof(native_bsp));
}

static void ce_tags_unload(void)
{
	extern void ce_resources_tags_unloaded(void);
	extern void ce_models_tags_unloaded(void);
	extern void ce_hud_tags_unloaded(void);

	ce_resources_tags_unloaded();
	ce_models_tags_unloaded();
	ce_hud_tags_unloaded();
	memset(&native_tags, 0, sizeof(native_tags));
	native_is_ce = 0;
	ce_space_release();
}
#endif
