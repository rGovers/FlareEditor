#pragma once

#include <functional>
#include <string>
#include <unordered_map>

class Texture;

class FileHandler
{
public:
    using FileCallback = std::function<void(const std::string_view&, const std::string_view&)>;

private:
    static FileHandler* Instance;

    std::unordered_map<std::string, Texture*> m_extTex;

    FileHandler();
protected:

public:
    ~FileHandler();

    static void Init();
    static void Destroy();

    static void GetFileData(const std::string_view& a_filename, const std::string_view& a_ext, FileCallback*& a_callback, Texture*& a_texture);
};