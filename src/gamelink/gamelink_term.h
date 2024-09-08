#pragma once

#include "config.h"

#if C_GAMELINK

#include "shared_data.h"

namespace GameLink
{

void ExecTerminal(sSharedMMapBuffer_R1* p_inbuf, sSharedMMapBuffer_R1* p_outbuf);

}; // namespace GameLink

#endif // C_GAMELINK
