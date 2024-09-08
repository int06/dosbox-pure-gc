#ifndef __GAMELINK_H___
#define __GAMELINK_H___

#include "dosbox.h"

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

//------------------------------------------------------------------------------
// Namespace Declaration
//------------------------------------------------------------------------------

namespace GameLink
{




	//--------------------------------------------------------------------------
	// Global Functions
	//--------------------------------------------------------------------------

	extern int Init();
	
	extern Bit8u* AllocRAM( const Bit32u size );

	extern void Term();

	extern int In( sSharedMMapInput_R2* p_input,
				   sSharedMMapAudio_R1* p_audio );

	extern void Out( const Bit16u frame_width,
					 const Bit16u frame_height,
					 const double source_ratio,
					 const bool need_mouse,
					 const char* p_program,
					 const Bit32u* p_program_hash,
					 const Bit8u* p_frame,
					 const Bit8u* p_sysmem );

}; // namespace GameLink

//==============================================================================

#endif // __GAMELINK_HDR__
