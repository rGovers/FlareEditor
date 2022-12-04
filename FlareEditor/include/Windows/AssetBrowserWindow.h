#pragma once

#include "Windows/Window.h"

#include <list>
#include <vector>

class Project;

struct DirectoryNode
{
    uint32_t Parent;
    std::string Path;
    std::list<std::string> Files;
    std::list<uint32_t> Children;
};

class AssetBrowserWindow : public Window
{
private:
    static constexpr int ItemWidth = 128;

    Project*                   m_project;

    std::vector<DirectoryNode> m_fileTree;
    uint32_t                   m_curIndex;

    void MakeDirectoryNode(uint32_t a_parent, const std::string_view& a_path);
    void TraverseFolderTree(uint32_t a_index);
protected:

public:
    AssetBrowserWindow(Project* a_project);
    virtual ~AssetBrowserWindow();
    
    virtual void Refresh();
    virtual void Update();
};