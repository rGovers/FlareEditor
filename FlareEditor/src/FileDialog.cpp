#include "FileDialog.h"

#include <filesystem>
#include <imgui.h>

static constexpr int BaseLength = 1;

static bool IDirectoryExplorer(const std::list<std::string>& a_dirs, std::string* a_path)
{
    if (a_path->size() > BaseLength)
    {
        if (ImGui::Selectable(".."))
        {
            const uint32_t len = a_path->size();
            for (int i = len; i >= BaseLength; --i)
            {
                const char& c = (*a_path)[i];
                if (c == '/' || c == '\\')
                {
                    a_path->resize(i);

                    return false;
                }
            }

            a_path->resize(BaseLength);

            return false;
        }

        ImGui::NextColumn();
    }

    for (const std::string& dir : a_dirs)
    {
        const char *name = dir.c_str();
        if (ImGui::Selectable(name))
        {
            char &c = (*a_path)[a_path->size() - 1];
            if (c != '/')
            {
                if (c == '\\')
                {
                    c = '/';
                }
                else
                {
                    *a_path += '/';
                }
            }

            *a_path += dir;

            return false;
        }

        ImGui::NextColumn();
    }

    return true;
}

bool FileDialog::GenerateDirs(std::list<std::string>* a_dirs, const std::string_view& a_path)
{
    if (std::filesystem::exists(a_path))
    {   
        auto fsIter = std::filesystem::directory_iterator(a_path, std::filesystem::directory_options::skip_permission_denied);

        for (auto& iter : fsIter)
        {
            if (iter.is_directory())
            {
                a_dirs->emplace_back(iter.path().filename().u8string());
            }
        }

        a_dirs->sort();

        return true;
    }

    return false;
}
bool FileDialog::DirectoryExplorer(const std::list<std::string>& a_dirs, std::string* a_path)
{
    bool ret = true;

    const ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 region = ImGui::GetContentRegionAvail();

    if (ImGui::BeginChild("Dir", { region.x - style.FramePadding.x, region.y - ImGui::GetFrameHeightWithSpacing() * 2}))
    {
        if (std::filesystem::exists(*a_path))
        {
            ImGui::Columns(3);
            ImGui::BeginGroup();

            ret = IDirectoryExplorer(a_dirs, a_path);

            ImGui::EndGroup();
            ImGui::Columns();
        }

        ImGui::EndChild();
    }

    return ret;
}