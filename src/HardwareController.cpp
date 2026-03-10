#include "../inc/HardwareController.h"
#include <Psapi.h>

HardwareController::HardwareController()
{
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
	vsync = false;
}

void HardwareController::getCPUInfo()
{// probably remove all methods and just start up the class monitors
    cpuMonitor.pollCPUMetrics(); 
}

void HardwareController::setRAMInfo()
{
    ramMonitor.pollRAMMetrics();
}

MEMORYSTATUSEX HardwareController::getRAM() const
{
    return ramMonitor.getRAM();
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
    return ramMonitor.getRAMHistory();
}

std::atomic<float>& HardwareController::getRAMValue()
{
    return ramMonitor.getRAMValue();
}

std::atomic<uint64_t>& HardwareController::getUsedRAM()
{
    return ramMonitor.getUsedRAM();
}

std::atomic<uint64_t>& HardwareController::getTotalPhysRAM()
{
    return ramMonitor.getTotalPhysRAM();
}

const HANDLE& HardwareController::getHandle() const
{
    return hSnap;
}

const PROCESSENTRY32& HardwareController::getProcessEntry() const
{
    return pe;
}

HardwareController::~HardwareController()
{
    printf("shutting down CpuMonitor\n");
    cpuMonitor.stopPolling();
    printf("CpuMonitor shut down...\n");
    printf("shutting down RamMonitor\n");
    ramMonitor.stopPolling();
    printf("RamMonitor shut down...\n");

    if (hSnap != INVALID_HANDLE_VALUE)
        CloseHandle(hSnap);

    if (processesThread.joinable())
        processesThread.join();
}
