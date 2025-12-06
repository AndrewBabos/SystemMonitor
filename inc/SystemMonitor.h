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
	// for some reason this solves the crashing when getting cpu usage info
	//HardwareController hwCtrl; this doesnt work :)
	//HardwareController* hwCtrl; // this works ?? :)
	std::unique_ptr<HardwareController> hwCtrl;

	// idk why i put this here maybe itll come back to me
	//AppConfigController configCtrl; // file saving

	bool vsync;

	/*
		TODO:
			Throw these into the hardware controller :3
	*/
	// processes
	HANDLE hSnap;
	PROCESSENTRY32 pe;

public:
	SystemMonitor();
	void render();
	bool setVsync();
	const HANDLE& getHandle() const;
	const PROCESSENTRY32& getProcessEntry() const;
	~SystemMonitor();
};