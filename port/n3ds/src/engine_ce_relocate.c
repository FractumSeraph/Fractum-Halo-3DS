/* Custom Edition maps on the 3DS: their pointers made native.
 *
 * A Halo PC map (Custom Edition's, version 609) is linked to a tag cache at
 * 0x40440000, CE_TAG_CACHE_SIZE (23 MB) long: its tags at the start, each
 * structure BSP at its own address in the same space. A 3DS program cannot
 * map memory there (its user space ends at 0x40000000), and the engine
 * reads tag pointers directly, so the whole space is one block of the
 * console's memory (the "space") and every pointer into it is rewritten,
 * once, to the same place in the block: what Halo3DS's setup does for the
 * Xbox's maps on a PC (port/n3ds/setup/Core/Relocation.cs, its .nrl tables),
 * here on the console, with the same tag layouts
 * (port/n3ds/src/engine_ce_schemas.inl, from its schemas.json).
 *
 * Only a word the layouts say is a pointer is rewritten, and only if it
 * points into the space: a model part's or bitmap's offsets into data
 * outside the tag cache (the model data block, the resource maps) are left
 * as they are, for the FractumSeraph/OpenCE fork's Custom Edition code that
 * runs next (port/linux/game/ce_*.c), which reads them as offsets. A word
 * is rewritten at most once (a bit for each word of the space), so a
 * structure reached twice is safe. Indexed tags (whose data is in the
 * resource maps) and streamed BSPs are left for that code and for the BSP
 * loader.
 *
 * Plain C over the space's bytes, so that a PC can test it on real maps
 * (port/n3ds/tests/ce_relocate_test.c). */
#include "engine_ce_relocate.h"

#include <stdlib.h>
#include <string.h>

enum
{
	CE_FIELD_STRUCT,
	CE_FIELD_BLOCK,
	CE_FIELD_REFERENCE,
	CE_FIELD_DATA,
	CE_FIELD_POINTER,
};

struct ce_schema_field { unsigned short offset; unsigned char kind; short type; unsigned char special; };
struct ce_schema_type { unsigned int size; unsigned short first, count; };
struct ce_schema_root { unsigned int group; short type; };

#include "engine_ce_schemas.inl"

enum
{
	CE_MAXIMUM_DEPTH = 64,
	CE_MAXIMUM_BLOCK = 1000000,
	CE_TAG_INSTANCE_SIZE = 32,
	CE_GROUP_SBSP = 0x73627370,
};

static unsigned int ce_u32(const unsigned char *p)
{
	return (unsigned int)p[0] | (unsigned int)p[1] << 8 | (unsigned int)p[2] << 16 | (unsigned int)p[3] << 24;
}

static void ce_put_u32(unsigned char *p, unsigned int v)
{
	p[0] = (unsigned char)v;
	p[1] = (unsigned char)(v >> 8);
	p[2] = (unsigned char)(v >> 16);
	p[3] = (unsigned char)(v >> 24);
}

/* a value's offset in the space, if it points into it (as linked, or as
   already rewritten); -1 if not */
static long ce_offset(const struct n3ds_ce_space *space, unsigned int value, unsigned long bytes)
{
	unsigned long native = space->native_address;
	unsigned long offset;

	if (value >= space->ce_base && value - space->ce_base < space->size)
		offset = value - space->ce_base;
	else if (value >= native && value - native < space->size)
		offset = value - native;
	else
		return -1;
	if (bytes > space->size - offset)
		return -1;
	return (long)offset;
}

/* the pointer at this offset of the space rewritten (once), if it points
   into the space; the offset it points to, or -1 */
static long ce_pointer(struct n3ds_ce_space *space, unsigned long at, unsigned long bytes)
{
	unsigned int value;
	long target;

	if (at > space->size - 4 || (at & 3))
		return -1;
	value = ce_u32(space->native + at);
	target = ce_offset(space, value, bytes);
	if (target < 0)
		return -1;
	if (!(space->done[at >> 5] & (1u << ((at >> 2) & 7))))
	{
		ce_put_u32(space->native + at, (unsigned int)(space->native_address + (unsigned long)target));
		space->done[at >> 5] |= (unsigned char)(1u << ((at >> 2) & 7));
		space->rewritten++;
	}
	return target;
}

static int ce_walk(struct n3ds_ce_space *space, short type, unsigned long at, int depth)
{
	const struct ce_schema_type *layout;
	unsigned short i;

	if (type < 0 || type >= (short)(sizeof(ce_schema_types) / sizeof(ce_schema_types[0])) || depth > CE_MAXIMUM_DEPTH)
		return 0;
	layout = &ce_schema_types[type];
	if (at > space->size || layout->size > space->size - at)
		return 0;
	for (i = 0; i < layout->count; i++)
	{
		const struct ce_schema_field *field = &ce_schema_fields[layout->first + i];
		unsigned long pos = at + field->offset;

		switch (field->kind)
		{
		case CE_FIELD_STRUCT:
			if (!ce_walk(space, field->type, pos, depth + 1))
				return 0;
			break;
		case CE_FIELD_BLOCK:
		{
			unsigned int count = ce_u32(space->native + pos);
			unsigned int size = ce_schema_types[field->type].size;
			long first;
			unsigned int element;

			if (!count)
				break;
			if (count > CE_MAXIMUM_BLOCK)
				return 0;
			first = ce_pointer(space, pos + 4, (unsigned long)count * size);
			if (first < 0)
			{
				/* (a block outside the space: a map that would read
				   memory it has not; refused) */
				space->bad_blocks++;
				break;
			}
			for (element = 0; element < count; element++)
				if (!ce_walk(space, field->type, (unsigned long)first + (unsigned long)element * size, depth + 1))
					return 0;
			break;
		}
		case CE_FIELD_REFERENCE:
			ce_pointer(space, pos + 4, 1);
			break;
		case CE_FIELD_DATA:
		{
			unsigned int size = ce_u32(space->native + pos);
			long target = size ? ce_pointer(space, pos + 12, size) : -1;

			/* (a scenario's script syntax data: a datum array whose data
			   pointer, 52 bytes in, points just after its header) */
			if (field->special == 1 && target >= 0 && size >= 56)
				ce_pointer(space, (unsigned long)target + 52, size - 56);
			break;
		}
		case CE_FIELD_POINTER:
			ce_pointer(space, pos, 1);
			break;
		}
	}
	/* the Xbox's geometry, as the fork's conversion of a gbxmodel or a BSP
	   leaves it (ce_models.c, ce_bsp.c) and Halo3DS's renderer reads it
	   (its setup's walk: Relocation.cs): a part's or a lightmap material's
	   Direct3D buffers (a descriptor whose second word is the data), and the
	   vertex buffer's base address the data itself */
	if (type == CE_SCHEMA_MODELGEOMETRYPART)
	{
		long descriptor;

		ce_pointer(space, at + 76, 1);
		descriptor = ce_pointer(space, at + 80, 12);
		if (descriptor >= 0)
			ce_pointer(space, (unsigned long)descriptor + 4, 1);
		descriptor = ce_pointer(space, at + 100, 12);
		if (descriptor >= 0)
		{
			ce_pointer(space, (unsigned long)descriptor + 4, 1);
			memcpy(space->native + at + 96, space->native + descriptor + 4, 4);
		}
	}
	else if (type == CE_SCHEMA_SCENARIOSTRUCTUREBSPMATERIAL)
	{
		static const unsigned short buffers[2] = { 176, 196 };
		int b;

		for (b = 0; b < 2; b++)
		{
			unsigned long buffer = at + buffers[b];
			long descriptor;

			if (!ce_u32(space->native + buffer + 4))
				continue;
			ce_pointer(space, buffer + 12, 1);
			descriptor = ce_pointer(space, buffer + 16, 12);
			if (descriptor >= 0)
			{
				ce_pointer(space, (unsigned long)descriptor + 4, 1);
				memcpy(space->native + buffer + 12, space->native + descriptor + 4, 4);
			}
		}
	}
	return 1;
}

static short ce_root(unsigned int group)
{
	unsigned int i;

	for (i = 0; i < sizeof(ce_schema_roots) / sizeof(ce_schema_roots[0]); i++)
		if (ce_schema_roots[i].group == group)
			return ce_schema_roots[i].type;
	return -1;
}

int n3ds_ce_space_create(struct n3ds_ce_space *space, unsigned char *native, unsigned long ce_base,
	unsigned long size)
{
	memset(space, 0, sizeof(*space));
	space->done = calloc((size >> 5) + 1, 1);
	if (!space->done)
		return 0;
	space->native = native;
	space->native_address = (unsigned long)(size_t)native;
	space->ce_base = ce_base;
	space->size = size;
	return 1;
}

void n3ds_ce_space_dispose(struct n3ds_ce_space *space)
{
	free(space->done);
	space->done = NULL;
}

int n3ds_ce_relocate_tags(struct n3ds_ce_space *space, unsigned long tag_bytes, long *scenario_index)
{
	unsigned char *header = space->native;
	long table;
	unsigned int count, i;

	if (tag_bytes < 40 || tag_bytes > space->size || ce_u32(header + 36) != 0x74616773)
		return 0;
	count = ce_u32(header + 12);
	if (!count || count > 65535)
		return 0;
	table = ce_pointer(space, 0, (unsigned long)count * CE_TAG_INSTANCE_SIZE);
	if (table < 0 || (unsigned long)table + (unsigned long)count * CE_TAG_INSTANCE_SIZE > tag_bytes)
		return 0;
	if (scenario_index)
		*scenario_index = (long)ce_u32(header + 4);
	for (i = 0; i < count; i++)
	{
		unsigned long entry = (unsigned long)table + i * CE_TAG_INSTANCE_SIZE;
		unsigned int group = ce_u32(space->native + entry);
		unsigned int indexed = ce_u32(space->native + entry + 24);

		short root;
		long body;

		ce_pointer(space, entry + 16, 1);
		/* (an indexed tag's data is a resource map's, found by its index,
		   until the fork's code copies it into the space (ce_resources.c):
		   walked if it is there now, else left; a BSP's is streamed:
		   n3ds_ce_relocate_bsp) */
		if (group == CE_GROUP_SBSP)
			continue;
		if (indexed && ce_offset(space, ce_u32(space->native + entry + 20), 1) < 0)
		{
			space->indexed_left++;
			continue;
		}
		root = ce_root(group);
		if (root < 0)
		{
			space->unknown_groups++;
			continue;
		}
		body = ce_pointer(space, entry + 20, ce_schema_types[root].size);
		if (body < 0)
		{
			space->bad_blocks++;
			continue;
		}
		if (!ce_walk(space, root, (unsigned long)body, 0))
			return 0;
	}
	return 1;
}

int n3ds_ce_relocate_bsp(struct n3ds_ce_space *space, unsigned long bsp_ce_address, unsigned long bsp_bytes)
{
	long at = ce_offset(space, (unsigned int)bsp_ce_address, bsp_bytes);
	long body;

	if (at < 0 || bsp_bytes < 24 || ce_u32(space->native + at + 20) != CE_GROUP_SBSP)
		return 0;
	/* (its header's pointers: the BSP, its vertices) */
	if (!ce_walk(space, CE_SCHEMA_SCENARIOSTRUCTUREBSPCOMPILEDHEADER, (unsigned long)at, 0))
		return 0;
	body = ce_offset(space, ce_u32(space->native + at), 1);
	return body >= 0 && ce_walk(space, CE_SCHEMA_SCENARIOSTRUCTUREBSP, (unsigned long)body, 0);
}
