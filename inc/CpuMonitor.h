#pragma once

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
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "pdh.lib")

class CpuMonitor
{
private:
	std::atomic<bool>	 running;
	std::thread			 cpuThread;
	PDH_HQUERY			 query;
	PDH_HCOUNTER		 counter;
	std::vector<PDH_HCOUNTER> coreCounters; // cores n+1
	PDH_FMT_COUNTERVALUE counterVal;
	std::atomic<float>	 cpuValue = { 0.0 }; // total cores avg %
	std::array<float,10> cpuHistory{};
	// # of Cores | That cores history
	std::vector<std::array<float, 10>> coreHistories;  // one history per core
	uint8_t index = 0;
	LONG cpuTemp;

public:
	CpuMonitor();
	void pollCPUMetrics();
	void stopPolling();
	HRESULT SetCpuTemperature(LPLONG pTemperature);
	const std::string getCPUStr() const;
	const std::atomic<float>& getCPUValue() const;
	const std::array<float, 10>& getCPUMetrics() const;
	const std::vector<std::array<float, 10>>& getIndividualCoreHistories() const;
	~CpuMonitor();
};