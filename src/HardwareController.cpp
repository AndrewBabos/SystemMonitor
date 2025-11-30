#include "../inc/HardwareController.h"
#include <Psapi.h>

std::wstring wstringConvert(const char* word);


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
    if (PdhAddCounter(query, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &counter) != ERROR_SUCCESS)
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

void HardwareController::getProcessesInfo()
{
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR GETTING stuff idk\n";
        return;
    }

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe))
    {
        do
        {
            ProcessInfo info{};
            info.pid = pe.th32ProcessID;
            //info.name = pe.szExeFile;   // wchar_t[] -> std::wstring
            info.name = wstringConvert(pe.szExeFile);   // wchar_t[] -> std::wstring
            info.cpuUsage = 0.0f;
            info.memoryUsage = 0.0f;
            info.gpuUsage = 0.0f;
            info.network = 0.0f;

            processList.push_back(info);
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
}

// testing
std::wstring wstringConvert(const char* word)
{
    if (!word) return L"";

    int neededSize = MultiByteToWideChar(CP_UTF8, 0, word, -1, nullptr, 0);
    if (neededSize <= 0) return L"";
    std::wstring convertedWord(neededSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, word, -1, &convertedWord[0], neededSize);
    
    // remove the null-terminator '\0'
    if (!convertedWord.empty() && convertedWord.back() == L'\0')
        convertedWord.pop_back();

    return convertedWord;
}

std::vector<ProcessInfo> HardwareController::getProcessList()
{
    return processList;
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
