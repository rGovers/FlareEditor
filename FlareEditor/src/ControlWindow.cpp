#include "Windows/ControlWindow.h"

#include <imgui.h>

#include "ProcessManager.h"

ControlWindow::ControlWindow(ProcessManager* a_processManager) : Window("Control")
{
    m_processManager = a_processManager;
}
ControlWindow::~ControlWindow()
{

}

void ControlWindow::Update()
{
    if (m_processManager->IsRunning())
    {
        if (ImGui::Button("Stop"))
        {
            m_processManager->Stop();
        }
    }
    else
    {
        if (ImGui::Button("Start"))
        {
            m_processManager->Start();
        }
    }
}