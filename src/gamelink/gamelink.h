#pragma once

#include "dosbox.h"

namespace GameLink
{

	bool Init();
	
	Bit8u* AllocRAM(const Bit32u size);

	void Term();

	void Out(const char* p_program, const Bit32u* p_program_hash, const Bit8u* p_sysmem);

}; // namespace GameLink
