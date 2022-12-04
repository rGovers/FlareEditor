#include "Windows/ControlWindow.h"

#include "FlareImGui.h"
#include "ProcessManager.h"
#include "Project.h"

ControlWindow::ControlWindow(ProcessManager* a_processManager, Project* a_project) : Window("Control")
{
    m_processManager = a_processManager;

    m_project = a_project;
}
ControlWindow::~ControlWindow()
{

}

void ControlWindow::Update()
{
    bool running = m_processManager->IsRunning();
    if (FlareImGui::ImageSwitchButton("Run Engine", "Textures/Icons/Controls_Stop.png", "Textures/Icons/Controls_Play.png", &running, glm::vec2(32.0f)))
    {
        if (running)
        {
            m_processManager->Start(m_project->GetCachePath());
        }
        else
        {
            m_processManager->Stop();
        }
    }
}