#pragma once

#include <filesystem>
#include <list>
#include <string>

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

    void Refresh(const std::string_view& a_workingDir);
    void BuildDirectory(const std::string_view& a_path) const;
};