/* ce_relocate_test <map>: port/n3ds/src/engine_ce_relocate.c on a real
 * Custom Edition map, on a PC: its tags and every structure BSP read into
 * a space as the 3DS reads them, relocated to a stand-in 32-bit address,
 * then checked: every tag's name and data, every BSP's body, inside the
 * space at the rewritten address. Prints what it found; exits 1 if a check
 * fails.
 *   cc -I port/n3ds/include -I port/n3ds/src port/n3ds/tests/ce_relocate_test.c \
 *      port/n3ds/src/engine_ce_relocate.c -o ce_relocate_test */
#include "engine_ce_relocate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CE_BASE 0x40440000UL
#define CE_SIZE 0x01700000UL
#define NATIVE 0x10000000UL

static unsigned int u32(const unsigned char *p)
{
	return (unsigned int)p[0] | (unsigned int)p[1] << 8 | (unsigned int)p[2] << 16 | (unsigned int)p[3] << 24;
}

static long native_offset(unsigned int value)
{
	return value >= NATIVE && value - NATIVE < CE_SIZE ? (long)(value - NATIVE) : -1;
}

int main(int argc, char **argv)
{
	FILE *file;
	unsigned char header[2048];
	unsigned char *space_bytes;
	struct n3ds_ce_space space;
	unsigned long tag_offset, tag_bytes;
	long scenario_index = -1, table, scenario, references;
	unsigned int count, i, bsp_count, failures = 0, left_in_range = 0;

	if (argc < 2 || !(file = fopen(argv[1], "rb")) || fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		fprintf(stderr, "usage: ce_relocate_test <Custom Edition map>\n");
		return 2;
	}
	if (u32(header) != 0x68656164 || u32(header + 4) != 609)
	{
		fprintf(stderr, "not a Custom Edition map (version %u)\n", u32(header + 4));
		return 2;
	}
	tag_offset = u32(header + 16);
	tag_bytes = u32(header + 20);
	space_bytes = calloc(CE_SIZE, 1);
	if (!space_bytes || tag_bytes > CE_SIZE || fseek(file, (long)tag_offset, SEEK_SET) ||
		fread(space_bytes, 1, tag_bytes, file) != tag_bytes)
		return 2;
	if (!n3ds_ce_space_create(&space, space_bytes, CE_BASE, CE_SIZE))
		return 2;
	space.native_address = NATIVE;
	if (!n3ds_ce_relocate_tags(&space, tag_bytes, &scenario_index))
	{
		printf("FAIL: tags not relocated\n");
		return 1;
	}
	printf("tags: %lu bytes, %lu pointers rewritten, %lu blocks outside, %lu unknown groups, scenario %ld\n",
		tag_bytes, space.rewritten, space.bad_blocks, space.unknown_groups, scenario_index & 0xffff);

	/* every tag's name and (not indexed, not a BSP) data at a rewritten address */
	count = u32(space_bytes + 12);
	table = native_offset(u32(space_bytes));
	if (table < 0)
		return 1;
	for (i = 0; i < count; i++)
	{
		const unsigned char *entry = space_bytes + table + i * 32;
		long name = native_offset(u32(entry + 16));

		if (name < 0 || !memchr(space_bytes + name, 0, 256))
		{
			if (failures++ < 5)
				printf("FAIL: tag %u's name at %08x\n", i, u32(entry + 16));
		}
		if (!u32(entry + 24) && u32(entry) != 0x73627370 && native_offset(u32(entry + 20)) < 0)
		{
			if (failures++ < 5)
				printf("FAIL: tag %u's data at %08x\n", i, u32(entry + 20));
		}
	}

	/* the BSPs: the scenario's references (0x5A4: count, 0x5A8: address) */
	scenario = native_offset(u32(space_bytes + table + (scenario_index & 0xffff) * 32 + 20));
	if (scenario < 0)
		return 1;
	bsp_count = u32(space_bytes + scenario + 0x5A4);
	references = native_offset(u32(space_bytes + scenario + 0x5A8));
	printf("bsps: %u\n", bsp_count);
	for (i = 0; bsp_count && references >= 0 && i < bsp_count; i++)
	{
		const unsigned char *reference = space_bytes + references + i * 32;
		unsigned long start = u32(reference), size = u32(reference + 4), address = u32(reference + 8);
		unsigned long before = space.rewritten;

		if (address < CE_BASE || address - CE_BASE + size > CE_SIZE || fseek(file, (long)start, SEEK_SET) ||
			fread(space_bytes + (address - CE_BASE), 1, size, file) != size)
		{
			printf("FAIL: bsp %u not read (address %08lx, %lu bytes)\n", i, address, size);
			failures++;
			continue;
		}
		if (!n3ds_ce_relocate_bsp(&space, address, size))
		{
			printf("FAIL: bsp %u not relocated\n", i);
			failures++;
			continue;
		}
		printf("bsp %u: %lu bytes at %08lx, %lu pointers rewritten, body at %08x\n", i, size, address,
			space.rewritten - before, u32(space_bytes + (address - CE_BASE)));
	}

	/* words still pointing into the CE space (not pointers by the layouts:
	   floats from 3.07 to 22.5 look like these; or a missed pointer) */
	for (i = 0; i + 4 <= tag_bytes; i += 4)
	{
		unsigned int v = u32(space_bytes + i);

		if (v >= CE_BASE && v < CE_BASE + tag_bytes)
			left_in_range++;
	}
	printf("tag words still in the CE range (likely floats): %u\n", left_in_range);
	/* (with -v: each, with the tag whose data starts nearest below it) */
	if (argc > 2)
	{
		unsigned int shown = 0;

		for (i = 0; i + 4 <= tag_bytes && shown < 2000; i += 4)
		{
			unsigned int v = u32(space_bytes + i), t, best = 0xffffffff;
			long best_start = -1;
			float f;

			if (v < CE_BASE || v >= CE_BASE + tag_bytes)
				continue;
			for (t = 0; t < count; t++)
			{
				long start = native_offset(u32(space_bytes + table + t * 32 + 20));

				if (start >= 0 && start <= (long)i && (long)i - start < (long)best)
				{
					best = (unsigned int)(i - start);
					best_start = t;
				}
			}
			memcpy(&f, &v, 4);
			printf("left %08x = %08x (%g) tag %ld '%.4s' +%u\n", i, v, f, best_start,
				best_start >= 0 ? (const char *)space_bytes + table + best_start * 32 : "????", best);
			shown++;
		}
	}
	n3ds_ce_space_dispose(&space);
	printf(failures ? "RESULT: FAIL (%u)\n" : "RESULT: PASS\n", failures);
	return failures ? 1 : 0;
}
