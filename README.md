# System Monitor

A lightweight **Windows System Monitor** built in **C++** using **OpenGL**, **GLFW**, **ImGui**, and **ImPlot**.  
It displays real-time CPU and RAM usage, along with detailed process information gathered directly through the **Win32 API** and **PDH** performance counters.

---

## Features

- **Live CPU & RAM graphs** updated in real-time  
- **Process table** with sorting, grouping, and per-process metrics  
- **Threaded polling system** for low overhead updates  
- **Modern UI** powered by ImGui + ImPlot  
- **Bloat-free and native** — no external frameworks beyond the ImGui stack  

---

## Architecture Overview

| Component | Responsibility |
|------------|----------------|
| `HardwareController` Class | Polls CPU, RAM, and system data through PDH and Win32 API |
| `UiController` Namespace | Renders live ImGui + ImPlot visualizations |
| `ProcessManager` | Enumerates and groups processes via `CreateToolhelp32Snapshot` |
| Threads | Handle data sampling every 1 second to avoid blocking the render loop |

---

## Dependencies

| Library | Purpose |
|----------|----------|
| **ImGui** | UI rendering |
| **ImPlot** | Graph plotting |
| **GLFW / OpenGL** | Windowing and graphics backend |
| **PDH** | CPU performance data |
| **psapi.h / tlhelp32.h** | Process and memory enumeration |

### Link against:
```
opengl32.lib
pdh.lib
psapi.lib
```

---

## Building

1. Clone the repository:
   ```bash
   git clone https://github.com/AndrewBabos/SystemMonitor.git
   cd SystemMonitor
   ```
2. Add ImGui and ImPlot source files to your project.
3. Include headers:
   ```cpp
   #include <windows.h>
   #include <pdh.h>
   #include <psapi.h>
   #include <tlhelp32.h>
   ```
4. Build with a C++20-capable compiler (MSVC recommended).

---

## Usage

Run the executable — the monitor opens separate panels for CPU, RAM, and processes:

```
CPU Usage: 42.7%
[Real-time line graph]

RAM Usage: 63.1%
[Real-time line graph]

Processes
Process ID(PID) | Process Name
  -----------------------------
   ├─ 1524      | chrome.exe
   ├─ 3128      | discord.exe
   └─ 4980      | explorer.exe
```

Each process entry can later include CPU and memory statistics or be expanded into grouped trees for duplicate process names.

---

## Example Code Snippet

```cpp
// Example: start polling thread
HardwareController hw;
hw.start();

// Render CPU + RAM
UiController::renderCPU(hwCtrl->getCPUValue(), hwCtrl->getCPUHistory());
UiController::renderRAM(hwCtrl->getRAMValue(), hwCtrl->getRAMHistory());
```

---

## Planned Enhancements

- Complete Processes List window, with organization based on CPU low-high or vice versa with all columns
- GPU utilization metrics (via NVML or DXGI)  
- Disk I/O visualization  
- Per-process CPU/RAM charts  
- Configurable refresh interval
- Saved settings config file

---

## License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.
