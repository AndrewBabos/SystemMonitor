#include "../inc/SystemMonitor.h"

// all hardware getters and setters be placed here
SystemMonitor::SystemMonitor()
{
    //hwCtrl = new HardwareController();
    hwCtrl = std::make_unique<HardwareController>();
    vsync = false;

    // list of processes
    /*hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);*/

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
    UiController::renderRAM(        hwCtrl->getRAMValue(),
                                    hwCtrl->getRAMHistory(),
                                    hwCtrl->getUsedRAM(),
                                    hwCtrl->getTotalPhysRAM());
    UiController::renderGPU(                                ); // put stuff ehre
    UiController::renderNetwork();
    UiController::renderSysInfo(    hwCtrl->getCPUBrandStr(), 
                                    hwCtrl->getSysInfo());
    UiController::renderProcesses(  hwCtrl->getHandle(),
                                    hwCtrl->getProcessEntry(),
                                    hwCtrl->getProcessMap());


    // references to their docs
    ImGui::ShowDemoWindow();
    //ImPlot::ShowDemoWindow();

	ImGui::End();
}

bool SystemMonitor::setVsync() const
{
    return !vsync;
}

SystemMonitor::~SystemMonitor()
{
}