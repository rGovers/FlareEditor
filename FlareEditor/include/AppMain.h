#pragma once

#include "Application.h"

#include <vector>

#include "Windows/Window.h"

class ProcessManager;

class AppMain : public Application
{
private:
    std::vector<Window*> m_windows;

    ProcessManager*      m_process;

protected:

public:
    AppMain();
    virtual ~AppMain();

    virtual void Update(double a_delta, double a_time);
};