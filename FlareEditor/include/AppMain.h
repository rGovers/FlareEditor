#pragma once

#include "Application.h"

#include <vector>

#include "Windows/Window.h"
#include "Modals/Modal.h"

class Project;
class ProcessManager;
class RuntimeManager;

class AppMain : public Application
{
private:
    double               m_titleSet;

    bool                 m_refresh;

    std::vector<Window*> m_windows;
    std::vector<Modal*>  m_modals;

    Project*             m_project;
    ProcessManager*      m_process;
    RuntimeManager*      m_runtime;

protected:

public:
    AppMain();
    virtual ~AppMain();

    virtual void Update(double a_delta, double a_time);

    inline void RefreshProject()
    {
        m_refresh = true;
    }

    void PushModal(Modal* a_modal);
};