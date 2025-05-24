#include "memory_info.h"

#include <string>
#include <engine/debug/debug.h>

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#include <Psapi.h>
#include <dxgi1_4.h>
#include <Processthreadsapi.h>
#elif defined(__PSP__)
#include <pspkernel.h>
#include <pspsdk.h>
#include <pspge.h>
#include <pspsysmem_kernel.h>
#include <vram.h>
#elif defined(__PS3__)
#elif defined(__vita__)
//#include <psp2/kernel/sysmem.h>
//#include <psp2/appmgr.h> 
#include <vitaGL.h>
#endif

#if defined(_WIN32) || defined(_WIN64) 
IDXGIAdapter3* adapter;
#endif

#if defined(__PSP__)
#define PSP_FAT_USER_MEMORY_SIZE 0x1800000
#define PSP_SLIM_USER_MEMORY_SIZE (0x1800000 + 0x1C00000)
bool isPspSlim = false;
#endif

void MemoryInfo::Init()
{
#if defined(_WIN32) || defined(_WIN64) 
	IDXGIFactory4* pFactory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);
	pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&adapter));
#endif

#if defined(__PSP__)
	const SceSize pspMem = pspSdkTotalFreeUserMemSize();
	if (pspMem > PSP_FAT_USER_MEMORY_SIZE)
	{
		isPspSlim = true;
	}
	else 
	{
		isPspSlim = false;
	}
#endif
}

size_t MemoryInfo::GetUsedMemory()
{
#if defined(_WIN32) || defined(_WIN64) 
	PROCESS_MEMORY_COUNTERS s_memoryInfo;
	GetProcessMemoryInfo(GetCurrentProcess(), &s_memoryInfo, sizeof(s_memoryInfo));
	return s_memoryInfo.WorkingSetSize;
#elif defined(__PSP__)
	return GetTotalMemory() - pspSdkTotalFreeUserMemSize();
#elif defined(__vita__)
	return 0; // Currently no way to get available memory on the vita because functions do not track newlib heap memory
#else
	return 0;
#endif
}

size_t MemoryInfo::GetTotalMemory()
{
#if defined(_WIN32) || defined(_WIN64) 
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullTotalPhys;
#elif defined(__PSP__)
	return isPspSlim ? PSP_SLIM_USER_MEMORY_SIZE : PSP_FAT_USER_MEMORY_SIZE;
#elif defined(__vita__)
	return 134217728; // Currently no way to get total memory, newlib uses 128 MegaBytes. Is the 134217728 value correct?
#else
	return 0;
#endif
}

size_t MemoryInfo::GetUsedVideoMemory()
{
#if defined(_WIN32) || defined(_WIN64) 
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
	size_t usedVRAM = videoMemoryInfo.CurrentUsage;
	return usedVRAM;
#elif defined(__PSP__)
	return GetTotalVideoMemory() - vmemavail();
#elif defined(__vita__)
	return vglMemTotal(VGL_MEM_VRAM);
#else
	return 0;
#endif
}

size_t MemoryInfo::GetTotalVideoMemory()
{
#if defined(_WIN32) || defined(_WIN64) 
	DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
	size_t maxVRAM = videoMemoryInfo.Budget;
	return maxVRAM;
#elif defined(__PSP__)
	return sceGeEdramGetSize();
#elif defined(__vita__)
	return vglMemFree(VGL_MEM_VRAM);
#else
	return 0;
#endif
}
