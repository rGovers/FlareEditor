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
  
    MonoDomain*   m_editorDomain;
    MonoAssembly* m_editorAssembly;
    MonoMethod*   m_editorUpdateMethod;
    MonoMethod*   m_editorUnloadMethod;

    MonoDomain*   m_buildDomain;
    MonoAssembly* m_buildAssembly;
    MonoMethod*   m_buildMethod;

    bool          m_built;
protected:

public:
    RuntimeManager();
    ~RuntimeManager();

    inline bool IsBuilt() const
    {
        return m_built;
    }

    bool Build(const std::string_view& a_path, const std::string_view& a_name);

    void Start();

    void Update();
};