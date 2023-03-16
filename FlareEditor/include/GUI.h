#pragma once

#include <list>
#include <string>

class RuntimeManager;

class GUI
{
private:
    RuntimeManager*        m_runtime;
    std::list<std::string> m_id;

    GUI(RuntimeManager* a_runtime);
protected:

public:
    ~GUI();

    inline RuntimeManager* GetRuntime() const
    {
        return m_runtime;
    }
    std::string GetID() const;

    inline void PushID(const std::string_view& a_str)
    {
        m_id.emplace_front(std::string(a_str));
    }
    inline void PopID()
    {
        m_id.pop_front();
    }

    static void Init(RuntimeManager* a_runtime);
    static void Destroy();
};
