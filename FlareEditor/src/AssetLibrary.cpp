#include "AssetLibrary.h"

#include <fstream>

#include "Logger.h"
#include "Runtime/RuntimeManager.h"

AssetLibrary::AssetLibrary()
{

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

static std::filesystem::path GetRelativePath(const std::filesystem::path& a_relative, const std::filesystem::path& a_path)
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
            if (ext == ".cs")
            {
                asset.AssetType = AssetType_Script;
            }
            else if (ext == ".def")
            {
                asset.AssetType = AssetType_Def;
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

void AssetLibrary::Refresh(const std::filesystem::path& a_workingDir, RuntimeManager* a_runtime)
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

    std::vector<const char*> assets;
    std::vector<unsigned int> sizes;
    std::vector<std::filesystem::path> paths;

    for (const Asset& asset : m_assets)
    {
        if (asset.AssetType == AssetType_Def)
        {
            assets.emplace_back(asset.Data);
            sizes.emplace_back((unsigned int)asset.Size);
            paths.emplace_back(p / asset.Path);
        }
    }

    MonoDomain* editorDomain = a_runtime->GetEditorDomain();

    MonoClass* stringClass = mono_get_string_class();

    const uint32_t size = (uint32_t)assets.size();

    MonoArray* assetsArray = mono_array_new(editorDomain, stringClass, (uintptr_t)size);
    MonoArray* pathArray = mono_array_new(editorDomain, stringClass, (uintptr_t)size);
    for (uint32_t i = 0; i < size; ++i)
    {
        mono_array_set(assetsArray, MonoString*, i, mono_string_new_len(editorDomain, assets[i], sizes[i]));
        mono_array_set(pathArray, MonoString*, i, mono_string_from_utf32((mono_unichar4*)paths[i].u32string().c_str()));
    }

    void* args[] = 
    {
        assetsArray,
        pathArray
    };

    a_runtime->ExecFunction("FlareEngine.Definitions", "DefLibrary", ":LoadDefs(string[],string[])", args);
}
void AssetLibrary::BuildDirectory(const std::filesystem::path& a_path) const
{
    for (const Asset& asset : m_assets)
    {
        switch (asset.AssetType)
        {
        case AssetType_Script:
        {
            continue;
        }
        case AssetType_Def:
        {
            const std::filesystem::path p = a_path / "Core" / "Defs" / asset.Path;
            
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