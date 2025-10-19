#include "../inc/SystemMonitor.h"


SystemMonitor::SystemMonitor()
{
    vsync = false;

    // list of processes
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Get the information associated with each extended ID.
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
    GetSystemInfo(&sysInfo);
    std::cout << "Processor Architecture: " << sysInfo.wProcessorArchitecture << std::endl;
    getCPUInfo();
    //getProcessesInfo();
}

void SystemMonitor::main()
{
    UiController::renderOptionsAndDockspace();
    // show system hardware information
    UiController::renderCPU(cpuValue, cpuHistory);
    UiController::renderSysInfo(CPUBrandString, sysInfo);
    //UiController::renderProcesses(hSnap, pe);
    UiController::testingTables(hSnap, pe);

    // references
    ImGui::ShowDemoWindow();

	ImGui::End();
}

void SystemMonitor::getCPUInfo()
{
    if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
        return;
    if (PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
        return;

    SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
    cpuThread = std::thread([this]()
    {
        while (running.load())
        {
            PdhCollectQueryData(query);
            PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal);
            cpuValue.store(counterVal.doubleValue);

            switch (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal))
            {
                case ERROR_SUCCESS:
                    cpuHistory[index] = static_cast<float>(counterVal.doubleValue);
                    index = (index + 1) % cpuHistory.size();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(thread_Update));
            //std::this_thread::sleep_for(std::chrono::seconds(2)); // task manager update
        }
    });
}

void SystemMonitor::getProcessesInfo()
{
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR GETTING stuff idk\n";
        return;
    }

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe)) {
        do {
            // Process info: pe.th32ProcessID, pe.szExeFile, etc.
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
}

bool SystemMonitor::setVsync()
{
    return !vsync;
}

void SystemMonitor::shutdown()
{
}

SystemMonitor::~SystemMonitor()
{
    running.store(false);
    if (cpuThread.joinable()) 
        cpuThread.join();
    PdhCloseQuery(query);
}
