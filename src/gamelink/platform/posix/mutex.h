#pragma once

#include "dosbox.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

#define GAMELINK_MUTEX_NAME		"/DWD_GAMELINK_MUTEX_R4"

namespace GameLink
{

class Mutex
{
public:
	Mutex() = default;

	Mutex(const Mutex&) = delete;
	Mutex operator=(const Mutex&) = delete;

	Mutex(Mutex&&) = delete;
	Mutex operator=(Mutex&&) = delete;

	~Mutex()
	{
		Cleanup();
	}

	bool Init()
	{
		handle = sem_open(GAMELINK_MUTEX_NAME, O_CREAT | O_EXCL, 0666, 1);
		if (handle == SEM_FAILED)
		{
			LOG_MSG("GAMELINK: sem_open(%s) failed: %d", GAMELINK_MUTEX_NAME, errno);
#ifdef MACOSX
			LOG_MSG("GAMELINK: Might need to manually reboot the system.");
#else // MACOSX
			LOG_MSG("GAMELINK: Might need to manually tidy up in /dev/shm (or reboot system).");
#endif // MACOSX
			return false;
		}
		return true;
	}

	bool Cleanup()
	{
		if (handle != nullptr) {
			sem_close(handle);
			sem_unlink(GAMELINK_MUTEX_NAME);
			handle = nullptr;
		}
	}

	explicit operator bool() const
	{
		return handle != nullptr;
	}

	bool Acquire()
	{
		if (sem_wait(handle) < 0)
		{
			LOG_MSG("GAMELINK: MUTEX lock failed: errno = %d", errno);
			return false;
		}
		return true;
	}

	void Release()
	{
		if (sem_post(handle) < 0) {
			LOG_MSG("GAMELINK: MUTEX unlock failed: errno = %d", errno);
		}
	}

private:
	sem_t* handle = nullptr;
};

}; // namespace GameLink
