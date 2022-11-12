#pragma once

#include <string>

class Window
{
private:
    bool        m_open;

    std::string m_idStr;
    std::string m_displayName;

protected:

public:
    Window(const std::string_view& a_displayName);
    virtual ~Window();

    bool Display();

    virtual void Update() = 0;
};