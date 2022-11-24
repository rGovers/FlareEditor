#pragma once

#include <string>

class AppMain;

class Project
{
private:
    AppMain*    m_app;

    std::string m_path;
    std::string m_name;

    void NewCallback(const std::string_view& a_path, const std::string_view& a_name);

protected:

public:
    Project(AppMain* a_app);
    ~Project();

    inline std::string_view GetPath() const
    {
        return m_path;
    }
    inline std::string_view GetName() const
    {
        return m_name;
    }

    inline bool ValidProject() const
    {
        return !m_path.empty() && !m_name.empty();
    }

    void New();
};