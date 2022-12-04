#include "FileHandler.h"

#include "Datastore.h"
#include "Texture.h"

FileHandler* FileHandler::Instance = nullptr;

FileHandler::FileHandler()
{
    m_extTex.emplace(".cs", Datastore::GetTexture("Textures/FileIcons/FileIcon_CSharpScript.png"));
}
FileHandler::~FileHandler()
{

}

void FileHandler::Init()
{
    if (Instance == nullptr)
    {
        Instance = new FileHandler();
    }
}
void FileHandler::Destroy()
{
    if (Instance != nullptr)
    {
        delete Instance;
        Instance = nullptr;
    }
}

void FileHandler::GetFileData(const std::string_view& a_filename, const std::string_view& a_ext, FileCallback*& a_callback, Texture*& a_texture)
{
    a_texture = Datastore::GetTexture("Textures/FileIcons/FileIcon_Unknown.png");
    a_callback = nullptr;

    const auto iter = Instance->m_extTex.find(std::string(a_ext));

    if (iter != Instance->m_extTex.end())
    {
        a_texture = iter->second;
    }
}