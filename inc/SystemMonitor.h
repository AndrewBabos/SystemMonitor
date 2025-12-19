#pragma once
#include "imgui.h"
#include "implot.h"
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

//#define thread_Update 0x1C2 // 450
//#define length_cpuBrandStr 0x40 // 64 bytes length


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