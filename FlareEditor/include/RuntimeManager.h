#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string_view>

class RuntimeManager
{
private:
    MonoDomain*   m_mainDomain;
  
    MonoDomain*   m_buildDomain;
    MonoAssembly* m_buildAssembly;
    MonoMethod*   m_buildMethod;
protected:

public:
    RuntimeManager();
    ~RuntimeManager();

    bool Build(const std::string_view& a_path, const std::string_view& a_name);

    bool Start();
};