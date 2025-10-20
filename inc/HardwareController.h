#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <cstdint>
#include <windows.h>
#include <thread>
#include <chrono>
#include <array>
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

	// processes list
	std::thread processThread;
	HANDLE hSnap;
	PROCESSENTRY32 pe;

	// glfw
	bool vsync;

public:
	HardwareController();
	void getCPUInfo();
	char* getCPUBrandStr();
	SYSTEM_INFO& getSysInfo();
	float getCPUValue() const;
	const std::array<float, 10>& getCPUHistory() const;
	void getProcessesInfo();

};

