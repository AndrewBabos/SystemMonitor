#include "../inc/HardwareController.h"
#include <Psapi.h>

HardwareController::HardwareController()
{
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
	vsync = false;

}

void HardwareController::getCPUInfo()
{
    //if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
    //    return;
    ////if (PdhAddCounter(query, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &counter) != ERROR_SUCCESS)
    //if (PdhAddCounterW(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
    //    return;

    ////SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
    //cpuThread = std::thread([this]()
    //{
    //    while (running.load())
    //    {
    //        PdhCollectQueryData(query);
    //        PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal);
    //        cpuValue.store(counterVal.doubleValue);
    //        if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal) == ERROR_SUCCESS)
    //        {
    //            cpuHistory[index] = static_cast<float>(counterVal.doubleValue);
    //            index = (index + 1) % cpuHistory.size();

    //            std::this_thread::sleep_for(std::chrono::milliseconds(thread_Update));
    //        }
    //    }
    //});
    //SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
    cpuMonitor.pollCPUMetrics();
}

void HardwareController::setRAMInfo()
{
    ramThread = std::thread([this]()
    {
        while (ramRunning.load())
        {
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            if (GlobalMemoryStatusEx(&memInfo))
            {
                std::uint64_t total = static_cast<std::uint64_t>(memInfo.ullTotalPhys);
                std::uint64_t used = total - static_cast<std::uint64_t>(memInfo.ullAvailPhys);

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
}

MEMORYSTATUSEX HardwareController::getRAM() const
{
    return memInfo;
}

std::string HardwareController::getCPUBrandStr() const
{
    //char CPUBrandString[length_cpuBrandStr] = "";

    //int CPUInfo[4] = {};
    //unsigned nExIds, i = 0;
    //__cpuid(CPUInfo, 0x80000000);
    //nExIds = CPUInfo[0];
    //for (i = 0x80000000; i <= nExIds; ++i)
    //{
    //    __cpuid(CPUInfo, i);
    //    // Interpret CPU brand string
    //    switch (i)
    //    {
    //        case 0x80000002: 
    //            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
    //            break;
    //        case 0x80000003: 
    //            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
    //            break;
    //        case 0x80000004: 
    //            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    //            break;
    //    }
    //}
    //return std::string(CPUBrandString);
    return cpuMonitor.getCPUStr();
}

SYSTEM_INFO& HardwareController::getSysInfo()
{
    GetSystemInfo(&sysInfo);
    return sysInfo;
}

float HardwareController::getCPUValue() const
{
    //return cpuValue;
    return cpuMonitor.getCPUValue();
}

const std::array<float, 10>& HardwareController::getCPUHistory() const
{
    //return cpuHistory;
    return cpuMonitor.getCPUMetrics();
}

void HardwareController::getProcessesInfo()
{
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR GETTING stuff idk\n";
        return;
    }

    processesThread = std::thread([this]() 
    {
            hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            pe.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hSnap, &pe))
            {
                do
                {
                    ProcessInfo info{};
                    info.pid = pe.th32ProcessID;
                    info.name = pe.szExeFile;      // const wchar_t* ? std::wstring OK
                    info.cpuUsage = 0.0f;
                    info.memoryUsage = 0.0f;
                    info.gpuUsage = 0.0f;
                    info.networkUsage = 0.0f;

                    //processList.push_back(info);
                    processMap[info.name].push_back(info);
                    //processList.push_back({ pe.th32ProcessID, pe.szExeFile, 0.0f, 0.0f, 0.0f, 0.0f });
                } while (Process32Next(hSnap, &pe));
            }
            CloseHandle(hSnap);
    });
}

std::map<std::string, std::vector<ProcessInfo>>& HardwareController::getProcessMap()
{
    return processMap;
}

std::array<float, 10>& HardwareController::getRAMHistory()
{
    return ramHistory;
}

std::atomic<float>& HardwareController::getRAMValue()
{
    return ramValue;
}

std::atomic<uint64_t>& HardwareController::getUsedRAM()
{
    return ramUsed;
}

std::atomic<uint64_t>& HardwareController::getTotalPhysRAM()
{
    return totalPhysRAM;
}

HardwareController::~HardwareController()
{
    running.store(false);
    PdhCloseQuery(query);
    //CloseHandle(hSnap);
    
    //if (cpuThread.joinable()) cpuThread.join();

    //if (processesThread.joinable()) processesThread.join();

    //if (ramThread.joinable()) ramThread.join();

    ramRunning.store(false);
    if (cpuMonitor.getCPUValue())
    {

    }

    if (hSnap != INVALID_HANDLE_VALUE)
        CloseHandle(hSnap);

    if (processThread.joinable())
        processThread.join();

    if (ramThread.joinable())
        ramThread.join();
}
