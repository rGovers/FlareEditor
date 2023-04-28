#pragma once

#include <filesystem>

class RuntimeManager;

class Workspace
{
private:
    RuntimeManager*       m_runtime;

    std::filesystem::path m_currentScene;

    std::filesystem::path m_selectionPath;

protected:

public:
    Workspace(RuntimeManager* a_runtime);
    ~Workspace();

    inline std::filesystem::path GetCurrentScene() const
    {
        return m_currentScene;
    }
    inline void SetCurrentScene(const std::filesystem::path& a_path)
    {
        m_currentScene = a_path;
    }

    inline RuntimeManager* GetRuntime() const
    {
        return m_runtime;
    }

    void SetScene(const std::filesystem::path& a_path, uint32_t a_size, const char* a_data);

    void OpenDef(const std::filesystem::path& a_path, uint32_t a_size, const char* a_data);
    void PushDef(const std::filesystem::path& a_path, uint32_t a_size, const char* a_data);
};