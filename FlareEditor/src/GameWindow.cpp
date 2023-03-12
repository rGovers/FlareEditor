#include "Windows/GameWindow.h"

#include <imgui.h>

#include "InputBindings.h"
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
    const ImVec2 vMinIm = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMaxIm = ImGui::GetWindowContentRegionMax();
    const ImVec2 sizeIm = { vMaxIm.x - vMinIm.x, vMaxIm.y - vMinIm.y };

    m_processManager->SetSize((uint32_t)sizeIm.x, (uint32_t)sizeIm.y);

    if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
    {
        const ImVec2 mousePosIm = ImGui::GetMousePos();
        const ImVec2 winPosIm = ImGui::GetWindowPos();
        const ImGuiStyle& style = ImGui::GetStyle();

        const float titleBarSize = ImGui::GetFontSize() + style.FramePadding.y * 2;

        const glm::vec2 cPos = glm::vec2(mousePosIm.x - winPosIm.x, mousePosIm.y - (winPosIm.y + titleBarSize));

        m_processManager->PushCursorPos(cPos);

        unsigned char mouseState = 0;
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            mouseState |= 0b1 << MouseButton_Left;
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
        {
            mouseState |= 0b1 << MouseButton_Middle;
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            mouseState |= 0b1 << MouseButton_Right;
        }

        m_processManager->PushMouseState(mouseState);
    }

    ImGui::Image((ImTextureID)m_processManager->GetImage(), sizeIm);
}