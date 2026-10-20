/* Shared on-disk structures: cache_files.c's (the FractumSeraph/OpenCE
fork's, with its Custom Edition maps'), for the engine and the 3DS port's
cache reader (port/n3ds/include/cache_reader.h). */
#ifndef HALO_CACHE_FILE_FORMAT_H
#define HALO_CACHE_FILE_FORMAT_H

struct cache_file_tag_instance
{
	long group_tag;
	long parent_group_tags[2];
	long tag_index;
	/* read from the cache file: Xbox addresses */
	XPTR(char) name;
	XPTR(void) base_address;
	unsigned long unused[2];
};

struct cache_file_tag_header
{
	/* read from the cache file: Xbox addresses */
	XPTR(struct cache_file_tag_instance) tag_instances;
	long scenario_tag_index;
	unsigned long checksum;
	long tag_count;
	long vertex_buffer_count;
	XPTR(void) vertex_buffers;
	long index_buffer_count;
	XPTR(void) index_buffers;
	unsigned long signature;
};

struct cache_file_structure_bsp_header
{
	/* read from the cache file: Xbox addresses */
	XPTR(void) base_address;
	long vertex_buffer_count;
	XPTR(void) vertex_buffers;
	long index_buffer_count;
	XPTR(void) index_buffers;
	unsigned long signature;
};

#ifdef HALO_CUSTOM_EDITION
/* port: Custom Edition maps (Halo PC's): their version, their tag cache
(platform.h), and their tag header, which has the models' vertices and
indices in one block of the file rather than Direct3D buffers */
#define CACHE_FILE_CE_VERSION 609
/* (and Halo PC retail's, version 7: HaloMD's maps, played as <name>@md and
read as Custom Edition's are, their stock bitmaps' and sounds' data from
Custom Edition's resource maps, port/linux/game/ce_resources.c) */
#define CACHE_FILE_RETAIL_VERSION 7
#define CACHE_FILE_VERSION_IS_PC(version) \
	((version) == CACHE_FILE_CE_VERSION || (version) == CACHE_FILE_RETAIL_VERSION)
#define CE_TAG_CACHE_BASE 0x40440000U
#define CE_TAG_CACHE_SIZE 0x01700000U

struct cache_file_ce_tag_header
{
	XPTR(struct cache_file_tag_instance) tag_instances;
	long scenario_tag_index;
	unsigned long checksum;
	long tag_count;
	long model_part_count;
	long model_data_file_offset;
	long model_part_count_again;
	long vertex_data_size;
	long model_data_size;
	unsigned long signature;
};

/* (whether the map being loaded, or loaded, is one: tag_groups.c) */
extern boolean cache_file_is_ce;

#endif

struct cache_file_header
{
	unsigned long header_signature;
	long version;
	long file_length;
	byte reservedC[4];
	long tag_data_offset;
	long tag_data_size;
	byte reserved18[8];
	char name[0x20];
	char build[0x20];
	byte reserved60[4];
	unsigned long checksum;
	byte reserved68[0x794];
	unsigned long footer_signature;
};


#endif
