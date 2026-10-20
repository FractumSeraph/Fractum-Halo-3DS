/* The Win32 performance counter, which the FractumSeraph/OpenCE fork's
 * engine reads for its collision and profiling timers, over the 3DS's tick
 * counter (engine_platform.c). */
#include <xtl.h>

long long n3ds_engine_ticks(void);
long long n3ds_engine_tick_frequency(void);

BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *counter)
{
    if (!counter) return FALSE;
    counter->QuadPart = n3ds_engine_ticks();
    return TRUE;
}

BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER *frequency)
{
    if (!frequency) return FALSE;
    frequency->QuadPart = n3ds_engine_tick_frequency();
    return TRUE;
}
