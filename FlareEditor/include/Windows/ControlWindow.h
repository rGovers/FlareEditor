#pragma once

#include "Windows/Window.h"

class AppMain;
class ProcessManager;
class Project;
class RuntimeManager;

class ControlWindow : public Window
{
private:
    AppMain*        m_app;

    RuntimeManager* m_runtimeManager;
    ProcessManager* m_processManager;

    Project*        m_project;

protected:

public:
    ControlWindow(AppMain* a_app, ProcessManager* a_processManager, RuntimeManager* a_runtimeManager, Project* a_project);
    ~ControlWindow();

    virtual void Update(double a_delta);
};