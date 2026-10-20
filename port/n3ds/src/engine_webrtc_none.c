/* WebRTC for internet play (p2p_internal.h's p2p_webrtc_*) on the 3DS: none,
 * as in the FractumSeraph/OpenCE fork's browser build (its
 * port/web/src/web_webrtc_none.c, which this is): the signalling says this
 * machine has no WebRTC, and every session is the tunnel p2p.c makes (the
 * fork's own 3DS port does the same). */

#include "posix.h"
#include "p2p_internal.h"

#include <string.h>

int p2p_webrtc_ready(void)
{
	return 1;
}

int p2p_webrtc_describe(const unsigned char *identifier, int proven, struct p2p_webrtc *local,
	struct p2p_candidate *candidates, int maximum_count)
{
	(void)identifier;
	(void)proven;
	memset(local, 0, sizeof(*local));
	local->kind = _p2p_webrtc_none;
	return p2p_local_candidates(candidates, maximum_count);
}

void p2p_webrtc_new_request(void)
{
}

int p2p_webrtc_offered(int connection, int peer, const unsigned char *secret, const struct p2p_webrtc *remote,
	const struct p2p_candidate *candidates, int count, int is_host)
{
	(void)connection;
	(void)peer;
	(void)secret;
	(void)remote;
	(void)candidates;
	(void)count;
	(void)is_host;
	return -1;
}

void p2p_webrtc_close(int connection)
{
	(void)connection;
}

void p2p_webrtc_send(int connection, const void *packet, int size)
{
	(void)connection;
	(void)packet;
	(void)size;
}

int p2p_webrtc_received(const unsigned char *packet, int size, unsigned long address, unsigned short port)
{
	(void)packet;
	(void)size;
	(void)address;
	(void)port;
	return 0;
}

int p2p_webrtc_update(void)
{
	return 0;
}
