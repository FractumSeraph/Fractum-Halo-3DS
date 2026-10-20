/*
XBOX_ADDRESS.H

Pointers inside Xbox data, for the 64-bit builds (HALO_64BIT).

The game's data formats embed 32-bit pointers: cache files hold tag data
whose blocks, references and data point at each other by address, game
state is saved and restored as raw memory, and Direct3D resources carry
physical addresses. A 64-bit build keeps all of that memory inside one
4 GB region reserved at start-up, at a fixed host address (anywhere it
fits on Android), so that Xbox address X is host
address XBOX_ADDRESS_SPACE_BASE + X (port/linux/src/xbox_memory.c).
Structures laid out like the Xbox's declare their pointer fields XPTR(type)
(a 32-bit Xbox address) and convert with XBOX_POINTER and XBOX_ADDRESS.

Everywhere else (the Xbox, the matching MSVC build and the 32-bit ports) a
pointer is an Xbox address: XPTR(type) is `type *` and the conversions do
nothing, so the code compiles as before.
*/

#ifndef __XBOX_ADDRESS_H
#define __XBOX_ADDRESS_H

#ifdef HALO_64BIT

#ifdef HALO_3DS

/* The Nintendo 3DS (port/3ds): a 32-bit host that cannot map memory at
the Xbox's addresses (its user space ends at 0x40000000). Xbox address X
is host address X + xbox_address_space_base, in 32-bit arithmetic (the sum
wraps): the offset that puts the Xbox contiguous window (0x80000000) at the
block the platform layer allocated for it (port/3ds/src/n3ds_memory.c).
The Xbox's 4 GB and the host's 4 GB are one ring, so every host pointer is
an Xbox address too, and none is ever out of range. */

extern unsigned int xbox_address_space_base;
#define XBOX_ADDRESS_SPACE_BASE xbox_address_space_base
#define XBOX_ADDRESS_SPACE_SIZE 0x100000000ULL

/* Halo PC's maps (Custom Edition, HaloMD) are linked to a tag cache of
their own at 0x40440000 (23 MB, platform.h's PLATFORM_CE_TAG_CACHE_BASE),
which one offset cannot put in the 3DS's user space beside the window:
those addresses are the window's from XBOX_CE_ALIAS on, the 1 MB the
platform layer keeps below the Xbox's tag cache and the Xbox's tag cache
itself, which a Halo PC map's tags take the place of while they are
loaded (port/3ds/src/n3ds_memory.c). While they are (xbox_ce_alias_active,
cache_files.c), a host pointer there is their address. */
#define XBOX_CE_BASE 0x40440000U
#define XBOX_CE_SIZE 0x01700000U
#define XBOX_CE_ALIAS 0x802A6000U
extern unsigned int xbox_ce_alias_active;

/* a pointer field of an Xbox-layout structure; type documents the target */
#define XPTR(type) unsigned int
/* an XPTR field's null */
#define XBOX_NULL 0

/* Xbox address to host pointer; 0 stays NULL */
static __inline__ __attribute__((always_inline)) void *xbox_pointer(unsigned int address)
{
	if (!address)
		return (void *)0;
	if (address - XBOX_CE_BASE < XBOX_CE_SIZE)
		address += XBOX_CE_ALIAS - XBOX_CE_BASE;
	return (void *)(__UINTPTR_TYPE__)(xbox_address_space_base + address);
}

/* host pointer to Xbox address; NULL stays 0 */
static __inline__ __attribute__((always_inline)) unsigned int xbox_address(void const *pointer)
{
	unsigned int address;

	if (!pointer)
		return 0;
	address = (unsigned int)(__UINTPTR_TYPE__)pointer - xbox_address_space_base;
	if (xbox_ce_alias_active && address - XBOX_CE_ALIAS < XBOX_CE_SIZE)
		address += XBOX_CE_BASE - XBOX_CE_ALIAS;
	return address;
}

#define POINTER_BITS(pointer) ((__UINTPTR_TYPE__)(pointer))

#else /* HALO_3DS */

/* (a build may place it elsewhere: AddressSanitizer's shadow memory lies
where these are, so the sanitized builds put it above it, at 48 TB:
tools/test_map_fuzz.py) */
#ifndef XBOX_ADDRESS_SPACE_BASE
#if defined(HALO_ANDROID)
/* wherever the system has 4 GB free, 4 GB-aligned, found at start-up
(port/linux/src/xbox_memory.c): an Android app's library is loaded into a
process the system has laid out already, and the Java runtime's heaps lie
where a fixed address would (port/android/README.md) */
extern unsigned long long xbox_address_space_base;
#define XBOX_ADDRESS_SPACE_BASE xbox_address_space_base
#elif defined(__linux__) && defined(__aarch64__)
/* 256 GB: below the top of the smallest address space a 64-bit ARM Linux
kernel gives a program (512 GB, with 39-bit addresses: Raspberry Pi OS's
kernels, Android's) */
#define XBOX_ADDRESS_SPACE_BASE 0x4000000000ULL /* 256 GB */
#else
#define XBOX_ADDRESS_SPACE_BASE 0x10000000000ULL /* 1 TB */
#endif
#endif
#define XBOX_ADDRESS_SPACE_SIZE 0x100000000ULL /* 4 GB */

/* a pointer field of an Xbox-layout structure; type documents the target */
#define XPTR(type) unsigned int
/* an XPTR field's null */
#define XBOX_NULL 0

/* Xbox address to host pointer; 0 stays NULL */
static __inline__ __attribute__((always_inline)) void *xbox_pointer(unsigned int address)
{
	return address ? (void *)(XBOX_ADDRESS_SPACE_BASE + address) : (void *)0;
}

/* reports a pointer outside the Xbox address space and aborts */
void xbox_address_out_of_range(void const *pointer) __attribute__((noreturn));

/* host pointer to Xbox address; NULL stays 0 */
static __inline__ __attribute__((always_inline)) unsigned int xbox_address(void const *pointer)
{
	unsigned long long offset = (unsigned long long)(__UINTPTR_TYPE__)pointer - XBOX_ADDRESS_SPACE_BASE;

	if (!pointer)
		return 0;
	if (offset >= XBOX_ADDRESS_SPACE_SIZE)
		xbox_address_out_of_range(pointer);
	return (unsigned int)offset;
}

/* a pointer as an integer, for alignment tests, ordering and differences
(the Xbox code cast pointers to int or unsigned int for these) */
#define POINTER_BITS(pointer) ((__UINTPTR_TYPE__)(pointer))

#endif /* HALO_3DS */

#elif defined(HALO_N3DS) && defined(HALO_CUSTOM_EDITION)

/* port: (the 3DS, Halo3DS's frontend) pointers are native, but a Halo PC
map's are linked to its tag cache at 0x40440000, which a 3DS program cannot
map: that space is a block of the console's memory (port/n3ds/src/
engine_ce_cache.inl), and the fork's Custom Edition code, which reads and
writes the map's addresses with these, has them translated there. Once the
map is converted its pointers are made native (engine_ce_relocate.c): an
address in the space is then left as it is, and only xbox_pointer translates
(a native pointer is never in that range: the 3DS's user space ends at
0x40000000); xbox_address gives the map's addresses back only while its
conversion runs (n3ds_ce_space_converting). */
extern unsigned char *n3ds_ce_space_native;
extern unsigned long n3ds_ce_space_size;
extern int n3ds_ce_space_converting;
#define N3DS_CE_SPACE_BASE 0x40440000UL
/* (the space's range, as the fork's 3DS code checks its tag cache's: its
tag cache and the conversion room above it, port/linux/game/ce_resources.c) */
#define XBOX_CE_BASE 0x40440000U
#define XBOX_CE_SIZE ((unsigned long)n3ds_ce_space_size)

static __inline__ __attribute__((always_inline)) void *n3ds_xbox_pointer(unsigned long address)
{
	if (n3ds_ce_space_native && address - N3DS_CE_SPACE_BASE < n3ds_ce_space_size)
		return n3ds_ce_space_native + (address - N3DS_CE_SPACE_BASE);
	return (void *)address;
}

static __inline__ __attribute__((always_inline)) void *n3ds_xbox_address(const void *pointer)
{
	unsigned long value = (unsigned long)pointer;
	unsigned long native = (unsigned long)n3ds_ce_space_native;

	if (n3ds_ce_space_converting && native && value - native < n3ds_ce_space_size)
		return (void *)(N3DS_CE_SPACE_BASE + (value - native));
	return (void *)pointer;
}

#define XPTR(type) type *
#define XBOX_NULL NULL
#define xbox_pointer(address) n3ds_xbox_pointer((unsigned long)(address))
#define xbox_address(pointer) n3ds_xbox_address((const void *)(pointer))
#define POINTER_BITS(pointer) ((unsigned long)(pointer))

#else

#define XPTR(type) type *
#define XBOX_NULL NULL
#define xbox_pointer(address) ((void *)(address))
#define xbox_address(pointer) ((void *)(pointer))
#define POINTER_BITS(pointer) ((unsigned long)(pointer))

#endif

#define XBOX_POINTER(type, address) ((type *)xbox_pointer(address))
#define XBOX_ADDRESS(pointer) xbox_address(pointer)

#endif // __XBOX_ADDRESS_H
