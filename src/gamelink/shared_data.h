#pragma once

#include "dosbox.h"
#include "program_hash.h"
#include "utils.h"

#include <string.h>
#include <type_traits>

namespace GameLink
{

#pragma pack( push, 1 )

	//
	// sSharedMMapFrame_R1
	//
	// Server -> Client Frame. 32-bit RGBA up to MAX_WIDTH x MAX_HEIGHT
	//
	struct sSharedMMapFrame_R1
	{
		static const size_t MAX_WIDTH	= 1280;
		static const size_t MAX_HEIGHT	= 1024;
		static const size_t MAX_PAYLOAD = MAX_WIDTH * MAX_HEIGHT * 4;

		Bit16u seq;
		Bit16u width;
		Bit16u height;

		Bit8u image_fmt; // 0 = no frame; 1 = 32-bit 0xAARRGGBB
		Bit8u reserved0;

		Bit16u par_x; // pixel aspect ratio
		Bit16u par_y;

		Bit8u buffer[MAX_PAYLOAD];

		void Init()
		{
			seq = 0;
			image_fmt = 0; // = no frame
			width = 0;
			height = 0;

			par_x = 1;
			par_y = 1;
			memset(buffer, 0, sizeof(buffer));
		}
	};

	//
	// sSharedMMapInput_R2
	//
	// Client -> Server Input Data
	//
	struct sSharedMMapInput_R2
	{
		float	mouse_dx;
		float	mouse_dy;
		Bit8u	ready;
		Bit8u	mouse_btn;
		Bit32u	keyb_state[8];

		void Init()
		{
			mouse_dx = 0;
			mouse_dy = 0;

			mouse_btn = 0;
			for (int i = 0; i < COUNTOF(keyb_state); ++i) {
				keyb_state[i] = 0;
			}
		}

	};

	//
	// sSharedMMapPeek_R2
	//
	// Memory reading interface.
	//
	struct sSharedMMapPeek_R2
	{
		static const Bit32u PEEK_LIMIT = 16 * 1024;

		Bit32u	addr_count;
		Bit32u	addr[PEEK_LIMIT];
		Bit8u	data[PEEK_LIMIT];

		void Init()
		{
			addr_count = 0;
			memset(addr, 0, sizeof(addr));
			memset(data, 0, sizeof(data));
		}
	};

	//
	// sSharedMMapBuffer_R1
	//
	// General buffer (64Kb)
	//
	struct sSharedMMapBuffer_R1
	{
		static const size_t BUFFER_SIZE = 64 * 1024;

		Bit16u	payload;
		Bit8u	data[BUFFER_SIZE];
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

		void Init()
		{
			// 100%
			master_vol_l = 100;
			master_vol_r = 100;
		}
	};

	struct sSharedMemoryMapProgHash_R4 {
		Bit32u head_checksum;
		Bit32u total_size;
		Bit32u total_checksum;
		Bit32u reserved;

		void Init() {
			head_checksum = 0;
			total_size = 0;
			total_checksum = 0;
			reserved = 0;
		}

		sSharedMemoryMapProgHash_R4& operator=(const ProgramHash& program_hash)
		{
			head_checksum = program_hash.head_checksum;
			total_checksum = program_hash.total_checksum;
			total_size = program_hash.total_size;
			reserved = 0;

			return *this;
		}
	};

	//
	// sSharedMemoryMap_R4
	//
	// Memory Map (top-level object)
	//
	struct sSharedMemoryMap_R4
	{
		static const Bit8u ProtocolVersion = 4;

		enum class Flags : Bit8u {
			WANT_KEYB	= 1 << 0,
			WANT_MOUSE	= 1 << 1,
			NO_FRAME	= 1 << 2,
			PAUSED		= 1 << 3,
		};

		static const size_t SYSTEM_MAXLEN  =  64;
		static const size_t PROGRAM_MAXLEN = 260;

		Bit8u version; // = PROTOCOL_VER
		Flags flags;
		char system[SYSTEM_MAXLEN];		// System name.
		char program[PROGRAM_MAXLEN];	// Program name. Zero terminated.
		sSharedMemoryMapProgHash_R4 program_hash;

		sSharedMMapFrame_R1 frame;
		sSharedMMapInput_R2 input;
		sSharedMMapPeek_R2 peek;
		sSharedMMapBuffer_R1 buf_recv; // a message to us.
		sSharedMMapBuffer_R1 buf_tohost;
		sSharedMMapAudio_R1 audio;

		// added for protocol v4
		Bit32u ram_size;

		void Init(Bit32u ram_size_)
		{
			SetProtocol();
			flags = static_cast<Flags>(0);

			memset(system, 0, sizeof(system));
			strcpy(system, "DOSBox");

			memset(program, 0, sizeof(program));

			program_hash.Init();

			input.Init();
			peek.Init();
			frame.Init();
			audio.Init();

			ram_size = ram_size_;
		}

		void SetProtocol() {
			version = ProtocolVersion;
		}

	};

#pragma pack( pop )

	inline sSharedMemoryMap_R4::Flags operator|(sSharedMemoryMap_R4::Flags f1, sSharedMemoryMap_R4::Flags f2)
	{
		using int_type = std::underlying_type<sSharedMemoryMap_R4::Flags>::type;
		return static_cast<sSharedMemoryMap_R4::Flags>(static_cast<int_type>(f1) | static_cast<int_type>(f2));
	}

	inline sSharedMemoryMap_R4::Flags& operator|=(sSharedMemoryMap_R4::Flags& f1, sSharedMemoryMap_R4::Flags f2)
	{
		f1 = f1 | f2;
		return f1;
	}

}; // namespace GameLink
