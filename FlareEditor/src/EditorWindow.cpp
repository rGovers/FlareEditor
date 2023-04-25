#include "Windows/EditorWindow.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <imgui.h>

#include "Gizmos.h"

EditorWindow::EditorWindow() : Window("Editor")
{
    m_width = -1;
    m_height = -1;

    glGenTextures(1, &m_textureHandle);
    glBindTexture(GL_TEXTURE_2D, m_textureHandle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_depthTextureHandle);
    glBindTexture(GL_TEXTURE_2D, m_depthTextureHandle);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1, 1, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &m_framebufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureHandle, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureHandle, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_translation = glm::vec3(0.0f, 1.0f, 10.0f);

    m_rotation = glm::identity<glm::quat>();
}
EditorWindow::~EditorWindow()
{
    glDeleteTextures(1, &m_textureHandle);
    glDeleteTextures(1, &m_depthTextureHandle);

    glDeleteFramebuffers(1, &m_framebufferHandle);
}

void EditorWindow::Update()
{
    const ImVec2 vMinIm = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMaxIm = ImGui::GetWindowContentRegionMax();
    const ImVec2 sizeIm = { vMaxIm.x - vMinIm.x, vMaxIm.y - vMinIm.y };

    if (sizeIm.x != m_width || sizeIm.y != m_height)
    {
        m_width = (uint32_t)sizeIm.x;
        m_height = (uint32_t)sizeIm.y;

        glBindTexture(GL_TEXTURE_2D, m_textureHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glBindTexture(GL_TEXTURE_2D, m_depthTextureHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    }

    if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);

        Gizmos::DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        const glm::mat4 per = glm::perspective(glm::pi<float>() * 0.4f, (float)m_width / m_height, 0.01f, 1000.0f);

        const glm::mat4 trans = glm::translate(glm::toMat4(m_rotation), m_translation);
        const glm::mat4 view = glm::inverse(trans);

        Gizmos::Render(per * view);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }   

    ImGui::Image((ImTextureID)m_textureHandle, sizeIm);
}