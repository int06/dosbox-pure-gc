#include "config.h"
#if C_GAMELINK

#include "gamelink_term.h"

void GameLink::ExecTerminal(sSharedMMapBuffer_R1& inbuf, sSharedMMapBuffer_R1& outbuf)
{
	// Nothing from the host, or host hasn't acknowledged our last message.
	if ( inbuf.payload == 0 ) {
		return;
	}
	if ( outbuf.payload > 0 ) {
		return;
	}

	// No mechanical commands implemented and human commands are deprecated

	// Acknowledge
	inbuf.payload = 0;
}

#endif // C_GAMELINK
