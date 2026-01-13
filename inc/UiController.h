#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <cstring>
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

//enum class UiStates
//{
//	Idle,
//	MenuOptions
//};

namespace UiController
{
	// let me cook
	static bool isSelected = false;
	static uint16_t selectedIndex = -1;
	static std::string nameTemp = "";
	//static char pidTempStr[16]{};
	static DWORD pidTemp;

	void renderOptionsAndDockspace();
	void renderSysInfo(std::string CPUBrandString, SYSTEM_INFO& sysInfo);

	void renderCPU(const std::atomic<float>& cpuValue, 
				   const std::array<float, 10>& cpuHistory,
				   const std::vector<std::array<float, 10>>& coreHistories);

	void renderRAM(const std::atomic<float>& ramValue,
				   const std::array<float, 10>& ramHistory,
				   const std::atomic<uint64_t>& ramUsed,
				   const std::atomic<uint64_t>& totalPhysRAM);

	void renderGPU();

	void renderNetwork();
	void renderProcesses(const HANDLE& hSnap, const PROCESSENTRY32& pe, const std::map<std::string, std::vector<ProcessInfo>> processMap);
};

