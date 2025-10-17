#pragma once
#include "imgui.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <vector>
#include <pdh.h>
#include <pdhmsg.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "pdh.lib")


class SystemMonitor
{
private:
	std::atomic<float> cpuValue = { 0.0 };
	std::atomic<bool> running = { true };
	std::thread cpuThread;
	PDH_HQUERY query;
	PDH_HCOUNTER counter;
	PDH_FMT_COUNTERVALUE counterVal;
	bool vsync = false;
	//static std::vector<double> cpu = {};

public:
	SystemMonitor();

	// render ui to screen (make this a controller eventually
	void RenderUi();
	void renderCPU();
	void renderRAM();

	// get system information (make this a controller eventually as well
	void setThreadsForInfo();
	void setVsync();
	void shutdown();
	~SystemMonitor();
};