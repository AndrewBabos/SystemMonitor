#include <ProcessesMonitor.h>

ProcessesMonitor::ProcessesMonitor()
{
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
    running.store(true, std::memory_order_relaxed);
}

ProcessesMonitor::~ProcessesMonitor()
{
    stopPolling();
}

void ProcessesMonitor::pollProcessesInfo()
{
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        std::cout << "ERROR GETTING stuff idk\n";
        return;
    }
    processesThread = std::thread([this]()
        {
            //while (running.load())
            //{
            hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            pe.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hSnap, &pe))
            {
                do
                {
                    ProcessInfo info{};
                    info.pid = pe.th32ProcessID;
                    info.cpuUsage = 0.0f;
                    info.memoryUsage = 0.0f;
                    info.gpuUsage = 0.0f;
                    info.networkUsage = 0.0f;

                    //processMap[info.name].push_back(info);
                    processMap[pe.szExeFile].push_back(info);
                } while (Process32Next(hSnap, &pe));
            }
            CloseHandle(hSnap);
            //}
        });
}

void ProcessesMonitor::stopPolling()
{
    if (!running.exchange(false))
        return;

    if (hSnap != INVALID_HANDLE_VALUE)
        CloseHandle(hSnap);

    if (processesThread.joinable())
        processesThread.join();
}

std::map<std::string, std::vector<ProcessInfo>>& ProcessesMonitor::getProcessMap()
{
	return processMap;
}

const HANDLE& ProcessesMonitor::getHandle() const
{
	return hSnap;
}

const PROCESSENTRY32& ProcessesMonitor::getProcessEntry() const
{
	return pe;
}
