#include "../inc/SystemMonitor.h"


SystemMonitor::SystemMonitor()
{
    getCPUInfo();

}

void SystemMonitor::RenderUi()
{

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


    ImGuiIO& io = ImGui::GetIO();
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

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
            if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
            if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
            if (ImGui::MenuItem("Set Vsync ON/OFF", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
            { 
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; 
                if (vsync)
                    glfwSwapInterval(1);
            }

            ImGui::Separator();

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            ImGui::TextUnformatted(
                "SystemMonitor" "\n\tCreated by: Andrew Babos\n"
                "A fully customizable and bloat-free system monitor for OS and hardware information." "\n"
                "\tCreated as a project with C++ using OpenGL, GLFW3, GLAD and Dear ImGui.");
            ImGui::Separator();
            ImGui::Separator();
            ImGui::TextUnformatted("When docking is enabled, you can ALWAYS dock MOST windows into another!" "\n"
                "- Drag from window title bar or their tab to dock/undock." "\n"
                "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
                "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
                "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)");
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // show system hardware information
    renderCPU();

    ImGui::End();
	ImGui::Begin("SystemMonitor");
	ImGui::Text("testing stuff");
	ImGui::End();
}


void SystemMonitor::renderCPU()
{
    ImGui::Begin("CPU");
    ImGui::Text("CPU Usage: %.2f%%", cpuValue.load());
    ImGui::Separator();
    ImGui::PlotLines("CPU Usage", cpuHistory.data(), cpuHistory.size(), 0, NULL, 0.0f, 100.0f, ImVec2(0, 80));
    ImGui::End();
}


void SystemMonitor::renderRAM()
{

}

void SystemMonitor::setThreadsForInfo()
{

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
            std::this_thread::sleep_for(std::chrono::milliseconds(450));
        }
    });


}

void SystemMonitor::setVsync()
{
    vsync = !vsync;
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
