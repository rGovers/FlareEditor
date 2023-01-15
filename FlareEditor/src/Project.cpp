#include "Project.h"

#include <filesystem>
#include <fstream>
#include <tinyxml2.h>

#include "AppMain.h"
#include "FlareEditorConfig.h"
#include "Logger.h"
#include "Modals/CreateProjectModal.h"
#include "Modals/OpenProjectModal.h"
#include "TemplateBuilder.h"
#include "Templates/AssemblyControlCS.h"

static void GenerateDirs(const std::filesystem::path& a_path)
{
    if (!std::filesystem::exists(a_path))
    {
        std::filesystem::create_directories(a_path);
    }

    const std::filesystem::path projectDir = a_path / "Project";
    if (!std::filesystem::exists(projectDir))
    {
        std::filesystem::create_directories(projectDir);
    }

    const std::filesystem::path sceneDir = projectDir / "Scenes";
    if (!std::filesystem::exists(sceneDir))
    {
        std::filesystem::create_directories(sceneDir);
    }

    const std::filesystem::path cacheDir = a_path / ".cache";
    if (!std::filesystem::exists(cacheDir))
    {
        std::filesystem::create_directories(cacheDir);
    }
}

Project::Project(AppMain* a_app)
{
    m_app = a_app;
}
Project::~Project()
{

}

void Project::NewCallback(const std::filesystem::path& a_path, const std::string_view& a_name)
{
    m_path = a_path;
    m_name = std::string(a_name);

    GenerateDirs(m_path);
    
    std::filesystem::path path = m_path / m_name;
    if (!path.has_extension())
    {
        path += ".flareproj";
    }

    tinyxml2::XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());
    tinyxml2::XMLElement* rootElement = doc.NewElement("Project");
    doc.InsertEndChild(rootElement);
    
    tinyxml2::XMLElement* versionElement = doc.NewElement("Version");
    rootElement->InsertEndChild(versionElement);
    
    tinyxml2::XMLElement* versionMajorElement = doc.NewElement("Major");
    versionElement->InsertEndChild(versionMajorElement);
    versionMajorElement->SetText(FLAREEDITOR_VERSION_MAJOR);

    tinyxml2::XMLElement* versionMinorElement = doc.NewElement("Minor");
    versionElement->InsertEndChild(versionMinorElement);
    versionMinorElement->SetText(FLAREEDITOR_VERSION_MINOR);

    doc.SaveFile(path.string().c_str());

    m_app->RefreshProject();

    const std::string assemblyControlName = std::string(a_name) + "AssemblyControl";

    std::ofstream assemblyControlStream = std::ofstream(m_path / "Project" / (assemblyControlName + ".cs"));
    if (assemblyControlStream.good() && assemblyControlStream.is_open())
    {
        assemblyControlStream << TemplateBuilder::GenerateFromTemplate(ASSEMBLYCONTROLTEMPLATECS, a_name, assemblyControlName);
        assemblyControlStream.close();
    }
    else
    {
        Logger::Error("Unable to create Assembly Control");
    }
}
void Project::OpenCallback(const std::filesystem::path& a_path, const std::string_view& a_name)
{
    m_path = a_path;
    m_name = std::string(a_name);

    GenerateDirs(m_path);

    m_app->RefreshProject();
}
void Project::New()
{
    Logger::Message("New Project");

    m_app->PushModal(new CreateProjectModal(m_app, std::bind(&Project::NewCallback, this, std::placeholders::_1, std::placeholders::_2)));
}
void Project::Open()
{
    Logger::Message("Open Project");

    m_app->PushModal(new OpenProjectModal(m_app, std::bind(&Project::OpenCallback, this, std::placeholders::_1, std::placeholders::_2)));
}