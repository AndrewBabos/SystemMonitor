#include "../inc/SystemMonitor.h"


SystemMonitor::SystemMonitor()
{
    vsync = false;
    // Get the information associated with each extended ID.
    int CPUInfo[4] = {};
    unsigned nExIds, i = 0;
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        switch (i)
        {
            case (0x80000002):
                memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            case (0x80000003):
                memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            case (0x80000004):
                memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }
    GetSystemInfo(&sysInfo);
    std::cout << "Processor Architecture: " << sysInfo.wProcessorArchitecture << std::endl;
    getCPUInfo();
}

void SystemMonitor::RenderUi()
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
            ImGui::TextUnformatted(
                "SystemMonitor" "\nCreated by: Andrew Babos\n"
                "A fully customizable and bloat-free system monitor for OS and hardware information." "\n"
                "Created as a project with C++ using OpenGL, GLFW3, GLAD and Dear ImGui.\n");
            ImGui::Separator();
            ImGui::Text("Socials");
            ImGui::TextLinkOpenURL("My website", "https://andrewbabos.ca");
            ImGui::TextLinkOpenURL("Linkedin", "https://www.linkedin.com/in/andrew-babos");
            ImGui::TextLinkOpenURL("Github", "https://github.com/AndrewBabos");
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


    // show system hardware information
    renderCPU();
    renderSysInfo();
    renderProcesses();

    // references
    ImGui::ShowDemoWindow();

	ImGui::End();
}

void SystemMonitor::renderCPU()
{
    ImGui::Begin("CPU");
    ImGui::Text("CPU Usage: %.2f%%", cpuValue.load());
    ImGui::Separator();
    if (ImPlot::BeginPlot("CPU Usage"))
    {
        ImPlot::SetupAxes("", "Percent Used");
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, cpuHistory.size(), ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
        ImPlot::PlotLine("Usage", cpuHistory.data(), cpuHistory.size());
        ImPlot::EndPlot();
    }
    ImGui::End();
}


void SystemMonitor::renderRAM()
{

}

void SystemMonitor::renderProcesses()
{
    // sample, will change
    ImGui::Begin("Processes");
    if (ImGui::BeginTable("SystemInfoTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Metric");
        ImGui::TableSetupColumn("Value");
        ImGui::TableSetupColumn("Unit");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CPU Usage");
        ImGui::TableSetColumnIndex(1);
        //ImGui::Text("%.2f", cpuUsage);
        ImGui::Text("%.2f");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%%");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Memory Usage");
        ImGui::TableSetColumnIndex(1);
        //ImGui::Text("%.2f", memUsage);
        ImGui::Text("%.2f");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("MB");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Temperature");
        ImGui::TableSetColumnIndex(1);
        //ImGui::Text("%.1f", tempC);
        ImGui::Text("%.1f");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("°C");

        ImGui::EndTable();
    }
    ImGui::End();
}

void SystemMonitor::setThreadsForInfo()
{

}

void SystemMonitor::renderSysInfo()
{
    ImGui::Begin("System Information");
    ImGui::Text("Processor:          %s", CPUBrandString);
    ImGui::Text("Number of Cores:   %d", sysInfo.dwNumberOfProcessors);
    ImGui::Text("Processor Architecture:   %d", sysInfo.wProcessorArchitecture);
    ImGui::Separator();
    ImGui::End();
}

void SystemMonitor::getCPUInfo()
{
    if (PdhOpenQuery(NULL, 0, &query) != ERROR_SUCCESS)
        return;
    if (PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter) != ERROR_SUCCESS)
        return;

    SetThreadPriority(cpuThread.native_handle(), THREAD_PRIORITY_LOWEST);
    cpuThread = std::thread([this]()
    {
        while (running.load())
        {
            PdhCollectQueryData(query);
            PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal);
            cpuValue.store(counterVal.doubleValue);

            switch (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterVal))
            {
                case ERROR_SUCCESS:
                    cpuHistory[index] = static_cast<float>(counterVal.doubleValue);
                    index = (index + 1) % cpuHistory.size();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(thread_Update));
            //std::this_thread::sleep_for(std::chrono::seconds(2)); // task manager update
        }
    });


}

bool SystemMonitor::setVsync()
{
    return !vsync;
}

void SystemMonitor::shutdown()
{
}

SystemMonitor::~SystemMonitor()
{
    running.store(false);
    if (cpuThread.joinable()) 
        cpuThread.join();
    PdhCloseQuery(query);
}
