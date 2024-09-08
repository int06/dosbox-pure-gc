#include "gamelink_term.h"

#include "config.h"

#if C_GAMELINK

void GameLink::ExecTerminal(sSharedMMapBuffer_R1* p_inbuf, sSharedMMapBuffer_R1* p_outbuf)
{
	// Nothing from the host, or host hasn't acknowledged our last message.
	if ( p_inbuf->payload == 0 ) {
		return;
	}
	if ( p_outbuf->payload > 0 ) {
		return;
	}

	// No mechanical commands implemented and human commands are deprecated

	// Acknowledge
	p_inbuf->payload = 0;
}

#endif // C_GAMELINK
