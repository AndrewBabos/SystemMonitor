#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <atomic>
#include <array>
#include <vector>
#include <Pdh.h>
#include <windows.h>
#include <TlHelp32.h>
#include <pdhmsg.h>


//#define length_cpuBrandStr 0x40

struct ProcessInfo
{
	DWORD pid;
	std::wstring name;
	float cpuUsage;
	SIZE_T memoryUsage;
};

namespace UiController
{
	#define length_cpuBrandStr 0x40

	void renderOptionsAndDockspace();
	void renderCPU(std::atomic<float>& cpuValue, std::array<float, 10>& cpuHistory);
	void renderRAM();
	void renderSysInfo(char CPUBrandString[length_cpuBrandStr], SYSTEM_INFO& sysInfo);
	void renderProcesses(HANDLE& hSnap, PROCESSENTRY32& pe);
	void testingTables(HANDLE& hSnap, PROCESSENTRY32& pe);
};

