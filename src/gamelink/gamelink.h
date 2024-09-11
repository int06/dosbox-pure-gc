#pragma once

#include "dosbox.h"

#include "program_hash.h"

namespace GameLink
{

bool Init();
	
Bit8u* AllocRAM(const Bit32u size);

void Term();

void Out(const char* program_name, const ProgramHash& program_hash, const Bit8u* sysmem_ptr);

}; // namespace GameLink
