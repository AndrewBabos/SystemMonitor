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

void UiController::renderCPU(const std::atomic<float>& cpuValue, const std::array<float, 10>& cpuHistory)
{
    ImGui::Begin("CPU");
    ImGui::Text("CPU Usage: %.2f%%", cpuValue.load());
    ImGui::Separator();
    if (ImPlot::BeginPlot("CPU Usage"))
    {
        ImPlot::SetupAxes("Intervals (Every 450ms)", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, cpuHistory.size(), ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        ImPlot::PlotLine("Usage", cpuHistory.data(), cpuHistory.size());
        ImPlot::EndPlot();
    }
    ImGui::End();
}

void UiController::renderRAM()
{

}

void UiController::renderSysInfo(char CPUBrandString[length_cpuBrandStr], SYSTEM_INFO& sysInfo)
{
    ImGui::Begin("System Information");
    ImGui::Text("Processor:          %s", CPUBrandString);
    ImGui::Text("Number of Cores:   %d", sysInfo.dwNumberOfProcessors);
    ImGui::Text("Processor Architecture:   %d", sysInfo.wProcessorArchitecture);
    ImGui::Separator();
    ImGui::End();
}

void UiController::renderProcesses(HANDLE& hSnap, PROCESSENTRY32& pe)
{
    ImGui::Begin("Processes");
    //getProcessesInfo();

    if (ImGui::BeginTable("SystemInfoTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Process");
        //ImGui::TableHeadersRow();
        ImGui::TableSetupColumn("CPU");
        ImGui::TableSetupColumn("Memory");
        ImGui::TableSetupColumn("GPU");
        ImGui::TableSetupColumn("Network");
        // This row is for processes
        //ImGui::TableHeadersRow();
        ImGui::TableHeadersRow();

        // figure out how to get this out of the controller
        if (hSnap == INVALID_HANDLE_VALUE)
        {
            std::cout << "ERROR GETTING stuff idk\n";
            return;
        }

        // figure out how to move this out of the render namespace
        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnap, &pe))
        {
            do
            {// Process info: pe.th32ProcessID, pe.szExeFile, etc.
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%S", pe.szExeFile);
                //ImGui::NextColumn();
                //ImGui::Selectable((const char*)pe.szExeFile);
            } while (Process32Next(hSnap, &pe));
        }

        ImGui::EndTable();
    }
    //ImGui::End();
}

// had to throw outside the namespace, cant use thread in non static
void fillProcessList(HANDLE& hSnap, PROCESSENTRY32& pe)
{
    //std::thread([]()
    //{
    //    while (true)
    //    {
    //        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    //        if (hSnap == INVALID_HANDLE_VALUE)
    //        {
    //            std::cerr << "Failed to create snapshot\n";
    //            std::this_thread::sleep_for(std::chrono::seconds(2));
    //            continue;
    //        }

    //        PROCESSENTRY32 pe{};
    //        pe.dwSize = sizeof(PROCESSENTRY32);
    //        std::vector<ProcessInfo> tempList;

    //        if (Process32First(hSnap, &pe))
    //        {
    //            do
    //            {
    //                ProcessInfo info{};
    //                info.pid = pe.th32ProcessID;
    //                info.name = pe.szExeFile;

    //                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
    //                if (hProcess)
    //                {
    //                    PROCESS_MEMORY_COUNTERS pmc{};
    //                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    //                    {// convert into MB from BYTES somehow
    //                        info.memoryUsage = pmc.WorkingSetSize;
    //                        //info.memoryUsage = pmc.WorkingSetSize / 1048576;
    //                    }
    //                    CloseHandle(hProcess);
    //                }

    //                tempList.push_back(info);
    //            } while (Process32Next(hSnap, &pe));
    //        }

    //        CloseHandle(hSnap);
    //        processList = std::move(tempList);

    //        // debugging
    //        for (auto process : processList)
    //        {
    //            std::string str(process.name.begin(), process.name.end());
    //            std::cout << str.c_str() << ", PID: " << process.pid << ", Memory (Bytes): " << process.memoryUsage << std::endl;
    //        }
    //        std::this_thread::sleep_for(std::chrono::seconds(3));
    //        // may have to clear and add it again
    //        //processList.clear();
    //    }
    //}).detach();
}

void UiController::testingTables(HANDLE& hSnap, PROCESSENTRY32& pe, std::vector<ProcessInfo> processList)
{
    ImGui::Begin("Processes");
    //getProcessesInfo();
    static ImGuiTableFlags flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollY;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    /*for (auto& process : processList)
    {
        std::cout << process.pid << std::endl;
    }*/

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

        if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
        {
            if (sort_specs->SpecsDirty)
            {
                std::sort(processList.begin(), processList.end(),
                    [sort_specs](const ProcessInfo& a, const ProcessInfo& b)
                    {
                        for (int n = 0; n < sort_specs->SpecsCount; n++)
                        {
                            const ImGuiTableColumnSortSpecs* spec = &sort_specs->Specs[n];
                            int result = (a.pid < b.pid) ? -1 : (a.pid > b.pid); // PID only
                            if (result != 0)
                                return spec->SortDirection == ImGuiSortDirection_Ascending ? result < 0 : result > 0;
                        }
                        return false;
                    });
                sort_specs->SpecsDirty = false;
            }
        }
        static int selectedIndex = -1; // store currently selected row

        for (int i = 0; i < processList.size(); i++)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            bool isSelected = (i == selectedIndex);

            // little hack till i figure this out
            char pidStr[16]; // enough for a 32-bit DWORD
            sprintf_s(pidStr, "%u", processList[i].pid);
            if (ImGui::Selectable(pidStr, isSelected, ImGuiSelectableFlags_SpanAllColumns))
                selectedIndex = i;

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%S", processList[i].name.c_str());
        }
        ImGui::EndTable();


        if (ImGui::Button("End Task"))
        {
            std::cout << "ended task\n";
        }
        //// figure out how to get this out of the controller
        //if (hSnap == INVALID_HANDLE_VALUE)
        //{
        //    std::cout << "ERROR GETTING stuff idk\n";
        //    return;
        //}

        //// figure out how to move this out of the render namespace
        //hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        //pe.dwSize = sizeof(PROCESSENTRY32);
        //if (Process32First(hSnap, &pe))
        //{
        //    do
        //    {// Process info: pe.th32ProcessID, pe.szExeFile, etc.
        //        ImGui::TableNextRow();
        //        ImGui::TableSetColumnIndex(0);
        //        ImGui::Text("%d", pe.th32ProcessID);
        //        //ImGui::TableNextRow();
        //        ImGui::TableSetColumnIndex(1);
        //        ImGui::Text("%S", pe.szExeFile);
        //        //ImGui::NextColumn();
        //        //ImGui::Selectable((const char*)pe.szExeFile);
        //    } while (Process32Next(hSnap, &pe));
        //}


        //ImGui::EndTable();
    }
}
