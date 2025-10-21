#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <cstdint>
#include <windows.h>
#include <thread>
#include <chrono>
#include <array>
#include <vector>
#include <pdh.h>
#include <pdhmsg.h>
#include <intrin.h>
#include <algorithm>
#include <TlHelp32.h>
//#pragma comment(lib, "pdh.lib")

#define thread_Update 450 // 0x1C2 (ms)
#define length_cpuBrandStr 64 // 0x40 64 bytes length

struct CPUInfo
{
	uint8_t cores = 0;
	uint8_t logicalCores = 0;
	uint16_t baseClockMHz = 0;
	uint16_t maxClockMHz = 0;
};

struct ProcessInfo
{
	DWORD pid;
	std::wstring name;
	float cpuUsage;
	float memoryUsage;
	float gpuUsage; // placerholder
	float network; // placerholder
};

class HardwareController
{
private:
	SYSTEM_INFO sysInfo;

	// cpu vars
	std::atomic<bool> running = { true };
	std::thread cpuThread;
	PDH_HQUERY query;
	PDH_HCOUNTER counter;
	PDH_FMT_COUNTERVALUE counterVal;
	std::atomic<float> cpuValue = { 0.0 };
	std::array<float, 10> cpuHistory{};
	uint8_t index = 0;

	/* 
		TODO: make a ordered map
		reduce the amount of processes that 
		are duplicated or just have multiple 
		for one app 
	*/
	// processes list
	std::thread processThread;
	HANDLE hSnap;
	PROCESSENTRY32 pe;
	std::vector<ProcessInfo> processList{};

	// ram memory
	std::thread ramThread;
	MEMORYSTATUSEX memInfo;
	std::atomic<bool> ramRunning{ true };
	std::array<float, 10> ramHistory{};
	std::atomic<float> ramValue;
	uint8_t ramIndex = 0;
	// glfw
	bool vsync;

public:
	HardwareController();
	void getCPUInfo();
	void setRAMInfo();
	MEMORYSTATUSEX getRAM() const;
	char* getCPUBrandStr();
	SYSTEM_INFO& getSysInfo();
	float getCPUValue() const;
	const std::array<float, 10>& getCPUHistory() const;
	void getProcessesInfo();
	std::vector<ProcessInfo> getProcessList();
	std::array<float, 10>& getRAMHistory();
	std::atomic<float>& getRAMValue();
};

