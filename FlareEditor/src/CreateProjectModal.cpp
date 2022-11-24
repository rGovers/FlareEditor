#include "Modals/CreateProjectModal.h"

#include <imgui.h>
#include <string.h>

#include "FileDialog.h"
#include "IO.h"
#include "Logger.h"

CreateProjectModal::CreateProjectModal(Callback a_callback) : Modal("Create Project", glm::vec2(640, 480))
{
    m_callback = a_callback;

    m_path = IO::GetHomePath();
    m_name = "UnnamedProject";

    FileDialog::GenerateDirs(&m_dirs, m_path);
}
CreateProjectModal::~CreateProjectModal()
{

}

bool CreateProjectModal::Update()
{
    char Buffer[BufferSize];

    const uint32_t pathLen = (uint32_t)m_path.length();
    if (pathLen > BufferSize)
    {
        Logger::Error("Path exceeds buffer size");

        m_path = IO::GetHomePath();

        return true;
    }
    for (uint32_t i = 0; i < pathLen; ++i)
    {
        Buffer[i] = m_path[i];
    }
    Buffer[pathLen] = 0;

    if (ImGui::InputText("Path", Buffer, BufferSize))
    {
        m_path = Buffer;

        m_dirs.clear();

        FileDialog::GenerateDirs(&m_dirs, m_path);
    }   

    if (!FileDialog::DirectoryExplorer(m_dirs, &m_path))
    {
        m_dirs.clear();

        FileDialog::GenerateDirs(&m_dirs, m_path);
    }

    const uint32_t nameLen = (uint32_t)m_name.length();
    if (nameLen > BufferSize)
    {
        Logger::Error("Name exceed buffer size");

        m_name.clear();

        return true;
    }
    for (uint32_t i = 0; i < nameLen; ++i)
    {
        Buffer[i] = m_name[i];
    }
    Buffer[nameLen] = 0;

    if (ImGui::InputText("Name", Buffer, BufferSize))
    {
        m_name = Buffer;
    }

    ImGui::SameLine();

    if (ImGui::Button("Create"))
    {
        char& c = m_path[pathLen - 1];
        if (c != '/')
        {
            if (c == '\\')
            {
                c = '/';
            }
            else
            {
                m_path += '/';
            }
        }

        if (m_name.empty())
        {
            Logger::Error("Invalid Name");

            return true;
        }

        m_callback(m_path + m_name, m_name);

        return false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        return false;
    }

    return true;
}