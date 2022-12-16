#include "Project.h"

#include <filesystem>
#include <fstream>
#include <tinyxml2.h>

#include "AppMain.h"
#include "FlareEditorConfig.h"
#include "Logger.h"
#include "Modals/CreateProjectModal.h"
#include "TemplateBuilder.h"
#include "Templates/AssemblyControlCS.h"

Project::Project(AppMain* a_app)
{
    m_app = a_app;
}
Project::~Project()
{

}

void Project::NewCallback(const std::string_view& a_path, const std::string_view& a_name)
{
    m_path = std::string(a_path);
    m_name = std::string(a_name);

    if (!std::filesystem::exists(m_path))
    {
        std::filesystem::create_directories(m_path);
    }
    
    std::filesystem::path path = m_path + "/" + m_name;
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

    const std::string projectDir = m_path + "/Project";
    if (!std::filesystem::exists(projectDir))
    {
        std::filesystem::create_directories(projectDir);
    }

    const std::string sceneDir = projectDir + "/Scenes";
    if (!std::filesystem::exists(sceneDir))
    {
        std::filesystem::create_directories(sceneDir);
    }

    const std::string cacheDir = m_path + "/.cache";
    if (!std::filesystem::exists(cacheDir))
    {
        std::filesystem::create_directories(cacheDir);
    }

    m_app->RefreshProject();

    const std::string assemblyControlName = std::string(a_name) + "AssemblyControl";

    std::ofstream assemblyControlStream = std::ofstream(projectDir + "/" + assemblyControlName + ".cs");
    if (assemblyControlStream.is_open())
    {
        assemblyControlStream << TemplateBuilder::GenerateFromTemplate(ASSEMBLYCONTROLTEMPLATECS, a_name, assemblyControlName);
        assemblyControlStream.close();
    }
    else
    {
        Logger::Error("Unable to create Assembly Control");
    }
}
void Project::New()
{
    Logger::Message("New Project");

    m_app->PushModal(new CreateProjectModal(std::bind(&Project::NewCallback, this, std::placeholders::_1, std::placeholders::_2)));
}