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
#include <RamMonitor.h>
#include <ProcessesMonitor.h>

#define thread_Update 450 // 0x1C2 (ms)
#define length_cpuBrandStr 64 // 0x40 64 bytes length

/* TODO:
	*		throw this into processes monitor
	*/
struct ProcessInfo
{
	DWORD pid;
	//std::string name;
	float cpuUsage;
	float memoryUsage;
	float gpuUsage;
	float networkUsage;
};

// probably dont need this, class into class into class just makes it
// redundant. SystemMonitor Class seems like HwCtrl anyway
class HardwareController
{
private:
	SYSTEM_INFO sysInfo;
	CpuMonitor cpuMonitor;
	RamMonitor ramMonitor;
	ProcessesMonitor processesMonitor;

//	Mike Topp

	// processes list
	/* TODO:
	*		throw this into its own monitor
	*/
	HANDLE hSnap;
	PROCESSENTRY32 pe;
	std::thread processesThread;
	std::map<std::string, std::vector<ProcessInfo>> processMap;

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
	const HANDLE& getHandle() const;
	const PROCESSENTRY32& getProcessEntry() const;
	~HardwareController();
};

