#pragma once

#include "Windows/Window.h"

class ProcessManager;

class ControlWindow : public Window
{
private:
    ProcessManager* m_processManager;

protected:

public:
    ControlWindow(ProcessManager* a_processManager);
    ~ControlWindow();

    virtual void Update();
};