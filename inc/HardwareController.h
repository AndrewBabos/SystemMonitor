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
#include <map>
#include <pdh.h>
#include <pdhmsg.h>
#include <intrin.h>
#include <algorithm>
#include <TlHelp32.h>
#include <unordered_map>
#pragma comment(lib, "pdh.lib")

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
	std::string name;
	float cpuUsage;
	float memoryUsage;
	float gpuUsage;
	float networkUsage;
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

//	Mike Topp

	// processes list
	std::thread processThread;
	HANDLE hSnap;
	PROCESSENTRY32 pe;

	std::thread processesThread;
	std::map<std::string, std::vector<ProcessInfo>> processMap;

	// ram memory
	std::thread ramThread;
	MEMORYSTATUSEX memInfo;
	std::atomic<bool> ramRunning{ true }; 
	std::array<float, 10> ramHistory{};
	std::atomic<float> ramValue; // percent
	std::atomic<uint64_t> ramUsed, totalPhysRAM{0};

	uint8_t ramIndex = 0;

	// convert to wstring 
	// pretty sure i dont need this
	std::wstring wstringConvert(const char* word);

	// glfw
	bool vsync;

public:
	HardwareController();
	void getCPUInfo();
	void setRAMInfo();
	MEMORYSTATUSEX getRAM() const;
	std::string getCPUBrandStr();
	SYSTEM_INFO& getSysInfo();
	float getCPUValue() const;
	const std::array<float, 10>& getCPUHistory() const;
	void getProcessesInfo();
	//std::vector<ProcessInfo> getProcessList();
	std::map<std::string, std::vector<ProcessInfo>>& getProcessMap();
	std::array<float, 10>& getRAMHistory();
	std::atomic<float>& getRAMValue();
	std::atomic<uint64_t>& getUsedRAM();
	std::atomic<uint64_t>& getTotalPhysRAM();

};

