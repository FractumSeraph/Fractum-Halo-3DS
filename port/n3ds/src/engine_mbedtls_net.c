/* What Mbed TLS asks of the system on the 3DS, which its own modules do not
 * know (they take only Unix and Windows): the socket functions the fork's
 * HTTPS client (port/linux/src/posix_browser.c) gives a TLS session, which
 * connects its socket itself, over libctru's sockets, as Mbed TLS's
 * net_sockets.c has them on Unix; and its millisecond clock
 * (MBEDTLS_PLATFORM_MS_TIME_ALT). net_sockets.c and timing.c are not built
 * (port/n3ds/sources.json). */
#include <3ds.h>
#include <errno.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/platform_time.h"
#include "mbedtls/ssl.h"

int posix_3ds_poll(struct pollfd *descriptors, int count, int timeout); /* (port/linux/src/posix_net.c) */

void mbedtls_net_init(mbedtls_net_context *ctx)
{
    ctx->fd = -1;
}

void mbedtls_net_free(mbedtls_net_context *ctx)
{
    if (ctx->fd < 0)
        return;
    shutdown(ctx->fd, SHUT_RDWR);
    close(ctx->fd);
    ctx->fd = -1;
}

static int would_block(void)
{
    return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS;
}

int mbedtls_net_send(void *context, const unsigned char *buf, size_t len)
{
    mbedtls_net_context *ctx = context;
    int ret;

    if (ctx->fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    ret = (int)send(ctx->fd, buf, len, 0);
    if (ret >= 0)
        return ret;
    if (would_block() || errno == EINTR)
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    if (errno == EPIPE || errno == ECONNRESET)
        return MBEDTLS_ERR_NET_CONN_RESET;
    return MBEDTLS_ERR_NET_SEND_FAILED;
}

int mbedtls_net_recv(void *context, unsigned char *buf, size_t len)
{
    mbedtls_net_context *ctx = context;
    int ret;

    if (ctx->fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    ret = (int)recv(ctx->fd, buf, len, 0);
    if (ret >= 0)
        return ret;
    if (would_block() || errno == EINTR)
        return MBEDTLS_ERR_SSL_WANT_READ;
    if (errno == EPIPE || errno == ECONNRESET)
        return MBEDTLS_ERR_NET_CONN_RESET;
    return MBEDTLS_ERR_NET_RECV_FAILED;
}

int mbedtls_net_recv_timeout(void *context, unsigned char *buf, size_t len, uint32_t timeout)
{
    mbedtls_net_context *ctx = context;
    struct pollfd descriptor;
    int ret;

    if (ctx->fd < 0)
        return MBEDTLS_ERR_NET_INVALID_CONTEXT;
    descriptor.fd = ctx->fd;
    descriptor.events = POLLIN;
    descriptor.revents = 0;
    /* (0: wait as long as it takes, as Mbed TLS's own; a slice at a time,
     * the socket service free between: posix_3ds_poll) */
    ret = posix_3ds_poll(&descriptor, 1, timeout == 0 ? -1 : (int)timeout);
    if (ret == 0)
        return MBEDTLS_ERR_SSL_TIMEOUT;
    if (ret < 0)
        return errno == EINTR ? MBEDTLS_ERR_SSL_WANT_READ : MBEDTLS_ERR_NET_RECV_FAILED;
    return mbedtls_net_recv(ctx, buf, len);
}

mbedtls_ms_time_t mbedtls_ms_time(void)
{
    /* (milliseconds since 1900, which only differences are taken of) */
    return (mbedtls_ms_time_t)osGetTime();
}
