#pragma once

#include "dosbox.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined(MACOSX)
	#define GAMELINK_MMAP_NAME "/DWD_GAMELINK_MMAP_R4"
#else // MACOSX
	#define GAMELINK_MMAP_NAME "DWD_GAMELINK_MMAP_R4"
#endif // MACOSX

namespace GameLink
{

class SharedMemory
{
public:
	SharedMemory() = default;

	SharedMemory(const SharedMemory&) = delete;
	SharedMemory operator=(const SharedMemory&) = delete;

	SharedMemory(SharedMemory&&) = delete;
	SharedMemory operator=(SharedMemory&&) = delete;

	~SharedMemory()
	{
		Cleanup();
	}

	bool Init(size_t size)
	{
		const auto mode = O_CREAT | O_RDWR
#ifndef MACOSX
			| O_TRUNC
#endif // !MACOSX
			;

		fd = shm_open(GAMELINK_MMAP_NAME, mode, 0666);
		if (fd < 0) {
			LOG_MSG("GAMELINK: shm_open(%s) failed: errno = %d", GAMELINK_MMAP_NAME, errno);
			Cleanup();
			return false;
		}

		if (ftruncate(fd, size) < 0) {
			LOG_MSG("GAMELINK: ftruncate failed: errno = %d", errno);
			Cleanup();
			return false;
		}

		ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptr == MAP_FAILED)
		{
			LOG_MSG("GAMELINK: mmap failed: errno = %d", errno);
			Cleanup();
			return false;
		}
		sz = size;

		return true;
	}

	void Cleanup()
	{
		if (ptr != nullptr)
		{
			munmap(ptr, sz);
			ptr = nullptr;
			sz = 0;

		}
		if (0 <= fd)
		{
			close(fd);
			fd = -1;
		}
		shm_unlink(GAMELINK_MMAP_NAME);
	}

	explicit operator bool() const
	{
		return ptr != nullptr;
	}

	void* GetPtr()
	{
		return ptr;
	}

private:
	int fd = -1;
	void* ptr = nullptr;
	size_t sz = 0;
};

}; // namespace GameLink
