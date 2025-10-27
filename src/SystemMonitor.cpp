#include "../inc/SystemMonitor.h"

// all hardware getters and setters be placed here
SystemMonitor::SystemMonitor()
{
    hwCtrl = new HardwareController();
    //vsync = false;

    // list of processes
   /* hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);*/

    hwCtrl->getCPUInfo();
    hwCtrl->getProcessesInfo();
    hwCtrl->setRAMInfo();
}

// all UiController method calls are put here to render
void SystemMonitor::main()
{
    // these are all fine
    UiController::renderOptionsAndDockspace();

    UiController::renderCPU(hwCtrl->getCPUValue(),
                            hwCtrl->getCPUHistory());
    UiController::renderSysInfo(hwCtrl->getCPUBrandStr(), 
                                hwCtrl->getSysInfo());
    UiController::renderRAM(hwCtrl->getRAMValue(),
                            hwCtrl->getRAMHistory(),
                            hwCtrl->getUsedRAM(),
                            hwCtrl->getTotalPhysRAM());

    // these 2 are the same
    //UiController::renderProcessesTable(hSnap, pe, hwCtrl->getProcessMap()); // prcesses tabole
    UiController::renderProcessesTable(hwCtrl->getSnapshotProcesses(),
                                        hwCtrl->singleProcessStruct(),
                                        hwCtrl->getProcessMap()); // prcesses tabole

    // references to their docs
    ImGui::ShowDemoWindow();

	ImGui::End();
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
