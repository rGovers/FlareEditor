#pragma once

#include <list>
#include <string>

class FileDialog
{
private:

protected:

public:
    static bool GenerateDirs(std::list<std::string>* a_dirs, const std::string_view& a_path);

    static bool DirectoryExplorer(const std::list<std::string>& a_dirs, std::string* a_path);
};