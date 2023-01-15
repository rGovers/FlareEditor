#pragma once

class RuntimeManager;

class GUI
{
private:
    RuntimeManager* m_runtime;

    GUI(RuntimeManager* a_runtime);
protected:

public:
    ~GUI();

    inline RuntimeManager* GetRuntime() const
    {
        return m_runtime;
    }

    static void Init(RuntimeManager* a_runtime);
    static void Destroy();
};
