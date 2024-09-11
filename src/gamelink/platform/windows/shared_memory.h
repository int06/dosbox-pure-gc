#pragma once

#include "dosbox.h"

#include <Windows.h>

#define GAMELINK_MMAP_NAME "DWD_GAMELINK_MMAP_R4"

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
		handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)size, GAMELINK_MMAP_NAME);
		if (handle == NULL)
		{
			Cleanup();
			return false;
		}

		ptr = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
		if (ptr == nullptr)
		{
			Cleanup();
			return false;
		}

		return true;
	}

	void Cleanup()
	{
		if (ptr != nullptr)
		{
			UnmapViewOfFile(ptr);
			ptr = nullptr;
		}
		if (handle != NULL)
		{
			CloseHandle(handle);
			handle = NULL;
		}
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
	HANDLE handle = NULL;
	void* ptr = nullptr;
};

}; // namespace GameLink
