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

#include "CpuMonitor.h"

#define thread_Update 450 // 0x1C2 (ms)
#define length_cpuBrandStr 64 // 0x40 64 bytes length

struct ProcessInfo
{
	DWORD pid;
	//std::string name;
	float cpuUsage;
	float memoryUsage;
	float gpuUsage;
	float networkUsage;
};

class HardwareController
{
private:
	SYSTEM_INFO sysInfo;
	CpuMonitor cpuMonitor;

	// cpu vars
	//std::atomic<bool> running = { true };
	//std::thread cpuThread;
	//PDH_HQUERY query;
	//PDH_HCOUNTER counter;
	//PDH_FMT_COUNTERVALUE counterVal;
	//std::atomic<float> cpuValue = { 0.0 };
	//std::array<float, 10> cpuHistory{};
	//uint8_t index = 0;

//	Mike Topp

	// processes list
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

	// glfw
	bool vsync;

public:
	HardwareController();


	// CpuMonitor class
	void getCPUInfo();
	std::string getCPUBrandStr() const;
	float getCPUValue() const;
	const std::array<float, 10>& getCPUHistory() const;
	const std::vector<std::array<float, 10>>& getIndividualCoreHistories() const;


	// RamMonitor class
	void setRAMInfo();
	MEMORYSTATUSEX getRAM() const;
	std::array<float, 10>& getRAMHistory();
	std::atomic<float>& getRAMValue();
	std::atomic<uint64_t>& getUsedRAM();
	std::atomic<uint64_t>& getTotalPhysRAM();

	// ProcessesMonitor class
	SYSTEM_INFO& getSysInfo();
	void getProcessesInfo();
	std::map<std::string, std::vector<ProcessInfo>>& getProcessMap();
	
	~HardwareController();
};

