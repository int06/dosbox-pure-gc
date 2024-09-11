#pragma once

#include "dosbox.h"

#include <Windows.h>

#define GAMELINK_MUTEX_NAME "DWD_GAMELINK_MUTEX_R4"

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
		handle = CreateMutexA(NULL, FALSE, GAMELINK_MUTEX_NAME);
		if (handle == NULL)
		{
			LOG_MSG("GAMELINK: CreateMutex(%s) failed: %d", GAMELINK_MUTEX_NAME, GetLastError());
			Cleanup();
			return false;
		}
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			LOG_MSG("GAMELINK: CreateMutex(%s) failed: already exists", GAMELINK_MUTEX_NAME);
			LOG_MSG("GAMELINK: Already running Game Link, or maybe a crash?");
			Cleanup();
			return false;
		}
		return true;
	}

	explicit operator bool() const
	{
		return handle != NULL;
	}

	void Cleanup()
	{
		if (handle != NULL)
		{
			CloseHandle(handle);
			handle = NULL;
		}
	}

	bool Acquire()
	{
		if (WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0)
		{
			LOG_MSG("GAMELINK: MUTEX lock failed: %d", GetLastError());
			return false;
		}
		return true;
	}

	void Release()
	{
		ReleaseMutex(handle);
	}

private:
	HANDLE handle = NULL;
};

}; // namespace GameLink
