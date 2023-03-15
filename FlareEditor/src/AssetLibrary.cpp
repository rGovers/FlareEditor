#include "AssetLibrary.h"

#include <fstream>

#include "Logger.h"
#include "Runtime/RuntimeManager.h"

static AssetLibrary* Instance = nullptr;

FLARE_MONO_EXPORT(void, PropertiesWindow_WriteDef, MonoString* a_path, MonoArray* a_data)
{
    mono_unichar4* str = mono_string_to_utf32(a_path);
    const std::filesystem::path p = std::filesystem::path(std::u32string((char32_t*)str));

    const uintptr_t len = mono_array_length(a_data);

    char* data = new char[len];
    for (uintptr_t i = 0; i < len; ++i)
    {
        data[i] = (char)mono_array_get(a_data, mono_byte, i);
    }

    Instance->WriteDef(p, (uint32_t)len, data);

    mono_free(str);
}

AssetLibrary::AssetLibrary(RuntimeManager* a_runtime)
{
    m_runtime->BindFunction("FlareEditor.PropertiesWindow::WriteDef", (void*)PropertiesWindow_WriteDef);

    m_runtime = a_runtime;
    
    Instance = this;
}
AssetLibrary::~AssetLibrary()
{
    for (const Asset& asset : m_assets)
    {
        if (asset.Data != nullptr)
        {
            delete[] asset.Data;
        }
    }
}

std::filesystem::path AssetLibrary::GetRelativePath(const std::filesystem::path& a_relative, const std::filesystem::path& a_path)
{
    std::filesystem::path tempPath = a_path;
    std::filesystem::path path;

    while (tempPath != a_relative)
    {
        if (path.empty())
        {
            path = tempPath.stem();
            path.replace_extension(tempPath.extension());
        }
        else
        {
            path = tempPath.stem() / path;
        }
        
        tempPath = tempPath.parent_path();
    }

    return path;
}

void AssetLibrary::TraverseTree(const std::filesystem::path& a_path, const std::filesystem::path& a_workingDir)
{
    for (const auto& iter : std::filesystem::directory_iterator(a_path, std::filesystem::directory_options::skip_permission_denied))
    {
        if (iter.is_regular_file())
        {
            Asset asset;

            asset.Path = GetRelativePath(a_workingDir, iter.path());

            const std::filesystem::path ext = asset.Path.extension();
            const std::filesystem::path name = asset.Path.filename();

            if (ext == ".cs")
            {
                asset.AssetType = AssetType_Script;
            }
            else if (ext == ".def")
            {
                asset.AssetType = AssetType_Def;
            }
            else if (name == "about.xml")
            {
                asset.AssetType == AssetType_About;
            }
            else
            {
                asset.AssetType = AssetType_Other;
            }

            std::ifstream file = std::ifstream(iter.path());
            if (file.good() && file.is_open())
            {
                // Get file size by reading the entire file and seek back to the start
                // Could use os specific stuff to find file size but it is os specific
                file.ignore(std::numeric_limits<std::streamsize>::max());
                asset.Size = file.gcount();
                file.clear();
                file.seekg(0, std::ios::beg);

                asset.Data = new char[asset.Size];
                file.read(asset.Data, asset.Size);

                m_assets.emplace_back(asset);

                file.close();
            }
            else
            {
                Logger::Warning("Failed to open: " + iter.path().string());
            }
        }
        else if (iter.is_directory())
        {
            TraverseTree(iter.path(), a_workingDir);
        }
    }
}

void AssetLibrary::WriteDef(const std::filesystem::path& a_path, uint32_t a_size, char* a_data)
{
    for (Asset& a : m_assets)
    {
        if (a.AssetType != AssetType_Def)
        {
            continue;
        }

        if (a.Path == a_path)
        {
            a.Data = a_data;
            a.Size = a_size;
        }
    }
}

void AssetLibrary::Refresh(const std::filesystem::path& a_workingDir)
{
    for (const Asset& asset : m_assets)
    {
        if (asset.Data != nullptr)
        {
            delete[] asset.Data;
        }
    }

    m_assets.clear();

    const std::filesystem::path p = a_workingDir / "Project";

    TraverseTree(p, p);

    if (!m_runtime->IsBuilt())
    {
        return;
    }

    std::vector<const char*> assets;
    std::vector<uint32_t> sizes;
    std::vector<std::filesystem::path> paths;

    for (const Asset& asset : m_assets)
    {
        if (asset.AssetType == AssetType_Def)
        {
            assets.emplace_back(asset.Data);
            sizes.emplace_back((uint32_t)asset.Size);
            paths.emplace_back(asset.Path);
        }
    }

    MonoDomain* editorDomain = m_runtime->GetEditorDomain();

    MonoClass* stringClass = mono_get_string_class();
    MonoClass* byteClass = mono_get_byte_class();

    const uint32_t size = (uint32_t)assets.size();

    MonoArray* dataArray = mono_array_new(editorDomain, stringClass, (uintptr_t)size);
    MonoArray* pathArray = mono_array_new(editorDomain, stringClass, (uintptr_t)size);
    for (uint32_t i = 0; i < size; ++i)
    {
        MonoArray* data = mono_array_new(editorDomain, byteClass, sizes[i]);
        for (uint32_t j = 0; j < sizes[i]; ++j)
        {
            mono_array_set(data, mono_byte, j, (mono_byte)assets[i][j]);
        }

        mono_array_set(dataArray, MonoArray*, i, data);
        mono_array_set(pathArray, MonoString*, i, mono_string_from_utf32((mono_unichar4*)paths[i].u32string().c_str()));
    }

    void* args[] = 
    {
        dataArray,
        pathArray
    };

    m_runtime->ExecFunction("FlareEngine.Definitions", "DefLibrary", ":LoadDefs(byte[][],string[])", args);
    m_runtime->ExecFunction("FlareEngine.Definitions", "DefLibrary", ":ResolveDefs()", nullptr);
}
void AssetLibrary::BuildDirectory(const std::filesystem::path& a_path) const
{
    for (const Asset& asset : m_assets)
    {
        switch (asset.AssetType)
        {
        case AssetType_About:
        {
            const std::filesystem::path p = a_path / "Core" / "about.xml";

            std::filesystem::create_directories(p.parent_path());

            std::ofstream file = std::ofstream(p, std::ios_base::binary);
            if (file.good() && file.is_open())
            {
                file.write(asset.Data, asset.Size);

                file.close();
            }
            else
            {
                Logger::Warning("Failed writing about: " + p.string());
            }

            break;
        }
        case AssetType_Def:
        {
            std::filesystem::path p = a_path / "Core" / "Defs" / asset.Path;

            if (asset.Path.parent_path() == "Defs")
            {
                p = a_path / "Core" / asset.Path;
            }

            std::filesystem::create_directories(p.parent_path());

            std::ofstream file = std::ofstream(p, std::ios_base::binary);
            if (file.good() && file.is_open())
            {
                file.write(asset.Data, asset.Size);

                file.close();
            }
            else
            {
                Logger::Warning("Failed writing build def: " + p.string());
            }

            break;
        }
        case AssetType_Script:
        {
            continue;
        }
        case AssetType_Other:
        {
            const std::filesystem::path p = a_path / "Core" / "Assets" / asset.Path;

            std::filesystem::create_directories(p.parent_path());
            
            std::ofstream file = std::ofstream(p, std::ios_base::binary);
            if (file.good() && file.is_open())
            {
                file.write(asset.Data, asset.Size);

                file.close();
            }
            else
            {
                Logger::Warning("Failed writing asset: " + p.string());
            }

            break;
        }
        default:
        {
            Logger::Warning("Invalid Asset: " + asset.Path.string());
        }
        }
    }
}

void AssetLibrary::GetAsset(const std::filesystem::path& a_workingDir, const std::filesystem::path& a_path, uint32_t* a_size, const char** a_data)
{
    *a_size = 0;
    *a_data = nullptr;

    const std::filesystem::path rPath = GetRelativePath(a_workingDir, a_path);

    for (const Asset& asset : m_assets)
    {
        if (rPath == asset.Path)
        {
            *a_size = asset.Size;
            *a_data = asset.Data;

            return;
        }
    }
}

void AssetLibrary::Serialize(const std::filesystem::path& a_workingDir) const
{
    std::vector<Asset> defs;

    for (const Asset& asset : m_assets)
    {
        if (asset.AssetType == AssetType_Def)
        {
            defs.emplace_back(asset);
        }
    }

    const std::filesystem::path pPath = a_workingDir / "Project";

    MonoDomain* domain = m_runtime->GetEditorDomain();
    MonoClass* stringClass = mono_get_string_class();

    const uintptr_t count = (uintptr_t)defs.size();
    MonoArray* pathArray = mono_array_new(domain, stringClass, count);
    for (uintptr_t i = 0; i < count; ++i)
    {
        const Asset& a = defs[i];
        
        const std::u32string pStr = a.Path.u32string();
        mono_array_set(pathArray, MonoString*, i, mono_string_new_utf32(domain, (mono_unichar4*)pStr.c_str(), (int32_t)pStr.size()));
    }

    void* args[] =
    {
        pathArray
    };

    m_runtime->ExecFunction("FlareEditor", "PropertiesWindow", ":SerializeDefs(string[])", args);

    for (const Asset& a : m_assets)
    {
        const std::filesystem::path p = pPath / a.Path;

        std::ofstream file = std::ofstream(p);
        if (file.good() && file.is_open())
        {
            file.write(a.Data, a.Size);

            file.close();
        }
    }
}