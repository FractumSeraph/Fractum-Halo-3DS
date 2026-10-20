/* What the FractumSeraph/OpenCE fork's platform layer asks of the system
 * that newlib and libctru do not have on the 3DS, as its own 3DS port
 * answers it: one user, and no UPnP (internet play then reaches other
 * machines through its tunnel's hole punching, and its relays). */
#include <3ds.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include "../../linux/src/posix.h"

/* (the SD card's files have owner 0: the secret files' owner check,
 * posix_net.c's, takes them as this user's) */
uid_t getuid(void)
{
    return 0;
}

int posix_upnp_forward_udp(unsigned short port, unsigned short preferred_port, posix_ulong *external_address,
    unsigned short *external_port, char *error, int error_size)
{
    (void)port;
    (void)preferred_port;
    if (external_address)
        *external_address = 0;
    if (external_port)
        *external_port = 0;
    if (error && error_size > 0)
        snprintf(error, (size_t)error_size, "no UPnP on the 3DS");
    return 0;
}

void posix_upnp_stop_forwarding_udp(unsigned short external_port)
{
    (void)external_port;
}

/* Mbed TLS's entropy (MBEDTLS_ENTROPY_HARDWARE_ALT; the 3DS has no
 * /dev/urandom, MBEDTLS_NO_PLATFORM_ENTROPY): the system's random numbers,
 * its PS service's, which posix_net.c's posix_random_bytes takes too */
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t length, size_t *written)
{
    static int ready;

    (void)data;
    *written = 0;
    if (!ready)
        ready = R_SUCCEEDED(psInit());
    if (!ready || R_FAILED(PS_GenerateRandomBytes(output, length)))
        return -1;
    *written = length;
    return 0;
}
