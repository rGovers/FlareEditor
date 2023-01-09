#include "AppMain.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <string>
#include <sstream>

#include "AssetLibrary.h"
#include "Datastore.h"
#include "FileHandler.h"
#include "Modals/CreateProjectModal.h"
#include "ProcessManager.h"
#include "ProfilerData.h"
#include "Project.h"
#include "RuntimeManager.h"
#include "Windows/AssetBrowserWindow.h"
#include "Windows/ConsoleWindow.h"
#include "Windows/ControlWindow.h"
#include "Windows/EditorWindow.h"
#include "Windows/GameWindow.h"
#include "Windows/ProfilerWindow.h"

#include "Modals/ErrorModal.h"

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
        break;
    }
    }
}

static void SetImguiStyle()
{
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameRounding = 4.0f;
    style.WindowRounding = 6.0f;
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.WindowMenuButtonPosition = ImGuiDir_Right;

    ImVec4* colors = style.Colors;
    // colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.06f, 0.94f);
}

AppMain::AppMain() : Application(1280, 720, "FlareEditor")
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    SetImguiStyle();

    if (!ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true))
	{
		assert(0);
	}
	if (!ImGui_ImplOpenGL3_Init("#version 130"))
	{
		assert(0);
	}

    Datastore::Init();
    FileHandler::Init();

    ProfilerData::Init();

    m_process = new ProcessManager();
    m_runtime = new RuntimeManager();

    m_project = new Project(this);

    m_assets = new AssetLibrary();

    m_windows.emplace_back(new ConsoleWindow());
    m_windows.emplace_back(new ControlWindow(this, m_process, m_runtime, m_project));
    m_windows.emplace_back(new EditorWindow());
    m_windows.emplace_back(new GameWindow(m_process));
    m_windows.emplace_back(new AssetBrowserWindow(m_project));

    m_titleSet = 0.0;
    m_refresh = false;
}
AppMain::~AppMain()
{
    delete m_project;

    delete m_assets;

    if (m_process->IsRunning())
    {
        m_process->Stop();
    }

    for (Window* wind : m_windows)
    {
        delete wind;
    }

    for (Modal* modal : m_modals)
    {
        delete modal;
    }

    delete m_process;
    delete m_runtime;

    ProfilerData::Destroy();

    FileHandler::Destroy();
    Datastore::Destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void AppMain::Update(double a_delta, double a_time)
{    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    const ImGuiIO& io = ImGui::GetIO();

    std::string title = "FlareEditor";

    const int focusState = glfwGetWindowAttrib(GetWindow(), GLFW_FOCUSED);
    if (!m_focused && focusState)
    {
        m_refresh = true;
    }

    m_focused = (bool)focusState;

    const bool validProject = m_project->IsValidProject();

    if (validProject)
    {
        title += " [" + std::string(m_project->GetName()) + "]";
    }

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    }

    m_process->Update();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Project"))
            {
                m_project->New();
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
                    m_windows.emplace_back(new GameWindow(m_process));
                }

                if (ImGui::MenuItem("Control"))
                {
                    m_windows.emplace_back(new ControlWindow(this, m_process, m_runtime, m_project));
                }

                if (ImGui::MenuItem("Console"))
                {
                    m_windows.emplace_back(new ConsoleWindow());
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Profiler"))
                {
                    m_windows.emplace_back(new ProfilerWindow());
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    if (validProject)
    {
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
    }
    
    if (!m_modals.empty())
    {
        Modal* modal = m_modals[m_modals.size() - 1];

        if (!modal->Display())
        {
            delete modal;
            m_modals.pop_back();
        }
    }

    m_runtime->Update();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    if (m_refresh && validProject)
    {
        Logger::Message("Refreshing Project");
        m_refresh = false;

        const std::string path = std::string(m_project->GetPath()); 

        m_assets->Refresh(path);
        m_assets->BuildDirectory(path + "/.cache");

        for (Window* wind : m_windows)
        {
            wind->Refresh();
        }

        if (m_runtime->Build(path, m_project->GetName()))
        {
            m_runtime->Start();
        }
    }

    const int fps = (int)(1.0 / a_delta);

    title += " EditorFPS: " + std::to_string(fps);

    if (m_process->IsRunning())
    {
        const int engineFPS = (int)m_process->GetFPS();
        const int engineUPS = (int)m_process->GetUPS();

        title += " EngineUPS: " + std::to_string(engineUPS);
        title += " EngineFPS: " + std::to_string(engineFPS);
    }

    m_titleSet -= a_delta;

    if (m_titleSet <= 0.0)
    {
        SetTitle(title);

        m_titleSet += 0.1;
    }
}

void AppMain::PushModal(Modal* a_modal)
{
    m_modals.emplace_back(a_modal);
}