#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <array>
#include <vector>
#include <Pdh.h>
#include <windows.h>
#include <TlHelp32.h>
#include <pdhmsg.h>

#include "HardwareController.h"
//#include <Psapi.h>
//#pragma comment(lib, "psapi.lib")

//#define length_cpuBrandStr 0x40
//struct ProcessInfo
//{
//	DWORD pid;
//	std::wstring name;
//	float cpuUsage;
//	SIZE_T memoryUsage;
//};
//
//// had to throw outside the namespace, cant use thread in non static
//static std::vector<ProcessInfo> processList = {};
//void fillProcessList(HANDLE& hSnap, PROCESSENTRY32& pe);


namespace UiController
{
	void renderOptionsAndDockspace();

	void renderCPU(const std::atomic<float>& cpuValue, 
				   const std::array<float, 10>& cpuHistory);

	void renderRAM(const std::atomic<float>& ramValue,
				   const std::array<float, 10>& ramHistory,
				   const std::atomic<int>& ramUsed,
				   const std::atomic<int>& totalPhysRAM);
	
	void renderSysInfo(char CPUBrandString[length_cpuBrandStr], 
		               SYSTEM_INFO& sysInfo);
	void renderProcesses(HANDLE& hSnap, PROCESSENTRY32& pe);
	
	// still testing
	void testingTables(HANDLE& hSnap, PROCESSENTRY32& pe, std::vector<ProcessInfo> processList);
};

