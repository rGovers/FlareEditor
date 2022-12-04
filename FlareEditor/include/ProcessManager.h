#pragma once

#include "PipeMessage.h"

#include <cstdint>
#include <glad/glad.h>
#include <string_view>

class ProcessManager
{
private:
    int      m_process;
         
    int      m_serverSock;
    int      m_pipeSock;

    bool     m_resize;

    uint32_t m_width;
    uint32_t m_height;

    int      m_frames;
    double   m_time;

    double   m_frameTime;
    double   m_fps;

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

    inline double GetFPS() const
    {
        return m_fps;
    }
    inline double GetFrameTime() const
    {
        return m_frameTime;
    }

    void SetSize(uint32_t a_width, uint32_t a_height);

    bool Start(const std::string_view& a_workingDir);
    void Update();
    void Stop();
};