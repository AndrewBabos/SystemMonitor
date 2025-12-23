#include "imgui.h"
#include "implot.h"
#include <windows.h>
#include <thread>
#include <chrono>
#include <array>
#include <vector>
#include <map>

class RamMonitor
{
private:
	std::thread ramThread;
	MEMORYSTATUSEX memInfo;
	std::atomic<bool> ramRunning{ true };
	std::array<float, 10> ramHistory{};
	std::atomic<float> ramValue; // percent
	std::atomic<uint64_t> ramUsed, totalPhysRAM{ 0 };
	uint8_t ramIndex = 0;

public:
	void setRAMInfo();
	MEMORYSTATUSEX getRAM() const;
	std::array<float, 10>& getRAMHistory();
	std::atomic<float>& getRAMValue();
	std::atomic<uint64_t>& getUsedRAM();
	std::atomic<uint64_t>& getTotalPhysRAM();
};