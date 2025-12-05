#include "CpuMonitor.h"

CpuMonitor::CpuMonitor()
{
}

void CpuMonitor::pollCPUMetrics()
{
}

const std::string CpuMonitor::getCPUStr() const
{
	return std::string();
}

const std::array<float, 10>& CpuMonitor::getCPUMetrics() const
{
	return cpuHistory;
}

CpuMonitor::~CpuMonitor()
{
}
