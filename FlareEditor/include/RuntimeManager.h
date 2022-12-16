#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string_view>

#if WIN32
#define FLARE_MONO_EXPORT(ret, func, args) __declspec(dllexport) ret func(args)
#else
#define FLARE_MONO_EXPORT(ret, func, args) static ret func(args)
#endif

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