#include "Windows/GameWindow.h"

#include <imgui.h>

#include "ProcessManager.h"

GameWindow::GameWindow(ProcessManager* a_processManager) : Window("Game")
{
    m_processManager = a_processManager;
}
GameWindow::~GameWindow()
{

}

void GameWindow::Update()
{
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };

    m_processManager->SetSize((uint32_t)size.x, (uint32_t)size.y);
    
    GLuint texHandle = m_processManager->GetImage();
    if (texHandle != -1)
    {
        ImGui::Image((ImTextureID)texHandle, size);
    }
}