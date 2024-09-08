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
	// Global Declarations
	//--------------------------------------------------------------------------

#pragma pack( push, 1 )

	//
	// sSharedMMapFrame_R1
	//
	// Server -> Client Frame. 32-bit RGBA up to MAX_WIDTH x MAX_HEIGHT
	//
	struct sSharedMMapFrame_R1
	{
		Bit16u seq;
		Bit16u width;
		Bit16u height;

		Bit8u image_fmt; // 0 = no frame; 1 = 32-bit 0xAARRGGBB
		Bit8u reserved0;

		Bit16u par_x; // pixel aspect ratio
		Bit16u par_y;

		enum { MAX_WIDTH = 1280 };
		enum { MAX_HEIGHT = 1024 };

		enum { MAX_PAYLOAD = MAX_WIDTH * MAX_HEIGHT * 4 };
		Bit8u buffer[ MAX_PAYLOAD ];
	};

	//
	// sSharedMMapInput_R2
	//
	// Client -> Server Input Data
	//
	struct sSharedMMapInput_R2
	{
		float mouse_dx;
		float mouse_dy;
		Bit8u ready;
		Bit8u mouse_btn;
		Bit32u keyb_state[ 8 ];
	};

	//
	// sSharedMMapPeek_R2
	//
	// Memory reading interface.
	//
	struct sSharedMMapPeek_R2
	{
		enum { PEEK_LIMIT = 16 * 1024 };

		Bit32u addr_count;
		Bit32u addr[ PEEK_LIMIT ];
		Bit8u data[ PEEK_LIMIT ];
	};

	//
	// sSharedMMapBuffer_R1
	//
	// General buffer (64Kb)
	//
	struct sSharedMMapBuffer_R1
	{
		enum { BUFFER_SIZE = ( 64 * 1024 ) };

		Bit16u payload;
		Bit8u data[ BUFFER_SIZE ];
	};

	//
	// sSharedMMapAudio_R1
	//
	// Audio control interface.
	//
	struct sSharedMMapAudio_R1
	{
		Bit8u master_vol_l;
		Bit8u master_vol_r;
	};

	//
	// sSharedMemoryMap_R4
	//
	// Memory Map (top-level object)
	//
	struct sSharedMemoryMap_R4
	{
		enum {
			FLAG_WANT_KEYB			= 1 << 0,
			FLAG_WANT_MOUSE			= 1 << 1,
			FLAG_NO_FRAME			= 1 << 2,
			FLAG_PAUSED				= 1 << 3,
		};

		enum {
			SYSTEM_MAXLEN			= 64
		};

		enum {
			PROGRAM_MAXLEN			= 260
		};

		Bit8u version; // = PROTOCOL_VER
		Bit8u flags;
		char system[ SYSTEM_MAXLEN ]; // System name.
		char program[ PROGRAM_MAXLEN ]; // Program name. Zero terminated.
		Bit32u program_hash[ 4 ]; // Program code hash (256-bits)

		sSharedMMapFrame_R1 frame;
		sSharedMMapInput_R2 input;
		sSharedMMapPeek_R2 peek;
		sSharedMMapBuffer_R1 buf_recv; // a message to us.
		sSharedMMapBuffer_R1 buf_tohost;
		sSharedMMapAudio_R1 audio;

		// added for protocol v4
		Bit32u ram_size;
	};

#pragma pack( pop )


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

	extern void ExecTerminal( sSharedMMapBuffer_R1* p_inbuf,
							  sSharedMMapBuffer_R1* p_outbuf,
							  sSharedMMapBuffer_R1* p_mechbuf );

	extern void ExecTerminalMech( sSharedMMapBuffer_R1* p_mechbuf );

	extern void InitTerminal();

}; // namespace GameLink

//==============================================================================

#endif // __GAMELINK_HDR__
