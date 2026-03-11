#include <iostream>
#include <SystemMonitor.h>

struct ProcessInfo
{
	DWORD pid;
	//std::string name;
	float cpuUsage;
	float memoryUsage;
	float gpuUsage;
	float networkUsage;
};

class ProcessesMonitor
{
private:
	HANDLE hSnap;
	PROCESSENTRY32 pe;
	std::map<std::string, std::vector<ProcessInfo>> processMap;

	std::thread processesThread;
	std::atomic<bool> running;
public:
	ProcessesMonitor();
	~ProcessesMonitor();

	void pollProcessesInfo();
	void stopPolling();
	std::map<std::string, std::vector<ProcessInfo>>& getProcessMap();
	const HANDLE& getHandle() const;
	const PROCESSENTRY32& getProcessEntry() const;
};