#include <iostream>
#include <Windows.h>
#include <iphlpapi.h>
#include <chrono>
#include <thread>
#include <array>

#pragma comment(lib, "iphlpapi.lib")

struct NetworkStats
{
	double download;
	double upload;
	ULONG64 receieved;
	ULONG64 sent;
};

class NetworkMonitor
{
private:
	std::thread				networkThread;
	std::atomic<bool>		running;
	std::chrono::steady_clock::time_point lastTime;
	std::atomic<ULONG64>	lastBytesReceieved;
	std::atomic<ULONG64>	lastBytesSent;
	std::array<float, 10>	downloadHistory;
	std::array<float, 10>	uploadHistory;

public:
	NetworkMonitor();
	~NetworkMonitor();

	void stopPolling();
	void pollNetworkSpeed();
	std::atomic<ULONG64>& getBytesReceieved();
	std::atomic<ULONG64>& getBytesSent();
	std::array<float, 10>& getDownloadHistory();
	std::array<float, 10>& getUploadHistory();
};