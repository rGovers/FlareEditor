#pragma once

#include "PipeMessage.h"

#include <cstdint>

class ProcessManager
{
private:
    int      m_process;
         
    int      m_serverSock;
    int      m_pipeSock;

    uint32_t m_width;
    uint32_t m_height;

    void PushMessage(const PipeMessage& a_message) const;
    
protected:

public:
    ProcessManager();
    ~ProcessManager();

    inline bool IsRunning() const
    {
        return m_process > 0 && m_serverSock != -1;
    }

    bool Start();
    void Update();
    void Stop();
};