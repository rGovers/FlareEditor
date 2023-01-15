#include "GUI.h"

#include <glm/glm.hpp>
#include <imgui.h>

#include "Runtime/RuntimeManager.h"

static GUI* Instance = nullptr;

static constexpr uint32_t BufferSize = 4096;

FLARE_MONO_EXPORT(uint32_t, GUI_GetInt, MonoString* a_str, int32_t* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    const bool ret = ImGui::InputInt(str, (int*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, GUI_GetUInt, MonoString* a_str, uint32_t* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    const bool ret = ImGui::InputInt(str, (int*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(uint32_t, GUI_GetFloat, MonoString* a_str, float* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    const bool ret = ImGui::DragFloat(str, a_value);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(uint32_t, GUI_GetVec2, MonoString* a_str, glm::vec2* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    const bool ret = ImGui::DragFloat2(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, GUI_GetVec3, MonoString* a_str, glm::vec3* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    const bool ret = ImGui::DragFloat3(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, GUI_GetVec4, MonoString* a_str, glm::vec4* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    const bool ret = ImGui::DragFloat4(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(MonoString*, GUI_GetString, MonoString* a_str, MonoString* a_value)
{
    char buffer[BufferSize];

    char* label = mono_string_to_utf8(a_str);
    char* str = mono_string_to_utf8(a_value);
    
    MonoString* outBuff = nullptr;

    strncpy(buffer, str, BufferSize - 1);
    if (ImGui::InputText(label, buffer, BufferSize))
    {
        RuntimeManager* runtime = Instance->GetRuntime();

        outBuff = mono_string_new(runtime->GetEditorDomain(), buffer);
    }

    mono_free(label);
    mono_free(str);

    return outBuff;
}

FLARE_MONO_EXPORT(uint32_t, GUI_GetSringList, MonoString* a_str, MonoArray* a_list, int32_t* a_selected)
{
    char* str = mono_string_to_utf8(a_str);
    const uintptr_t size = mono_array_length(a_list);
    if (*a_selected >= size)
    {
        *a_selected = 0;
    }

    char* selectedStr = mono_string_to_utf8(mono_array_get(a_list, MonoString*, *a_selected));

    bool ret = false;
    if (ImGui::BeginCombo(str, selectedStr))
    {
        for (int32_t i = 0; i < size; ++i)
        {
            const bool selected = i == *a_selected;

            char* selectableStr = mono_string_to_utf8(mono_array_get(a_list, MonoString*, i));

            if (ImGui::Selectable(selectableStr, selected))
            {
                *a_selected = i;

                ret = true;
            }

            if (selected)
            {
                ImGui::SetItemDefaultFocus();
            }

            mono_free(selectableStr);
        }

        ImGui::EndCombo();
    }

    mono_free(str);

    return ret;
}

GUI::GUI(RuntimeManager* a_runtime)
{
    m_runtime = a_runtime;
}
GUI::~GUI()
{
    
}

void GUI::Init(RuntimeManager* a_runtime)
{
    if (Instance == nullptr)
    {
        Instance = new GUI(a_runtime);

        a_runtime->BindFunction("FlareEditor.GUI::GetInt", (void*)GUI_GetInt);
        a_runtime->BindFunction("FlareEditor.GUI::GetUInt", (void*)GUI_GetUInt);

        a_runtime->BindFunction("FlareEditor.GUI::GetFloat", (void*)GUI_GetFloat);
        a_runtime->BindFunction("FlareEditor.GUI::GetVec2", (void*)GUI_GetVec2);
        a_runtime->BindFunction("FlareEditor.GUI::GetVec3", (void*)GUI_GetVec3);
        a_runtime->BindFunction("FlareEditor.GUI::GetVec4", (void*)GUI_GetVec4);

        a_runtime->BindFunction("FlareEditor.GUI::GetString", (void*)GUI_GetString);
        a_runtime->BindFunction("FlareEditor.GUI::GetStringList", (void*)GUI_GetSringList);
    }
}
void GUI::Destroy()
{
    if (Instance != nullptr)
    {
        delete Instance;
        Instance = nullptr;
    }
}