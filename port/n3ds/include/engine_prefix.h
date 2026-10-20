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
/* (structures the fork's engine renamed, under Halo3DS's names) */
#define rasterizer_model_skinning render_skinning
#include "engine_fork_exports.h"
#endif
