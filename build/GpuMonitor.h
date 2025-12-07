#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <array>
#include <vector>

// get backends for NVIDIA and AMD gpus (maybe intel?);


struct GpuMetrics
{
	std::string name;
};

class GpuMonitor
{
private:

public:
	GpuMonitor();

	~GpuMonitor();
};

