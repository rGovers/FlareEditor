#pragma once

#include <filesystem>
#include <list>
#include <string>

class RuntimeManager;

enum e_AssetType
{
    AssetType_Null = -1,
    AssetType_Script,
    AssetType_Def,
    AssetType_Other
};

struct Asset
{
    std::filesystem::path Path;
    e_AssetType AssetType;
    uint32_t Size;
    char* Data;
};

class AssetLibrary
{
private:
    std::list<Asset> m_assets;

    void TraverseTree(const std::filesystem::path& a_path, const std::filesystem::path& a_workingDir);

protected:

public:
    AssetLibrary();
    ~AssetLibrary();

    void Refresh(const std::filesystem::path& a_workingDir, RuntimeManager* a_runtime);
    void BuildDirectory(const std::filesystem::path& a_path) const;

    void GetAsset(const std::filesystem::path& a_workingDir, const std::filesystem::path& a_path, uint32_t* a_size, const char** a_data);
};