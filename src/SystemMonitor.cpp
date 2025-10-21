#include "../inc/SystemMonitor.h"

SystemMonitor::SystemMonitor()
{
    hwCtrl = new HardwareController();
    vsync = false;

    // list of processes
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);

    hwCtrl->getCPUInfo();
    hwCtrl->getProcessesInfo();
    hwCtrl->setRAMInfo();

    //getProcessesInfo();
    //UiController::fillProcessList(hSnap, pe);
    //fillProcessList(hSnap, pe);
}

void SystemMonitor::main()
{
    UiController::renderOptionsAndDockspace();
    UiController::renderCPU(hwCtrl->getCPUValue(), hwCtrl->getCPUHistory());
    UiController::renderSysInfo(hwCtrl->getCPUBrandStr(), hwCtrl->getSysInfo());
    UiController::renderRAM(hwCtrl->getRAMValue(), hwCtrl->getRAMHistory());
    // these 2 are the same
    //UiController::renderProcesses(hSnap, pe);
    //fillProcessList(hSnap, pe);
    UiController::testingTables(hSnap, pe, hwCtrl->getProcessList()); // prcesses tabole

    // references
    ImGui::ShowDemoWindow();
	ImGui::End();
}

// move this to hwctrl
void SystemMonitor::getProcessesInfo()
{
    //if (hSnap == INVALID_HANDLE_VALUE)
    //{
    //    std::cout << "ERROR GETTING stuff idk\n";
    //    return;
    //}

    //hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    //pe.dwSize = sizeof(PROCESSENTRY32);
    //if (Process32First(hSnap, &pe)) 
    //{
    //    do 
    //    {
    //        // Process info: pe.th32ProcessID, pe.szExeFile, etc.

    //    } while (Process32Next(hSnap, &pe));
    //}
    //CloseHandle(hSnap);
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
    //delete hwCtrl; // dont know why this crashes, its literally freeing
    /*running.store(false);
    if (cpuThread.joinable()) 
        cpuThread.join();*/
    //PdhCloseQuery(query);
}
