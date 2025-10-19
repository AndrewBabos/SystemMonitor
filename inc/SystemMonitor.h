#pragma once
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <array>
#include <pdh.h>
#include <pdhmsg.h>
#include <intrin.h>
#include <algorithm>
#include <TlHelp32.h>

#include <GLFW/glfw3.h>

#include "HardwareController.h"
#include "UiController.h"

#pragma comment(lib, "pdh.lib")

#define thread_Update 0x1C2 // 450
#define length_cpuBrandStr 0x40 // 64 bytes length


class SystemMonitor
{
private:
	// controllers
	//UiController uiCtrl; // not needed
	//HardwareController hwCtrl;
	//AppConfigController configCtrl; // file saving

	// All System Information
	SYSTEM_INFO sysInfo;
	char CPUBrandString[length_cpuBrandStr];

	std::atomic<bool> running = { true };
	std::thread cpuThread;
	PDH_HQUERY query;
	PDH_HCOUNTER counter;
	PDH_FMT_COUNTERVALUE counterVal;
	bool vsync;

	// cpu vars
	std::atomic<float> cpuValue = { 0.0 };
	std::array<float, 10> cpuHistory {};
	uint8_t index = 0;

	// processes
	std::thread processThread;
	HANDLE hSnap;
	PROCESSENTRY32 pe;

public:
	SystemMonitor();
	void main(); // name this to MainState
	//void setThreadsForInfo();
	void getCPUInfo();
	void getProcessesInfo();
	bool setVsync();
	void shutdown();
	~SystemMonitor();
};