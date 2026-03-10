#include "RamMonitor.h"

RamMonitor::RamMonitor()
{
    running.store(true, std::memory_order_relaxed);
}

RamMonitor::~RamMonitor()
{
    stopPolling();
}

void RamMonitor::pollRAMMetrics()
{
    ramThread = std::thread([this]()
        {
            while (running.load())
            {
                MEMORYSTATUSEX memInfo;
                memInfo.dwLength = sizeof(MEMORYSTATUSEX);
                if (GlobalMemoryStatusEx(&memInfo))
                {
                    uint64_t total = static_cast<uint64_t>(memInfo.ullTotalPhys);
                    uint64_t used = total - static_cast<uint64_t>(memInfo.ullAvailPhys);

                    totalPhysRAM.store(total, std::memory_order_relaxed);
                    ramUsed.store(used, std::memory_order_relaxed);

                    float percent = static_cast<float>((double)used / (double)total * 100.0);
                    ramValue.store(percent, std::memory_order_relaxed);

                    ramHistory[ramIndex] = percent;
                    ramIndex = (ramIndex + 1) % ramHistory.size();
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        });
    SetThreadPriority(ramThread.native_handle(), THREAD_PRIORITY_LOWEST);
}

void RamMonitor::stopPolling()
{
    if (!running.exchange(false))
        return;
    if (ramThread.joinable())
        ramThread.join();
}

MEMORYSTATUSEX RamMonitor::getRAM() const
{
	return memInfo;
}

std::array<float, 10>& RamMonitor::getRAMHistory()
{
    return ramHistory;
}

std::atomic<float>& RamMonitor::getRAMValue()
{
    return ramValue;
}

std::atomic<uint64_t>& RamMonitor::getUsedRAM()
{
    return ramUsed;
}

std::atomic<uint64_t>& RamMonitor::getTotalPhysRAM()
{
    return totalPhysRAM;
}
