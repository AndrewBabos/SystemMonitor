#include "RamMonitor.h"

void RamMonitor::setRAMInfo()
{
}

MEMORYSTATUSEX RamMonitor::getRAM() const
{
	return MEMORYSTATUSEX();
}

//std::array<float, 10>& RamMonitor::getRAMHistory()
//{
//	// TODO: insert return statement here
//}
//
//std::atomic<float>& RamMonitor::getRAMValue()
//{
//	// TODO: insert return statement here
//}
//
//std::atomic<uint64_t>& RamMonitor::getUsedRAM()
//{
//	// TODO: insert return statement here
//}
//
//std::atomic<uint64_t>& RamMonitor::getTotalPhysRAM()
//{
//	// TODO: insert return statement here
//}
