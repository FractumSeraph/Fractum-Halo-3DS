/* Custom Edition maps on the 3DS: their pointers made native
 * (port/n3ds/src/engine_ce_relocate.c says how). */
#ifndef HALO_N3DS_ENGINE_CE_RELOCATE_H
#define HALO_N3DS_ENGINE_CE_RELOCATE_H

/* the CE tag cache's space: size bytes of the console's memory standing for
   the map's addresses ce_base to ce_base + size */
struct n3ds_ce_space
{
	unsigned char *native;
	/* the address rewritten pointers get for the space's start: native's on
	   the console (a test on a 64-bit PC sets a 32-bit one) */
	unsigned long native_address;
	unsigned long ce_base;
	unsigned long size;
	/* (a bit for each word: rewritten already) */
	unsigned char *done;
	/* what was found: words rewritten, blocks pointing outside the space,
	   tags of groups the layouts do not know */
	unsigned long rewritten;
	unsigned long bad_blocks;
	unsigned long unknown_groups;
	/* (indexed tags whose data is still the resource maps') */
	unsigned long indexed_left;
};

int n3ds_ce_space_create(struct n3ds_ce_space *space, unsigned char *native, unsigned long ce_base,
	unsigned long size);
void n3ds_ce_space_dispose(struct n3ds_ce_space *space);
/* the map's tags (tag_bytes of them, at the space's start), once the fork's
   code has converted them: their tag directory, names and every tag's data
   in the space (indexed tags' copied in too), as their groups now are (a
   gbxmodel converted is a model); not the BSPs'. The scenario's tag index.
   0 if the map is not a valid Custom Edition one */
int n3ds_ce_relocate_tags(struct n3ds_ce_space *space, unsigned long tag_bytes, long *scenario_index);
/* a structure BSP read into the space at its address */
int n3ds_ce_relocate_bsp(struct n3ds_ce_space *space, unsigned long bsp_ce_address, unsigned long bsp_bytes);

#endif
