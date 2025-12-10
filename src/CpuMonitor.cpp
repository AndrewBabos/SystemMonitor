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
        std::cout << "Error line 15 CpuMonitor.cpp\n";
        running.store(false);
        return;
    }
    if (PdhAddCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
    {
        std::cout << "Error line 22 CpuMonitor.cpp\n";
        running.store(false);
        PdhCloseQuery(query);
        return;
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const DWORD coreCount = sysInfo.dwNumberOfProcessors;
    coreCounters.clear();
    coreCounters.reserve(coreCount);
    coreHistories.clear();
    coreHistories.assign(coreCount, {});  // coreCount histories

    for (DWORD i = 0; i < coreCount; ++i)
    {
        std::wstring buffer = L"\\Processor(" + std::to_wstring(i) + L")\\% Processor Time";

        PDH_HCOUNTER c{};
        if (PdhAddCounterW(query, buffer.c_str(), 0, &c) != ERROR_SUCCESS)
        {
            std::cout << "Error line 33 CpuMonitor.cpp\n";
            running.store(false);
            PdhCloseQuery(query);
            return;
        }
        coreCounters.push_back(c);
    }

    std::cout << "Number of cores added to vector: " << coreCounters.size() << '\n';

    cpuThread = std::thread([this, coreCount]()
        {
            PdhCollectQueryData(query);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            while (running.load(std::memory_order_relaxed))
            {
                if (PdhCollectQueryData(query) != ERROR_SUCCESS)
                    continue;

                PDH_FMT_COUNTERVALUE v{};

                // total
                if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &v) == ERROR_SUCCESS)
                {
                    float total = static_cast<float>(v.doubleValue);
                    cpuValue.store(total);
                    cpuHistory[index] = total;
                }

                // per core
                for (uint8_t core = 0; core < coreCount; ++core)
                {
                    if (PdhGetFormattedCounterValue(coreCounters[core], PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS)
                    {
                        float val = static_cast<float>(v.doubleValue);
                        coreHistories[core][index] = val;
                    }
                }

                index = (index + 1) % cpuHistory.size();
                std::this_thread::sleep_for(std::chrono::milliseconds(450));
            }
        });

    SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
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

const std::vector<std::array<float, 10>>& CpuMonitor::getIndividualCoreHistories() const
{
    return coreHistories;
}

CpuMonitor::~CpuMonitor()
{
    // pretty sure this stops it?
    if (cpuThread.joinable())
        cpuThread.join();
}
