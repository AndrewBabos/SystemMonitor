#pragma once
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <array>
#include <pdh.h>
#include <pdhmsg.h>
#include <intrin.h>
#include <algorithm>
#include <TlHelp32.h>

#include <GLFW/glfw3.h>

#include "HardwareController.h"
#include "UiController.h"

#pragma comment(lib, "pdh.lib")

class SystemMonitor
{
private:
	std::unique_ptr<HardwareController> hwCtrl;
	bool vsync;

public:
	SystemMonitor();
	void render();
	bool setVsync() const;
	~SystemMonitor();
};