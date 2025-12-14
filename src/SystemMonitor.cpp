#include "../inc/SystemMonitor.h"

// all hardware getters and setters be placed here
SystemMonitor::SystemMonitor()
{
    //hwCtrl = new HardwareController();
    hwCtrl = std::make_unique<HardwareController>();
    vsync = false;

    // list of processes
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);

    hwCtrl->getCPUInfo();
    hwCtrl->getProcessesInfo();
    hwCtrl->setRAMInfo();
}

// all UiController method calls are put here to render
void SystemMonitor::render()
{
    UiController::renderOptionsAndDockspace();


    UiController::renderCPU(        hwCtrl->getCPUValue(),
                                    hwCtrl->getCPUHistory(),
                                    hwCtrl->getIndividualCoreHistories());
    UiController::renderSysInfo(    hwCtrl->getCPUBrandStr(), 
                                    hwCtrl->getSysInfo());
    UiController::renderRAM(        hwCtrl->getRAMValue(),
                                    hwCtrl->getRAMHistory(),
                                    hwCtrl->getUsedRAM(),
                                    hwCtrl->getTotalPhysRAM());
    UiController::renderProcesses(  getHandle(), 
                                    getProcessEntry(), 
                                    hwCtrl->getProcessMap());

    // references to their docs
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();

	ImGui::End();
}

bool SystemMonitor::setVsync()
{
    return !vsync;
}

const HANDLE& SystemMonitor::getHandle() const
{
    return hSnap;
}

const PROCESSENTRY32& SystemMonitor::getProcessEntry() const
{
    return pe;
}

SystemMonitor::~SystemMonitor()
{
    CloseHandle(hSnap);
    // pretty sure trying to stop a thread makes it crash
    // its a unique pointer so no need for delete
    //delete hwCtrl; // dont know why this crashes
}