#include "ProcessManager.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <assert.h>
#include <cstdlib>
#include <poll.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "Logger.h"

ProcessManager::ProcessManager()
{
    m_process = -1;   
    m_pipeSock = -1;

    const char* tempDir = std::getenv("TMPDIR");
    if (tempDir == nullptr)
    {
        tempDir = std::getenv("TMP");
    }
    if (tempDir == nullptr)
    {
        tempDir = std::getenv("TEMP");
    }
    if (tempDir == nullptr)
    {
        tempDir = std::getenv("XDG_RUNTIME_DIR");
    }

    assert(tempDir != nullptr);

    const std::string addrStr = std::string(tempDir) + "/FlareEngine-IPC";

    // Failsafe for unsafe close 
    // Frees the IPC from past instances
    unlink(addrStr.c_str());

    m_serverSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_serverSock < 0)
    {
        Logger::Error("Failed creating IPC");
        perror("socket");
        assert(0);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, addrStr.c_str());

    if (bind(m_serverSock, (struct sockaddr*)&addr, SUN_LEN(&addr)) < 0)
    {
        Logger::Error("Failed binding IPC");
        perror("bind");
        assert(0);
    }

    if (listen(m_serverSock, 10) < 0)
    {
        Logger::Error("Failed setting IPC listen");
        perror("listen");
        assert(0);
    }

    m_resize = false;

    m_width = 1280;
    m_height = 720;

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}   
ProcessManager::~ProcessManager()
{
    glDeleteTextures(1, &m_tex);

    close(m_serverSock);
}

PipeMessage ProcessManager::RecieveMessage() const
{
    PipeMessage msg;

    const uint32_t size = (uint32_t)read(m_pipeSock, &msg, PipeMessage::Size);
    if (size >= 8)
    {
        msg.Data = new char[msg.Length];
        char* DataBuffer = msg.Data;
        uint32_t len = DataBuffer - msg.Data;
        while (len < msg.Length)
        {
            DataBuffer += read(m_pipeSock, DataBuffer, msg.Length - len);

            len = DataBuffer - msg.Data;
        }

        return msg;
    }

    msg.Type = PipeMessageType_Null;
    msg.Length = 0;
    msg.Data = nullptr;

    return msg;
}
void ProcessManager::PushMessage(const PipeMessage& a_message) const
{
    write(m_pipeSock, &a_message, PipeMessage::Size);
    if (a_message.Data != nullptr)
    {
        write(m_pipeSock, a_message.Data, a_message.Length);
    }
}

bool ProcessManager::Start()
{
    Logger::Message("Spawning FlareEngine Instance");
    if (m_process == -1)
    {
        // This is a bit odd leaving this here as a note
        // This create another copy of the process on unix systems 
        // You can tell if you are the parent or child process based on the return result
        // If I am the child process I run the execute process which overwrites the current process with the new process
        // Unix systems are a bit odd but it works so I am not gonna question it
        m_process = fork();

        if (m_process < 0)
        {
            Logger::Error("Process Fork Failed");
            perror("fork");

            return false;
        }
        else if (m_process == 0)
        {
            // Starting the engine
            execl("./FlareNative", "--headless", "--testB", nullptr);
        }
        else
        {
            // Communicating with the engine
            struct timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;

            fd_set fdSet;
            FD_ZERO(&fdSet);
            FD_SET(m_serverSock, &fdSet);

            if (select(m_serverSock + 1, &fdSet, NULL, NULL, &tv) <= 0)
            {
                Logger::Error("Failed connecting to FlareEngine");

                return false;
            }

            if (FD_ISSET(m_serverSock, &fdSet))
            {
                m_pipeSock = accept(m_serverSock, NULL, NULL);
                if (m_pipeSock < 0)
                {
                    Logger::Error("Failed connecting to FlareEngine");

                    return false;
                }

                Logger::Message("Connected to FlareEngine");

                glm::ivec2 data = glm::ivec2((int)m_width, (int)m_height);

                PipeMessage msg;
                msg.Type = PipeMessageType_Resize;
                msg.Length = sizeof(data);
                msg.Data = (char*)&data;

                PushMessage(msg);

                return true;
            }
            
            Logger::Error("Failed to start FlareEngine");

            kill(m_process, SIGTERM);

            m_process = -1;

            return false;
        }
    }

    return false;
}
void ProcessManager::Update()
{
    if (m_pipeSock == -1 || m_process == -1)
    {
        return;
    }

    struct pollfd fds;
    fds.fd = m_pipeSock;
    fds.events = POLLIN;

    while (poll(&fds, 1, 1) > 0)
    {
        if (fds.revents & (POLLNVAL | POLLERR | POLLHUP))
        {
            m_pipeSock = -1;

            return;
        }

        if (fds.revents & POLLIN)
        {
            const PipeMessage msg = RecieveMessage();

            switch (msg.Type)
            {
            case PipeMessageType_PushFrame:
            {
                if (msg.Length == m_width * m_height * 4)
                {
                    glBindTexture(GL_TEXTURE_2D, m_tex);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, msg.Data);
                }

                break;
            }
            case PipeMessageType_Message:
            {
                constexpr uint32_t TypeSize = sizeof(e_LoggerMessageType);

                const std::string_view str = std::string_view(msg.Data + TypeSize, msg.Length - TypeSize);

                switch (*(e_LoggerMessageType*)msg.Data)
                {
                case LoggerMessageType_Message:
                {
                    Logger::Message(str);

                    break;
                }
                case LoggerMessageType_Warning:
                {
                    Logger::Warning(str);

                    break;
                }
                case LoggerMessageType_Error:
                {
                    Logger::Error(str);

                    break;
                }
                }

                break;
            }
            default:
            {
                Logger::Error("Invalid Pipe Message: " + std::to_string(msg.Type));

                break;
            }
            }
            
            if (msg.Data)
            {
                delete[] msg.Data;
            }
        }
    }

    if (m_resize)
    {
        m_resize = false;

        glm::ivec2 size = glm::ivec2((int)m_width, (int)m_height);

        PipeMessage msg;
        msg.Type = PipeMessageType_Resize;
        msg.Length = sizeof(glm::ivec2);
        msg.Data = (char*)&size;

        PushMessage(msg);
    }
}
void ProcessManager::Stop()
{
    Logger::Message("Stopping FlareEngine Instance");
    PipeMessage msg;
    msg.Type = PipeMessageType_Close;
    msg.Length = 0;
    msg.Data = nullptr;

    PushMessage(msg);

    close(m_pipeSock);
    m_pipeSock = -1;
    m_process = -1;
}
void ProcessManager::SetSize(uint32_t a_width, uint32_t a_height)
{
    if (m_width != a_width || m_height != a_height)
    {
        m_width = a_width;
        m_height = a_height;

        m_resize = true;
    }
}