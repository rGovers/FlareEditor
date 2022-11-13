#pragma once

#include "PipeMessage.h"

#include <cstdint>
#include <glad/glad.h>

class ProcessManager
{
private:
    int      m_process;
         
    int      m_serverSock;
    int      m_pipeSock;

    bool     m_resize;

    uint32_t m_width;
    uint32_t m_height;

    GLuint   m_tex;

    PipeMessage RecieveMessage() const;
    void PushMessage(const PipeMessage& a_message) const;
    
protected:

public:
    ProcessManager();
    ~ProcessManager();

    inline bool IsRunning() const
    {
        return m_process > 0 && m_serverSock != -1;
    }

    inline GLuint GetImage() const
    {
        return m_tex;
    }

    inline uint32_t GetWidth()
    {
        return m_width;
    }
    inline uint32_t GetHeight()
    {
        return m_height;
    }

    void SetSize(uint32_t a_width, uint32_t a_height);

    bool Start();
    void Update();
    void Stop();
};