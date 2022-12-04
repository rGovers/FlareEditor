#pragma once

#include "Windows/Window.h"

class ProcessManager;
class Project;

class ControlWindow : public Window
{
private:
    ProcessManager* m_processManager;

    Project*        m_project;

protected:

public:
    ControlWindow(ProcessManager* a_processManager, Project* a_project);
    ~ControlWindow();

    virtual void Update();
};