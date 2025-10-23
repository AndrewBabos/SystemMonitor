#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <array>
#include <unordered_map>
#include <vector>
#include <Pdh.h>
#include <windows.h>
#include <TlHelp32.h>
#include <pdhmsg.h>
#include <codecvt>

#include "HardwareController.h"
//#include <Psapi.h>
//#pragma comment(lib, "psapi.lib")


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
	
	// testing
	//void renderProcessesTable(HANDLE& hSnap, PROCESSENTRY32& pe, std::vector<ProcessInfo> processList);
	void renderProcessesTable(HANDLE& hSnap, PROCESSENTRY32& pe, const std::unordered_map<std::string, std::vector<ProcessInfo>> processMap);
};

