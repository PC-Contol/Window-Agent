//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "MainUnit.h"
#include "SystemInfoThread.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
SystemInfo *si;
__fastcall SystemInfoThread::SystemInfoThread(SystemInfo *pSi)
	: TThread(false)
{
	si = pSi;

}

//---------------------------------------------------------------------------
void __fastcall SystemInfoThread::Execute()
{
	DWORD siTick = 0;
	GetSystemInfo();
	while (!this->Terminated)
	{
		if (GetTickCount() - siTick > 6 * 1000)
		{
			GetSystemInfo();
			siTick = GetTickCount();
		}
		Sleep(1000);
	}

	//---- Place thread code here ----
}
//---------------------------------------------------------------------------

void GetSystemInfo()
{
	GetHddInfo();
	GetMemInfo();
	getCPU();
}


void getCPU(void)
{
	FILETIME idleTime;   	// ¦¤
	FILETIME kernelTime; 	// ¦¢ GetSystemTimes's parameter
	FILETIME userTime;   	// ¦¥
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	DWORD kernel1_high = kernelTime.dwHighDateTime;
	// In first kernel's HighDataTime
	DWORD kernel1_low = kernelTime.dwLowDateTime;
	// In first kernel's LowDataTime
	DWORD user1_high = userTime.dwHighDateTime; // In first user's HighDataTime
	DWORD user1_low = userTime.dwLowDateTime; // In first user's LowDataTime
	DWORD idle1_high = idleTime.dwHighDateTime; // In first idle's HighDataTime
	DWORD idle1_low = idleTime.dwLowDateTime; // In first idle's LowDataTime
	DWORD high1 = kernel1_high + user1_high + idle1_high;
	// Add each first HighTime
	Sleep(1000); // 1000 is 1Sec
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	DWORD kernel2_high = kernelTime.dwHighDateTime;
	// In second kernel's HighDataTime
	DWORD kernel2_low = kernelTime.dwLowDateTime;
	// In second kernel's LowDataTime
	DWORD user2_high = userTime.dwHighDateTime; // In second user's HighDataTime
	DWORD user2_low = userTime.dwLowDateTime; // In second user's LowDataTime
	DWORD idle2_high = idleTime.dwHighDateTime; // In second idle's HighDataTime
	DWORD idle2_low = idleTime.dwLowDateTime; // In second idle's LowDataTime
	DWORD high2 = kernel2_high + user2_high + idle2_high;
	// Add each second HighTime
	DWORD high = high2 - high1; // differ to first HighTime and second HighTime
	__int64 kernel = kernel2_low - kernel1_low; // In used kernel time in 1 sec
	__int64 user = user2_low - user1_low; // In used user time in 1 sec
	__int64 idle = idle2_low - idle1_low; // in used idle time in 1 sec

	if (high) { // if high != 0
		if (high == 1) { // many times tested -> high no more than 3
			kernel = (kernel / 10) + 10000000000;
			// ¦¤ Effection of "high & low" ex) high=2, low=100 -> 210 (& : connection operator)
			user = (user / 10) + 10000000000;
			// ¦¢ Why 210 not 2100? : Because only compare to 3 values(kernel,user,idle) for get CPU rate
			idle = (idle / 10) + 10000000000; // ¦¥  (ex> 100/210 = 1000/2100)
		}
		if (high == 2) {
			kernel = (kernel / 10) + 20000000000;
			user = (user / 10) + 20000000000;
			idle = (idle / 10) + 20000000000;
		}
	}

	// get CPU rate
	// Why __int64?
	// Because DWORD < __int64 (compare to data size) / DWORD + DWORD -> sometimes overflow occurrence
	ULONGLONG delta = ((user + kernel) - idle) * 100 / (user + kernel);
	si->UseCpu = delta;
//	return delta;
}

void GetHddInfo()
{
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	ULONGLONG FreeSpace, TotalSpace;
	String szDrive = "C:\\";
	::GetDiskFreeSpaceEx(szDrive.c_str(), &FreeBytesAvailableToCaller,
		&TotalNumberOfBytes, &TotalNumberOfFreeBytes);
	FreeSpace = TotalNumberOfFreeBytes.QuadPart;
	TotalSpace = TotalNumberOfBytes.QuadPart;
	si->Hdd.Use = TotalSpace - FreeSpace;
	si->Hdd.Total = TotalSpace;
}

void GetMemInfo()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	si->Mem.Use = statex.ullTotalPhys - statex.ullAvailPhys;
	si->Mem.Total = statex.ullTotalPhys;
}
