#pragma once
#include "imgui.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <array>
#include <pdh.h>
#include <pdhmsg.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "pdh.lib")


class SystemMonitor
{
private:
	/*UiController uiCtrl;
	HardwareController hwCtrl;*/

	std::atomic<bool> running = { true };
	std::thread cpuThread;
	PDH_HQUERY query;
	PDH_HCOUNTER counter;
	PDH_FMT_COUNTERVALUE counterVal;
	bool vsync = false;

	// cpu vars
	std::atomic<float> cpuValue = { 0.0 };
	std::array<float, 10> cpuHistory {};
	uint8_t index = 0;

public:
	SystemMonitor();

	// render ui to screen (make this a controller eventually
	void RenderUi();
	void renderCPU();
	void renderRAM();

	// get system information (make this a controller eventually as well
	void setThreadsForInfo();
	void getCPUInfo();
	void setVsync();
	void shutdown();
	~SystemMonitor();
};