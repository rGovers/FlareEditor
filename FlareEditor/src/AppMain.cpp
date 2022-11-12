#include "AppMain.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <sstream>

#include "ProcessManager.h"
#include "Windows/ConsoleWindow.h"
#include "Windows/EditorWindow.h"
#include "Windows/GameWindow.h"

static void GLAPIENTRY MessageCallback
( 
    GLenum a_source,
    GLenum a_type,
    GLuint a_id,
    GLenum a_severity,
    GLsizei a_length,
    const GLchar* a_message,
    const void* a_userParam 
)
{
    std::stringstream stream;

    stream << "GL CALLBACK: ";

    switch (a_severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
    {
        stream << "HIGH SEVERITY ";

        break;
    }
    case GL_DEBUG_SEVERITY_MEDIUM:
    {
        stream << "MEDIUM SEVERITY ";

        break;
    }
    case GL_DEBUG_SEVERITY_LOW:
    {
        stream << "LOW SEVERITY ";

        break;
    }
    default:
    {
        stream << std::hex << a_severity << " ";

        break;
    }
    }

    stream << a_message;

    switch (a_type)
    {
    case GL_DEBUG_TYPE_ERROR:
    {
        Logger::Error(stream.str());

        break;
    }
    case GL_DEBUG_TYPE_PERFORMANCE:
    {
        Logger::Warning(stream.str());

        break;
    }
    default:
    {
        Logger::Message(stream.str());

        break;
    }
    }
}

AppMain::AppMain() : Application(1280, 720, "FlareEditor")
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true))
	{
		assert(0);
	}
	if (!ImGui_ImplOpenGL3_Init("#version 130"))
	{
		assert(0);
	}

    m_process = new ProcessManager();

    m_windows.emplace_back(new ConsoleWindow());
    m_windows.emplace_back(new EditorWindow());
    m_windows.emplace_back(new GameWindow());

    m_process->Start();
}
AppMain::~AppMain()
{
    for (Window* wind : m_windows)
    {
        delete wind;
    }

    delete m_process;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void AppMain::Update(double a_delta, double a_time)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Project"))
            {
                Logger::Error("New Project Not Implemented");
            }

            if (ImGui::MenuItem("Open Project"))
            {
                Logger::Error("Open Project Not Implemented");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Build Project"))
            {
                Logger::Error("Build Project Not Implemented");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
            {
                Close();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::BeginMenu("Add"))
            {
                if (ImGui::MenuItem("Editor"))
                {
                    m_windows.emplace_back(new EditorWindow());
                }

                if (ImGui::MenuItem("Game"))
                {
                    m_windows.emplace_back(new GameWindow());
                }

                if (ImGui::MenuItem("Console"))
                {
                    m_windows.emplace_back(new ConsoleWindow());
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    for (auto iter = m_windows.begin(); iter != m_windows.end(); ++iter)
    {
        if (!(*iter)->Display())
        {
            delete *iter;
            iter = m_windows.erase(iter);

            if (iter == m_windows.end())
            {
                break;
            }
        }
    }
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}