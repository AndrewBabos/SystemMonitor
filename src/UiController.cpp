#include "../inc/UiController.h"
#include <Psapi.h>
#include "../inc/HardwareController.h"
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
                    ImGui::Text("Not implemented yet");
                    ImGui::Text("Font Scale: %.1f", fontScale);
                    ImGui::TreePop();
                }
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

void UiController::renderCPU(const std::atomic<float>& cpuValue, 
                             const std::array<float, 10>& cpuHistory,
                             const std::vector<std::array<float, 10>>& coreHistories)
{
    ImGui::Begin("CPU");
    if (ImPlot::BeginPlot("CPU Usage"))
    {
        ImPlot::SetupAxes("Intervals (Every 450ms)", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, cpuHistory.size(), ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        ImPlot::PlotLine("Total", cpuHistory.data(), cpuHistory.size());
        // individual cores
        /*if (ImGui::Button("Individual Cores"))
            CpuCore_Flag = !CpuCore_Flag;*/

        /*switch (CpuCore_Flag)
        {
        case true:
            for (size_t i = 0; i < coreHistories.size(); ++i)
            {
                std::string label = "Core #" + std::to_string(i);
                ImPlot::PlotLine(label.c_str(), coreHistories[i].data(), coreHistories[i].size());
            }
            break;
        case false:
            break;
        }*/
        /*if (CpuCore_Flag)
        {
            for (size_t i = 0; i < coreHistories.size(); ++i)
            {
                std::string label = "Core #" + std::to_string(i);
                ImPlot::PlotLine(label.c_str(), coreHistories[i].data(), coreHistories[i].size());
            }
        }*/
        /*for (size_t i = 0; i < coreHistories.size(); ++i)
        {
            std::string label = "Core #" + std::to_string(i);
            ImPlot::PlotLine(label.c_str(), coreHistories[i].data(), coreHistories[i].size());
        }*/
        ImPlot::EndPlot();
    }
    ImGui::Separator();
    ImGui::Text("CPU Usage: %.2f%%", cpuValue.load());
    ImGui::Text("CPU Clock Frequency: <work in progress>");
    ImGui::End();
}

void UiController::renderRAM(const std::atomic<float>& ramValue, 
                             const std::array<float, 10>& ramHistory, 
                             const std::atomic<uint64_t>& ramUsed,
                             const std::atomic<uint64_t>& totalPhysRAM)
{
    /*uint64_t totalBytes = totalPhysRAM.load(std::memory_order_relaxed);
    uint64_t usedBytes = ramUsed.load(std::memory_order_relaxed);*/

    ImGui::Begin("RAM");
    if (ImPlot::BeginPlot("RAM Usage"))
    {
        ImPlot::SetupAxes("Intervals (Every 3s)", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, ramHistory.size(), ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        ImPlot::PlotLine("Usage", ramHistory.data(), ramHistory.size());
        ImPlot::EndPlot();
    }
    ImGui::Separator();
    ImGui::Text("%.0fMB installed", (double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024));
    ImGui::Text("%.0fMB used", (double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024));
    ImGui::Text("%.0fMB available", ((double)totalPhysRAM.load(std::memory_order_relaxed) / (1024 * 1024) - (double)ramUsed.load(std::memory_order_relaxed) / (1024 * 1024)));
    ImGui::End();
}

void UiController::renderGPU()
{
    ImGui::Begin("GPU");
    ImGui::End();
}

void UiController::renderSysInfo(std::string CPUBrandString, SYSTEM_INFO& sysInfo)
{
    ImGui::Begin("System Information");
    ImGui::Text("Processor:          %s", CPUBrandString.c_str());
    ImGui::Text("Number of Cores:   %d", sysInfo.dwNumberOfProcessors);
    ImGui::Text("Processor Architecture:   %d", sysInfo.wProcessorArchitecture);
    ImGui::Separator();
    ImGui::End();
}

void UiController::renderProcesses(const HANDLE& hSnap, const PROCESSENTRY32& pe, const std::map<std::string, std::vector<ProcessInfo>> processMap)
{
    ImGui::Begin("Processes");
    //getProcessesInfo();
    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollY;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    if (ImGui::BeginTable("ProcessesTable", 6, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 15), 0.0f))
    {
        ImGui::TableSetupColumn("Process ID (PID)", ImGuiTableColumnFlags_DefaultSort, 0.0f, 0); // <-- ColumnUserID = 0
        ImGui::TableSetupColumn("Process Name");
        ImGui::TableSetupColumn("CPU");
        ImGui::TableSetupColumn("Memory");
        ImGui::TableSetupColumn("GPU");
        ImGui::TableSetupColumn("Network");
        ImGui::TableSetupScrollFreeze(0, 1); // set always visible
        ImGui::TableHeadersRow();

        static int selectedIndex, index = -1;
        //bool isSelected = false; // start on false, will be used eventually
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

                        char pidStr[16];
                        _ultoa_s(process.pid, pidStr, sizeof(pidStr), 10);
                        ImGui::Text(pidStr);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("     %s", processName.c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%d", process.cpuUsage);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%zu", process.memoryUsage);

                        ImGui::TableSetColumnIndex(4);
                        ImGui::Text("%d", process.gpuUsage);

                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%d", process.networkUsage);
                    }
                    ImGui::TreePop();
                }
            }
            else
            {
                const ProcessInfo& process = processList.front();
                char pidStr[12];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                _ultoa_s(process.pid, pidStr, sizeof(pidStr), 10);
                ImGui::Text(pidStr);

                ImGui::TableSetColumnIndex(1);
                //ImGui::Text("   %s", processName.c_str());
                ImGui::Text("%s", processName.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", process.cpuUsage);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%zu", process.memoryUsage);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%zu", process.gpuUsage);

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%zu", process.networkUsage);

            }
        }

        ImGui::EndTable();

        if (ImGui::Button("End Task"))
            std::cout << "ended task\n";
    }
}