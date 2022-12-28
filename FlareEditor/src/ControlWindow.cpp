#include "Windows/ControlWindow.h"

#include "AppMain.h"
#include "FlareImGui.h"
#include "Modals/ErrorModal.h"
#include "ProcessManager.h"
#include "Project.h"
#include "RuntimeManager.h"

ControlWindow::ControlWindow(AppMain* a_app, ProcessManager* a_processManager, RuntimeManager* a_runtimeManager, Project* a_project) : Window("Control")
{
    m_app = a_app;

    m_runtimeManager = a_runtimeManager;
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
            if (!m_runtimeManager->IsBuilt())
            {
                m_app->PushModal(new ErrorModal("Cannot start with build errors"));

                return;
            }

            m_processManager->Start(m_project->GetCachePath());
        }
        else
        {
            m_processManager->Stop();
        }
    }
}