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
    if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
        return;
    if (PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
        return;

    //SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
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

                std::this_thread::sleep_for(std::chrono::milliseconds(thread_Update));
                //std::this_thread::sleep_for(std::chrono::seconds(2)); // task manager updates at this interval
            }
        }
    });
    SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
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
                DWORDLONG total = memInfo.ullTotalPhys;
                DWORDLONG used = total - memInfo.ullAvailPhys;
                ramUsed.store(static_cast<int>(used));
                //totalPhysRAM.store(static_cast<int>((double)used / (double)total)); // might need
                ramValue.store(static_cast<float>((double)used / (double)total * 100.0));
                ramHistory[ramIndex] = static_cast<float>((double)used / (double)total * 100.0);
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

char* HardwareController::getCPUBrandStr()
{
    char CPUBrandString[length_cpuBrandStr];
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
            case (0x80000002):
                memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            case (0x80000003):
                memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            case (0x80000004):
                memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }
    return CPUBrandString;
}

SYSTEM_INFO& HardwareController::getSysInfo()
{
    GetSystemInfo(&sysInfo);
    return sysInfo;
}

float HardwareController::getCPUValue() const
{
    return cpuValue;
}

const std::array<float, 10>& HardwareController::getCPUHistory() const
{
    return cpuHistory;
}


/*
*   TODO:
*       fill out the rest of the struct for the process, still need CPU, RAM, NETWORK, AND GPU metrics
*       ALSO, make an alg that checks the map if a process is new, 
*       if its not create a new entry in vector, 
*       if it is add to the vector so a tree builds up
*/
void HardwareController::getProcessesInfo()
{
    // throw this into a thread to watch for any new processes added (eventuallpy)
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR GETTING stuff idk\n";
        return;
    }

    PDH_HQUERY processQueryStr{};
    PDH_HCOUNTER processCounter{};
    PDH_FMT_COUNTERVALUE cpuVal{};

    //PROCESS_MEMORY_COUNTERS_EX processMem;
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe))
    {
        do
        {
            // map stuff
            ProcessInfo info{};
            std::wstring widename = pe.szExeFile;
            std::string name(widename.begin(), widename.end());
            std::string path = "\\Processor(" + name + ")\\% Processor Time";
            PROCESS_MEMORY_COUNTERS_EX processMem{};
            GetProcessMemoryInfo(hSnap, (PROCESS_MEMORY_COUNTERS*)&processMem, sizeof(processMem));
            info.pid = pe.th32ProcessID;
            info.name = name;
            info.memoryUsage = processMem.WorkingSetSize / (1024.0 * 1024.0);
            std::cout << info.memoryUsage << std::endl;
            /*PdhOpenQuery(nullptr, 0, &processQueryStr);
            PdhAddCounterA(processQueryStr, path.c_str(), 0, &processCounter);
            PdhCollectQueryData(processQueryStr);
            PdhCloseQuery(query);

            info.cpuUsage = cpuVal.doubleValue / std::thread::hardware_concurrency();
            */
            processMap[info.name].push_back(info);

        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    //PdhCloseQuery(processQueryStr);

    // print debugging
    // take the name and how many processes (vector) out of the map iterator and use that
    for (auto iterator = processMap.cbegin(); iterator != processMap.cend(); ++iterator)
    {
        const std::string& processName = iterator->first;
        const std::vector<ProcessInfo>& processList = iterator->second;

        // this displays the imaginary tree lol,
        // if i have 3 chrome.exe, itll display here, if theres 1, still displays :p
        for (const ProcessInfo& process : processList)
        {
            printf("PID: %lu ; Process name: %s\n", process.pid, processName.c_str());
            //std::cout << processList.size() << std::endl;
        }
    }
}

std::unordered_map<std::string, std::vector<ProcessInfo>>& HardwareController::getProcessMap()
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
    // return ramValue.load();
}

std::atomic<int>& HardwareController::getUsedRAM()
{
    return ramUsed;
}

std::atomic<int>& HardwareController::getTotalPhysRAM()
{
    return totalPhysRAM;
}

HANDLE& HardwareController::getSnapshotProcesses()
{
    return hSnap;
}

PROCESSENTRY32& HardwareController::singleProcessStruct()
{
    return pe;
}
