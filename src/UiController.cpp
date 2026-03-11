#include "../inc/UiController.h"
#include <Psapi.h>
#include "../inc/HardwareController.h"
#include <numeric>
#pragma comment(lib, "psapi.lib")


void UiController::renderOptionsAndDockspace()
{
    ImGuiIO& io = ImGui::GetIO();
    static float fontScale = io.FontGlobalScale;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Dockspace", nullptr, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();
    if (opt_fullscreen)
        ImGui::PopStyleVar(2);


    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    // Show demo options and help
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::TreeNode("Options"))
            {
                // font not done yet
                if (ImGui::TreeNode("Font"))
                {
                    if (ImGui::Button("Up"));// fontScale += 0.1f;
                    ImGui::SameLine();
                    if (ImGui::Button("Down")); //fontScale -= 0.1f;
                        io.FontGlobalScale = fontScale;
                    ImGui::Text("<Not implemented yet>");
                    ImGui::Text("Font Scale: %.1f", fontScale);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Window Size"))
                {
                    ImGui::Text("asd");
                    ImGui::SameLine();
                    ImGui::Button("Set");
                    ImGui::TreePop();
                }
                ImGui::Button("breaks");
                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                if (ImGui::MenuItem("Set Vsync ON/OFF", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
                {
                    dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                    /*setVsync();
                    if (vsync)
                    {
                        std::cout << "vsync on";
                        glfwSwapInterval(1);
                    }
                    else
                        std::cout << "vsync off";*/
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            //ImGui::Separator();
            ImGui::Text("SystemMonitor");
            ImGui::Separator();
            ImGui::TextUnformatted(
                "Created by: Andrew Babos\n\n"
                "A fully customizable and bloat-free system monitor for OS and hardware information." "\n"
                "Created as a project with C++ using OpenGL, GLFW3, GLAD and Dear ImGui, along with\n"
                "help from ImPlot : )\n");
            ImGui::Separator();
            ImGui::Text("My Socials :)");
            ImGui::TextLinkOpenURL("My website : andrewbabos.ca", "https://andrewbabos.ca");
            ImGui::TextLinkOpenURL("Linkedin : www.linkedin.com/in/andrew-babos", "https://www.linkedin.com/in/andrew-babos");
            ImGui::TextLinkOpenURL("Github : github.com/AndrewBabos", "https://github.com/AndrewBabos");
            /*ImGui::TextUnformatted("When docking is enabled, you can ALWAYS dock MOST windows into another!" "\n"
                "- Drag from window title bar or their tab to dock/undock." "\n"
                "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
                "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
                "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)");*/
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void UiController::renderSysInfo(std::string CPUBrandString, SYSTEM_INFO& sysInfo)
{
    ImGui::Begin("System Information");
    ImGui::Text("Processor:          %s", CPUBrandString.c_str());
    //ImGui::Text("CPU Usage: %.2f%%", cpuValue.load());
    ImGui::Text("CPU Clock Frequency: <work in progress>");
    ImGui::Text("Number of Cores:   %d", sysInfo.dwNumberOfProcessors);
    if (sysInfo.wProcessorArchitecture == 9)
        ImGui::Text("Processor Architecture:   x86-64", sysInfo.wProcessorArchitecture);
    ImGui::Separator();
    ImGui::End();
}



void UiController::renderCPU(const std::atomic<float>& cpuValue, // total core usage
                             const std::array<float, 10>& cpuHistory, // total core usage over 10 checks
                             const std::vector<std::array<float, 10>>& coreHistories) // individual cores)
{
    ImGui::Begin("CPU");
    uint8_t numCores = coreHistories.size();
    std::string percentBuffer = std::to_string(cpuValue.load()) + "%";
    color2 =    cpuValue.load() < 50 ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) :  // GREEN
                cpuValue.load() < 80 ? ImVec4(0.9f, 0.9f, 0.2f, 1.0f) :  // YELLOW
                                       ImVec4(0.9f, 0.2f, 0.2f, 1.0f);  // RED

    ImGui::Text("Total CPU Usage"); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color2);
    ImGui::ProgressBar(cpuValue.load() / 100.0f, ImVec2(200.0f, 20.0f), percentBuffer.c_str());
    //ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PopStyleColor();

    ////////////////////
    ImGui::Separator();
    ////////////////////

    // TODO:
    // figure out why the grid is updating so slow
    ImGui::BeginChild("Grid-View");
    ImGui::Text("Individual Cores");

    for (size_t i = 0; i < numCores; i++)
    {
        ImGui::PushID(i);
        ImGui::BeginGroup();
        ImGui::Text("Core %2d", i);
        ImGui::SameLine();

        // depending on USAGE, the color cycles
        coreUsage = coreHistories[i][0];
        averageCoreUsage = std::accumulate(coreHistories[i].begin(), coreHistories[i].end(), 0.0f) / 10.0f;
        
        color = averageCoreUsage < 50 ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) :  // GREEN
                averageCoreUsage < 80 ? ImVec4(0.9f, 0.9f, 0.2f, 1.0f) :  // YELLOW
                                        ImVec4(0.9f, 0.2f, 0.2f, 1.0f);  // RED

        ImGui::PushStyleColor(ImGuiCol_PlotLines, color);
        ImGui::PlotLines("##history",
            coreHistories[i].data(),
            10,           // values count
            0,            // offset
            nullptr,      // overlay text
            0.0f,         // scale min
            100.0f,       // scale max
            ImVec2(100.0f, 20.0f));
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
        ImGui::ProgressBar(averageCoreUsage / 100.0f, ImVec2(100.0f, 20.0f), "");
        ImGui::PopStyleColor();
        //ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
        ////ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        //ImGui::ProgressBar(coreUsage / 100.0f, ImVec2(200.0f, 20.0f), "");
        //ImGui::PopStyleColor();

        //// Percentage text
        //{
        //    ImGui::SameLine();
        //    ImGui::Text("%5.1f%%", coreUsage);
            ImGui::EndGroup();
            ImGui::PopID();
        //}

        // Create 2-column layout
        if ((i + 1) % columns != 0 && i < numCores - 1)
        {
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void UiController::renderRAM(const std::atomic<float>& ramValue,
                             const std::array<float, 10>& ramHistory,
                             const std::atomic<uint64_t>& ramUsed,
                             const std::atomic<uint64_t>& totalPhysRAM)
{
    ImGui::Begin("RAM");
    //double installedRAM = (double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024);
    //double usedRAM = (double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024);
    //double percentUsed = ((double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024)) / ((double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024)) * 100;
    if (ImPlot::BeginPlot("RAM Usage", ImVec2(300, 300)))
    {
        ImPlot::SetupAxes("Intervals (Every 3s)", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, ramHistory.size(), ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        ImPlot::PlotLine("Used", ramHistory.data(), ramHistory.size());
        ImPlot::EndPlot();
    }
    ImGui::Separator();
    //ImGui::Text("%.0fMB installed", installedRAM);
    //ImGui::Text("Used:      %.0fMB / %.0fMB (%.0f%%)", usedRAM, installedRAM, percentUsed);
    //ImGui::Text("Available: %.0fMB", ((double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024) - (double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024)));
    //ImGui::Text("Cached ( **FIX ME** ):    %.0fMB");

    ImGui::Text("Used:      %.0fMB / %.0fMB (%.0f%%)", (double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024), (double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024), ((double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024)) / ((double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024)) * 100);
    ImGui::Text("Available: %.0fMB", ((double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024) - (double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024)));
    ImGui::Text("Cached ( **FIX ME** ):    %.0fMB");
    ImGui::End();
}

// maybe put these as a pie chart 
// data: 
//  USED | AVAILABLE | Temperature | latest driver version | DirectX version
void UiController::renderGPU()
{
    ImGui::Begin("GPU");
    if (ImPlot::BeginPlot("GPU Usage"))
    {
        ImPlot::SetupAxes("Intervals (Every 3s)", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 10, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        //ImPlot::PlotLine("Used", ramHistory.data(), ramHistory.size());
        ImPlot::EndPlot();
    }
    ImGui::Separator();
    ImGui::Text("VRAM Allocated");
    ImGui::Text("<put stuff here>");
    ImGui::Text("<put stuff here>");
    
    ImGui::End();
}

void UiController::renderNetwork()
{
    ImGui::Begin("Network");
    if (ImPlot::BeginPlot("Network Usage"))
    {
        ImPlot::SetupAxes("Intervals (Every 3s)", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, 10, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        //ImPlot::PlotLine("Used", ramHistory.data(), ramHistory.size());
        ImPlot::EndPlot();
    }
    ImGui::End();
}

void UiController::renderProcesses(const HANDLE& hSnap, const PROCESSENTRY32& pe, const std::map<std::string, std::vector<ProcessInfo>> processMap)
{
    ImGui::Begin("Processes");
    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollY;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    if (ImGui::BeginTable("ProcessesTable", 6, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f))
    {
        ImGui::TableSetupColumn("Process ID", ImGuiTableColumnFlags_DefaultSort, 0.0f, 0); // <-- ColumnUserID = 0
        ImGui::TableSetupColumn("Process Name");
        ImGui::TableSetupColumn("CPU");
        ImGui::TableSetupColumn("Memory");
        ImGui::TableSetupColumn("GPU");
        ImGui::TableSetupColumn("Network");
        ImGui::TableSetupScrollFreeze(0, 1); // set always visible
        ImGui::TableHeadersRow();

        // temps
        uint8_t id = 0;

        for (auto iterator = processMap.begin(); iterator != processMap.end(); ++iterator)
        {
            const std::string& processName = iterator->first; // might not need this
            const std::vector<ProcessInfo>& processList = iterator->second;

            // if theres more than 1 process with the same name (*.exe)
            if (processList.size() > 1)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(1);
                if (ImGui::TreeNodeEx(processName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth))
                {
                    // take all the processes average CPU, RAM, GPU and NETWORK usage
                    // and average it out
                    for (const ProcessInfo& process : processList)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::PushID(id);
                        char pidStr[16];
                        _ultoa_s(process.pid, pidStr, sizeof(pidStr), 10);
                        if (ImGui::Selectable(pidStr, selectedIndex == id))
                        {
                            selectedIndex = id;
                            isSelected = !isSelected;
                            //_ultoa_s(process.pid, pidTempStr, sizeof(pidTempStr), 10);
                            pidTemp = process.pid;
                            nameTemp = processName;
                        }
                        //ImGui::Text(pidStr);

                        ImGui::TableSetColumnIndex(1);
                        if (ImGui::Selectable(processName.c_str(), selectedIndex == id))
                        {
                            selectedIndex = id;
                            isSelected = !isSelected;
                            //_ultoa_s(process.pid, pidTempStr, sizeof(pidTempStr), 10);
                            pidTemp = process.pid;
                            nameTemp = processName;
                        }
                        //ImGui::Text("     %s", processName.c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", process.cpuUsage);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%zu", process.memoryUsage);

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%d", process.gpuUsage);

                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%d", process.networkUsage);

                        //ImGui::PushID(id);
                        id++;
                    }
                    for (const ProcessInfo& process : processList)
                        ImGui::PopID();

                    ImGui::TreePop();
                }
            }
            else
            {
                ImGui::PushID(id);
                const ProcessInfo& process = processList.front();
                char pidStr[12];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                _ultoa_s(process.pid, pidStr, sizeof(pidStr), 10);
                //ImGui::Text(pidStr);

                if (ImGui::Selectable(pidStr, selectedIndex == id))
                {
                    selectedIndex = id;
                    isSelected = !isSelected;
                    pidTemp = process.pid;
                    nameTemp = processName;
                }

                ImGui::TableSetColumnIndex(1);
                //ImGui::Text("   %s", processName.c_str());
                if (ImGui::Selectable(processName.c_str(), selectedIndex == id))
                {
                    selectedIndex = id;
                    isSelected = !isSelected;
                    pidTemp = process.pid;
                    nameTemp = processName;
                }

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", process.cpuUsage);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%zu", process.memoryUsage);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%zu", process.gpuUsage);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%zu", process.networkUsage);

                id++;
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
        if (ImGui::Button("Properties"))
            UiController::openPropertiesTab();
        ImGui::SameLine();
        if (ImGui::Button("End Task"))
            UiController::endProcessTask();
        
    }
}


//void UiController::openPropertiesTab(DWORD pid)
void UiController::openPropertiesTab()
{
    HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pidTemp);
    if (!handle) return;

    char path[MAX_PATH];
    DWORD size = MAX_PATH;
    std::cout << "opening properties tab of : " << nameTemp << std::endl;
    if (QueryFullProcessImageNameA(handle, 0, path, &size))
    {
        SHELLEXECUTEINFOA shellExeInfo = {};
        shellExeInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
        shellExeInfo.fMask = SEE_MASK_INVOKEIDLIST;
        shellExeInfo.lpFile = path;
        std::cout << shellExeInfo.lpFile << std::endl;
        shellExeInfo.nShow = SW_SHOW;
        shellExeInfo.lpVerb = "properties";
        ShellExecuteEx(&shellExeInfo);
    }
    else
    {
        std::cout << "error opening: " << nameTemp << std::endl;
        CloseHandle(handle);
        return;
    }
    std::cout << "opened properties tab of : " << nameTemp << std::endl;
    CloseHandle(handle);
}

void UiController::endProcessTask()
{
    std::cout << "Selected Process: " << nameTemp << std::endl;
    std::cout << "PID: " << pidTemp << std::endl;
    HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pidTemp);
    if (!handle) return;
    TerminateProcess(handle, 1);
    CloseHandle(handle);
}
