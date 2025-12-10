#include "CpuMonitor.h"

CpuMonitor::CpuMonitor()
{
	running.store(true);
    query = {};
    counter = {};
    counterVal = {};
}

void CpuMonitor::pollCPUMetrics()
{
    if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
    {
        running.store(false);
        return;
    }
    if (PdhAddCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
    {
        running.store(false);
        PdhCloseQuery(query);
        return;
    }
    cpuThread = std::thread([this]()
    {
        while (running.load())
        {
            PdhCollectQueryData(query);
            PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal);
            cpuValue.store(counterVal.doubleValue);
            if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal) == ERROR_SUCCESS)
            {
                cpuHistory[index] = static_cast<float>(counterVal.doubleValue);
                index = (index + 1) % cpuHistory.size();

                std::this_thread::sleep_for(std::chrono::milliseconds(450));
            }
        }
    });
    SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
}

void CpuMonitor::pollIndividualCoreMetrics()
{

}

void CpuMonitor::stopPolling()
{
    if (!running.exchange(false))
        return;

    if (cpuThread.joinable())
        cpuThread.join();

    if (query != nullptr)
    {
        PdhCloseQuery(query);
        query = nullptr;
        counter = nullptr;
    }
}

const std::string CpuMonitor::getCPUStr() const
{
    char CPUBrandString[64] = "";

    int CPUInfo[4] = {};
    unsigned nExIds, i = 0;
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        switch (i)
        {
        case 0x80000002:
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            break;
        case 0x80000003:
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            break;
        case 0x80000004:
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
            break;
        }
    }
    return std::string(CPUBrandString);
}

const std::atomic<float>& CpuMonitor::getCPUValue() const
{
    return cpuValue;
}

const std::array<float, 10>& CpuMonitor::getCPUMetrics() const
{
	return cpuHistory;
}

CpuMonitor::~CpuMonitor()
{
    // pretty sure this stops it?
    if (cpuThread.joinable())
        cpuThread.join();
}
