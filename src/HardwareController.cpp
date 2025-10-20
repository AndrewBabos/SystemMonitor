#include "../inc/HardwareController.h"


HardwareController::HardwareController()
{
	vsync = false;
    GetSystemInfo(&sysInfo);
    getCPUBrandStr();
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
}


void HardwareController::getCPUInfo()
{

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

void HardwareController::getProcessesInfo()
{

}