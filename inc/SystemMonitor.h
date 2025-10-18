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

#include <GLFW/glfw3.h>

#include "HardwareController.h"
#include "UiController.h"

#pragma comment(lib, "pdh.lib")

#define thread_Update 0x1C2 // 450
#define length_cpuBrandStr 0x40 // 64 bytes length


class SystemMonitor
{
private:
	//UiController uiCtrl;
	//HardwareController hwCtrl;

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

public:
	SystemMonitor();

	// render ui to screen (make this a controller eventually
	void RenderUi(); // name this to MainState

	void renderCPU();
	void renderRAM();
	void renderProcesses();

	// get system information (make this a controller eventually as well
	void setThreadsForInfo();
	void renderSysInfo();
	void getCPUInfo();
	bool setVsync();
	void shutdown();
	~SystemMonitor();
};