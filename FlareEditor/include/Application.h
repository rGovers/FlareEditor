#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <string_view>

class Application
{
private:
    GLFWwindow* m_window;

    std::string m_title;

    uint32_t    m_width;
    uint32_t    m_height;

protected:

public:
    Application(uint32_t a_width, uint32_t a_height, const std::string_view& a_title);
    virtual ~Application();

    void Run();

    virtual void Update(double a_delta, double a_time) = 0;

    inline GLFWwindow* GetWindow() const
    {
        return m_window;
    }

    void Close();
};