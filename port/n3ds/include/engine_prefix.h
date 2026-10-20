#ifndef HALO_N3DS_ENGINE_PREFIX_H
#define HALO_N3DS_ENGINE_PREFIX_H
/* Compile original engine declarations against the supplied XDK interfaces.
 * _X86_ selects the original 32-bit SDK layouts, not executable x86 code.
 * tools/compat.py replaces only the SDK's three x86 shift helpers with the
 * existing upstream C equivalents. Hardware services still need real adapters. */
#define _X86_ 1
#define _M_IX86 600
#define _STDCALL_SUPPORTED 1
#define _INTEGRAL_MAX_BITS 64
#define _WCHAR_T_DEFINED
#define _USE_MATH_DEFINES
#define __export
#define DECLSPEC_SELECTANY __attribute__((weak))
#define _InterlockedCompareExchange halo_n3ds_InterlockedCompareExchange
#define _InterlockedDecrement halo_n3ds_InterlockedDecrement
#define _InterlockedExchange halo_n3ds_InterlockedExchange
#define _InterlockedExchangeAdd halo_n3ds_InterlockedExchangeAdd
#define _InterlockedIncrement halo_n3ds_InterlockedIncrement
#define __try if (1)
#define __except(filter) else if (0)
#define __finally
#define __leave
/* (the fork's socket sets: room for its 128 machines, halo_port_limits.h) */
#define FD_SETSIZE 256
#include "platform.h"
/* The engine's Xbox Winsock calls go to the fork's Winsock layer
 * (port/linux/src/xnet.c), under its private names: libctru's BSD socket
 * functions have the same names but other structures. The platform layer's
 * own units (HALO_LINUX_PLATFORM_LAYER) take the names as the system's. */
#ifndef HALO_LINUX_PLATFORM_LAYER
#include "halo_linux_winsock_names.h"
#endif
/* These declarations are also used by the upstream ARM port. Variadic calls
 * must see their real prototypes on ARM hard-float. */
union real_argb_color;
void error(short priority, const char *format, ...);
void console_printf(unsigned char clear, const char *format, ...);
void terminal_printf(const union real_argb_color *color, const char *format, ...);
/* The FractumSeraph/OpenCE fork's engine: its native builds' multiplayer
 * session limits (which its servers' games take) and its per-platform
 * capacities, the 3DS's the Xbox's (port/linux/include). */
#include "halo_port_limits.h"
#include "halo_port_capacity.h"
/* (and its port functions that the engine calls, declared for it) */
#include "halo_linux_source_fixups.h"
/* (the C library's declarations the fork's engine takes from its prefix,
 * halo_linux_prefix.h, and its timing functions, cseries_windows.h) */
#include <string.h>
#include <stdlib.h>
unsigned long system_milliseconds(void);
/* The fork's engine opens files by Xbox path (d:\maps_ce\..., z:\...),
 * which its builds translate as CreateFile does (port/linux/include/stdio.h):
 * here, as engine_win32_files.c does (n3ds_xbox_path). A native path
 * (sdmc:/...) is left as it is. */
#ifndef HALO_LINUX_PLATFORM_LAYER
#include <stdio.h>
FILE *n3ds_xbox_fopen(const char *path, const char *mode);
FILE *n3ds_xbox_freopen(const char *path, const char *mode, FILE *stream);
int n3ds_xbox_remove(const char *path);
int n3ds_xbox_rename(const char *old_path, const char *new_path);
#define fopen n3ds_xbox_fopen
#define freopen n3ds_xbox_freopen
#define remove n3ds_xbox_remove
#define rename n3ds_xbox_rename
#endif
/* (structures the fork's engine renamed, under Halo3DS's names) */
#define rasterizer_model_skinning render_skinning
#include "engine_fork_exports.h"
#endif
