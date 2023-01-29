#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string_view>

#if WIN32
#define FLARE_MONO_EXPORT(ret, func, ...) __declspec(dllexport) ret func(__VA_ARGS__)
#else
#define FLARE_MONO_EXPORT(ret, func, ...) static ret func(__VA_ARGS__)
#endif

class RuntimeManager
{
private:
    MonoDomain*   m_mainDomain;
  
    MonoDomain*   m_editorDomain;
    MonoAssembly* m_editorAssembly;
    MonoImage*    m_editorImage;
    MonoMethod*   m_editorUpdateMethod;
    MonoMethod*   m_editorUnloadMethod;

    MonoAssembly* m_engineAssembly;
    MonoImage*    m_engineImage;

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

    inline MonoDomain* GetEditorDomain() const
    {
        return m_editorDomain;
    }

    bool Build(const std::string_view& a_path, const std::string_view& a_name);

    void Start();

    void Update();

    void BindFunction(const std::string_view& a_location, void* a_function);
    void ExecFunction(const std::string_view& a_namespace, const std::string_view& a_class, const std::string_view& a_method, void** a_args) const;
};