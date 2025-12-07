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

class CpuMonitor
{
private:
	std::atomic<bool>	 running;
	std::thread			 cpuThread;
	PDH_HQUERY			 query;
	PDH_HCOUNTER		 counter;
	PDH_FMT_COUNTERVALUE counterVal;
	std::atomic<float>	 cpuValue = { 0.0 };
	std::array<float,10> cpuHistory{};
	uint8_t index = 0;

public:
	CpuMonitor();
	void pollCPUMetrics();
	void stopPolling();
	const std::string getCPUStr() const;
	const std::atomic<float>& getCPUValue() const;
	const std::array<float, 10>& getCPUMetrics() const;
	~CpuMonitor();
};