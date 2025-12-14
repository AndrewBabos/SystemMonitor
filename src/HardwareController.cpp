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

const std::vector<std::array<float, 10>>& HardwareController::getIndividualCoreHistories() const
{
    return cpuMonitor.getIndividualCoreHistories();
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
                    info.cpuUsage = 0.0f;
                    info.memoryUsage = 0.0f;
                    info.gpuUsage = 0.0f;
                    info.networkUsage = 0.0f;

                    //processMap[info.name].push_back(info);
                    processMap[pe.szExeFile].push_back(info);
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
    cpuMonitor.stopPolling();

    ramRunning.store(false);
    /*if (cpuMonitor.getCPUValue())
    {

    }*/

    if (hSnap != INVALID_HANDLE_VALUE)
        CloseHandle(hSnap);

    if (processesThread.joinable())
        processesThread.join();

    if (ramThread.joinable())
        ramThread.join();
}
