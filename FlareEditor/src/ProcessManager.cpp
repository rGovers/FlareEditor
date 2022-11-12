#include "ProcessManager.h"

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <assert.h>
#include <cstdlib>
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

    m_width = 1280;
    m_height = 720;
}   
ProcessManager::~ProcessManager()
{
    close(m_serverSock);
}

void ProcessManager::PushMessage(const PipeMessage& a_message) const
{
    write(m_pipeSock, &a_message, 8);
    write(m_pipeSock, a_message.Data, a_message.Length);
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

}
void ProcessManager::Stop()
{
    
}