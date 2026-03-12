#include <NetworkMonitor.h>


NetworkMonitor::NetworkMonitor()
{
	running.store(true);
	lastTime = std::chrono::steady_clock::now();
}

NetworkMonitor::~NetworkMonitor()
{
	stopPolling();
}

void NetworkMonitor::stopPolling()
{
	if (networkThread.joinable())
		networkThread.join();
}

void NetworkMonitor::pollNetworkSpeed()
{
	networkThread = std::thread([this]()
	{

	});
}

std::atomic<ULONG64>& NetworkMonitor::getBytesReceieved()
{
	return lastBytesReceieved;
}

std::atomic<ULONG64>& NetworkMonitor::getBytesSent()
{
	return lastBytesSent;
}

std::array<float, 10>& NetworkMonitor::getDownloadHistory()
{
	return downloadHistory;
}

std::array<float, 10>& NetworkMonitor::getUploadHistory()
{
	return uploadHistory;
}
