#include "AssetLibrary.h"

#include <fstream>

#include "Logger.h"

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

void AssetLibrary::TraverseTree(const std::filesystem::path& a_path, const std::filesystem::path& a_workingDir)
{
    for (const auto& iter : std::filesystem::directory_iterator(a_path, std::filesystem::directory_options::skip_permission_denied))
    {
        if (iter.is_regular_file())
        {
            Asset asset;

            std::filesystem::path tempPath = iter.path();

            while (tempPath != a_workingDir)
            {
                if (asset.Path.empty())
                {
                    asset.Path = tempPath.stem();
                    asset.Path.replace_extension(tempPath.extension());
                }
                else
                {
                    asset.Path = tempPath.stem() / asset.Path;
                }
                tempPath = tempPath.parent_path();
            }

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

void AssetLibrary::Refresh(const std::string_view& a_workingDir)
{
    for (const Asset& asset : m_assets)
    {
        if (asset.Data != nullptr)
        {
            delete[] asset.Data;
        }
    }

    m_assets.clear();

    const std::filesystem::path p = std::filesystem::path(a_workingDir) / "Project";

    TraverseTree(p, p);
}
void AssetLibrary::BuildDirectory(const std::string_view& a_path) const
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
            const std::filesystem::path p = std::filesystem::path(a_path) / "Core" / "Defs" / asset.Path;
            
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
            const std::filesystem::path p = std::filesystem::path(a_path) / "Core" / "Assets" / asset.Path;

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