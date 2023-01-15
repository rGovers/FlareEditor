#include "Workspace.h"

#include "Runtime/RuntimeManager.h"

Workspace::Workspace(RuntimeManager* a_runtime)
{
    m_runtime = a_runtime;
}
Workspace::~Workspace()
{
    
}

void Workspace::OpenDef(const std::filesystem::path& a_path, uint32_t a_size, const char* a_data)
{
    MonoDomain* editorDomain = m_runtime->GetEditorDomain();

    MonoString* pathString = mono_string_from_utf32((mono_unichar4*)a_path.u32string().c_str());

    void* args[] =
    {
        pathString
    };

    m_runtime->ExecFunction("FlareEditor", "AssetProperties", ":PushDef(string)", args);
}