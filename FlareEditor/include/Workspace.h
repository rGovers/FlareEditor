#pragma once

#include <filesystem>

class RuntimeManager;

class Workspace
{
private:
    RuntimeManager*       m_runtime;

    std::filesystem::path m_selectionPath;

protected:

public:
    Workspace(RuntimeManager* a_runtime);
    ~Workspace();

    void OpenDef(const std::filesystem::path& a_path, uint32_t a_size, const char* a_data);
    void PushDef(const std::filesystem::path& a_path, uint32_t a_size, const char* a_data);
};