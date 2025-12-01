#include "../inc/SystemMonitor.h"

// all hardware getters and setters be placed here
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
}

// all UiController method calls are put here to render
void SystemMonitor::main()
{
    UiController::renderOptionsAndDockspace();
    UiController::renderCPU(hwCtrl->getCPUValue(),
                            hwCtrl->getCPUHistory());
    UiController::renderSysInfo(hwCtrl->getCPUBrandStr(), 
                                hwCtrl->getSysInfo());
    UiController::renderRAM(hwCtrl->getRAMValue(),
                            hwCtrl->getRAMHistory(),
                            hwCtrl->getUsedRAM(),
                            hwCtrl->getTotalPhysRAM());

    //UiController::testingTables(hSnap, pe, hwCtrl->getProcessList()); // prcesses tabole with vector
    UiController::testingTables(hSnap, pe, hwCtrl->getProcessMap()); // prcesses tabole with map

    // references to their docs
    //ImGui::ShowDemoWindow();

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
