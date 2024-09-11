#include "config.h"
#if C_GAMELINK

#include "gamelink.h"

#include "shared_data.h"
#include "gamelink_term.h"

#include "dosbox.h"

#if defined(_WIN32)

#include "platform/windows/mutex.h"
#include "platform/windows/shared_memory.h"

#elif defined(__unix__) || defined(MACOSX)

#include "platform/posix/mutex.h"
#include "platform/posix/shared_memory.h"

#else

#error "Unsupported system"

#endif

#undef min

#include <algorithm>

namespace GameLink
{

class State
{
public:
	State() = default;

	State(const State&) = delete;
	State operator=(const State&) = delete;

	State(State&&) = delete;
	State operator=(State&&) = delete;

	~State()
	{
		Cleanup();
	}

	bool Init()
	{
		LOG_MSG("GameLink::Init");

		// Already initialised?
		if (mutex)
		{
			LOG_MSG("GAMELINK: Ignoring re-initialisation.");
			return true;
		}

		if (!mutex.Init())
		{
			LOG_MSG("GAMELINK: Couldn't initialise inter-process communication");
			return false;
		}

		return true;
	}

	void Cleanup()
	{
		LOG_MSG("GameLink::Term");

		if (shared_data != nullptr)
		{
			// SEND ABORT CODE TO CLIENT (don't care if it fails)
			shared_data->version = 0;
		}
		shared_data = nullptr;
		membase_ptr = nullptr;
		membase_size = 0;

		shared_memory.Cleanup();
		mutex.Cleanup();
	}

	Bit8u* AllocRAM(const Bit32u size)
	{
		LOG_MSG("GameLink::AllocRAM(%d)", size);

		if (!mutex)
		{
			LOG_MSG("GAMELINK: mutex not initialized");
			return nullptr;
		}

		if (shared_memory)
		{
			LOG_MSG("GAMELINK: memory already allocated");
			return nullptr;
		}

		const auto total_size = sizeof(sSharedMemoryMap_R4) + size;
		if (!shared_memory.Init(total_size))
		{
			LOG_MSG("GAMELINK: memory alllocation failed");
			return nullptr;
		}

		auto ptr = shared_memory.GetPtr();
		shared_data  = static_cast<sSharedMemoryMap_R4*>(ptr);
		membase_ptr  = static_cast<Bit8u*>(ptr) + sizeof(sSharedMemoryMap_R4);
		membase_size = size;

		shared_data->Init(size);

		LOG_MSG("GAMELINK: Initialised. Allocated %d MB of shared memory.", (total_size + (1024 * 1024) - 1) / (1024 * 1024));

		return membase_ptr;
	}

	void Out(const char* program_name, const ProgramHash& program_hash, const Bit8u* sysmem_ptr)
	{
		if (!mutex)
		{
			LOG_MSG("GAMELINK: mutex not initialized");
			return;
		}

		if (!shared_memory)
		{
			LOG_MSG("GAMELINK: shared memory not initialized");
			return;
		}

		if (mutex.Acquire())
		{
			// Set version
			shared_data->SetProtocol();

			// Set program
			strncpy(shared_data->program, program_name, 256);
			shared_data->program_hash = program_hash;

			// Store flags

			// Tracking Only - DOSBox handles video/input as usual.
			shared_data->flags = sSharedMemoryMap_R4::Flags::NO_FRAME;

			// Peek
			for (Bit32u pindex = 0; pindex < std::min(shared_data->peek.addr_count, sSharedMMapPeek_R2::PEEK_LIMIT); ++pindex)
			{
				// read address
				Bit32u address = shared_data->peek.addr[pindex];

				// dosbox_pure allocates larger program environment block than classic dosbox 
				// (code under C_DBP_RELIABLE_MEMORY_ADDRESSES condition in MakeEnv() function 
				// in dos_execute.cpp file at line ~180), so all (?) program adresses are moved 
				// a bit higher than Grid Cartographer expects them to be
				address += 0x60;

				const Bit8u data = (address < membase_size) ? sysmem_ptr[address] : 0;

				// write data
				shared_data->peek.data[pindex] = data;
			}

			// Message Processing.
			ExecTerminal(shared_data->buf_recv, shared_data->buf_tohost);

			mutex.Release();
		}
	}

private:
	Mutex mutex;
	SharedMemory shared_memory;

	sSharedMemoryMap_R4* shared_data = nullptr;
	Bit8u* membase_ptr = nullptr;
	Bit32u membase_size = 0;
};

//==============================================================================

static State g_state;

bool Init()
{	
	return g_state.Init();
}

Bit8u* AllocRAM(const Bit32u size)
{	
	return g_state.AllocRAM(size);	
}

void Out(const char* program_name, const ProgramHash& program_hash, const Bit8u* sysmem_ptr)
{
	g_state.Out(program_name, program_hash, sysmem_ptr);
}

void Term()
{
	g_state.Cleanup();
}

//==============================================================================

}; // namespace GameLink

#endif // C_GAMELINK
