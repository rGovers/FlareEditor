#include "RuntimeManager.h"

#include <assert.h>
#include <chrono>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <string>

#include "Logger.h"

static void Logger_PushMessage(MonoString* a_string)
{
    char* str = mono_string_to_utf8(a_string);

    Logger::Message(str);

    mono_free(str);
}
static void Logger_PushWarning(MonoString* a_string)
{
    char* str = mono_string_to_utf8(a_string);

    Logger::Warning(str);

    mono_free(str);
}
static void Logger_PushError(MonoString* a_string)
{
    char* str = mono_string_to_utf8(a_string);

    Logger::Error(str);

    mono_free(str);
}

RuntimeManager::RuntimeManager()
{
    mono_config_parse(NULL);

    mono_set_dirs("./lib", "./etc");

    m_mainDomain = mono_jit_init_version("FlareCS", "v4.0");

    m_buildDomain = mono_domain_create_appdomain("FlareEditorBuildEngine", NULL);
    assert(m_buildDomain != nullptr);
    mono_domain_set(m_buildDomain, 0);
    m_buildAssembly = mono_domain_assembly_open(m_buildDomain, "./FlareEditorBuildEngine.dll");
    assert(m_buildAssembly != nullptr);
    MonoImage* image = mono_assembly_get_image(m_buildAssembly);
    MonoClass* buildProgramClass = mono_class_from_name(image, "FlareEditor.BuildEngine", "Program");

    MonoMethodDesc* buildDesc = mono_method_desc_new(":Build(string,string)", 0);
    m_buildMethod = mono_method_desc_search_in_class(buildDesc, buildProgramClass);

    mono_method_desc_free(buildDesc);

    mono_add_internal_call("FlareEditor.BuildEngine.Logger::PushMessage", (void*)Logger_PushMessage);
    mono_add_internal_call("FlareEditor.BuildEngine.Logger::PushWarning", (void*)Logger_PushWarning);
    mono_add_internal_call("FlareEditor.BuildEngine.Logger::PushError", (void*)Logger_PushError);
}
RuntimeManager::~RuntimeManager()
{
    mono_jit_cleanup(m_mainDomain);
}

bool RuntimeManager::Build(const std::string_view& a_path, const std::string_view& a_name)
{
    const std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();

    MonoString* pathString = mono_string_new(m_buildDomain, a_path.data());
    MonoString* nameString = mono_string_new(m_buildDomain, a_name.data());

    void* args[2];
    args[0] = pathString;
    args[1] = nameString;

    MonoObject* retVal = mono_runtime_invoke(m_buildMethod, NULL, args, NULL);
    const mono_bool retValBool = *(bool*)mono_object_unbox(retVal);

    if (retValBool == 0)
    {
        Logger::Error("Failed to build project");

        return false;
    }

    const std::chrono::time_point endTime = std::chrono::high_resolution_clock::now();

    const double buildTime = std::chrono::duration<double>(endTime - startTime).count();

    Logger::Message("Built Project in " + std::to_string(buildTime) + "s");

    return true;
}

bool RuntimeManager::Start()
{
    
}