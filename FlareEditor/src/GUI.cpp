#include "GUI.h"

#include <glm/glm.hpp>
#include <string>

#include "FlareImGui.h"
#include "Logger.h"
#include "Runtime/RuntimeManager.h"

static GUI* Instance = nullptr;

static constexpr uint32_t BufferSize = 4096;

FLARE_MONO_EXPORT(MonoString*, GUI_GetDef, MonoString* a_str, MonoString* a_preview, MonoString* a_value)
{
    char* str = mono_string_to_utf8(a_str);
    char* preview = mono_string_to_utf8(a_preview);
    char* val = mono_string_to_utf8(a_value);

    MonoString* outBuff = nullptr;
    if (ImGui::Button(preview))
    {
        Logger::Error("Not implemented use drag and drop");
    }
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DefPath");
        if (payload != nullptr)
        {
            const RuntimeManager* runtime = Instance->GetRuntime();
            outBuff = mono_string_from_utf32((mono_unichar4*)payload->Data);
        } 

        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    ImGui::Text(str);

    return outBuff;
}

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
        const RuntimeManager* runtime = Instance->GetRuntime();

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

FLARE_MONO_EXPORT(uint32_t, GUI_ResetButton, MonoString* a_str)
{
    char* str = mono_string_to_utf8(a_str);

    bool ret = FlareImGui::ImageButton(str, "Textures/Icons/Icon_Reset.png", glm::vec2(16.0f));

    ImGui::SameLine();

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(void, GUI_Indent)
{
    ImGui::Indent();
}
FLARE_MONO_EXPORT(void, GUI_Unindent)
{
    ImGui::Unindent();
}

FLARE_MONO_EXPORT(void, GUI_Tooltip, MonoString* a_title, MonoString* a_str)
{
    char* title = mono_string_to_utf8(a_title);
    char* str = mono_string_to_utf8(a_str);

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();

        if (title != nullptr)
        {
            ImGui::Text(title);

            if (str != nullptr)
            {
                ImGui::Separator();
            }
        }

        if (str != nullptr)
        {
            ImGui::Text(str);
        }

        ImGui::EndTooltip();
    }
    
    mono_free(title);
    mono_free(str);
}

FLARE_MONO_EXPORT(uint32_t, GUI_ShowStructView, MonoString* a_str)
{
    bool ret = false;

    char* str = mono_string_to_utf8(a_str);
    if (ImGui::CollapsingHeader(str))
    {
        ret = true;
    }

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, GUI_ShowArrayView, MonoString* a_str, uint32_t* a_addValue)
{
    bool ret = false;

    char* str = mono_string_to_utf8(a_str);

    ImGui::PushID((std::string(str) + "_Add").c_str());
    *a_addValue = (uint32_t)ImGui::Button("+");
    ImGui::PopID();

    ImGui::SameLine();
    
    if (ImGui::CollapsingHeader(str))
    {
        ret = true;
    }   

    mono_free(str);

    return (uint32_t)ret;
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

        a_runtime->BindFunction("FlareEditor.GUI::GetDef", (void*)GUI_GetDef);

        a_runtime->BindFunction("FlareEditor.GUI::GetInt", (void*)GUI_GetInt);
        a_runtime->BindFunction("FlareEditor.GUI::GetUInt", (void*)GUI_GetUInt);

        a_runtime->BindFunction("FlareEditor.GUI::GetFloat", (void*)GUI_GetFloat);
        a_runtime->BindFunction("FlareEditor.GUI::GetVec2", (void*)GUI_GetVec2);
        a_runtime->BindFunction("FlareEditor.GUI::GetVec3", (void*)GUI_GetVec3);
        a_runtime->BindFunction("FlareEditor.GUI::GetVec4", (void*)GUI_GetVec4);

        a_runtime->BindFunction("FlareEditor.GUI::GetString", (void*)GUI_GetString);
        a_runtime->BindFunction("FlareEditor.GUI::GetStringList", (void*)GUI_GetSringList);

        a_runtime->BindFunction("FlareEditor.GUI::ResetButton", (void*)GUI_ResetButton);

        a_runtime->BindFunction("FlareEditor.GUI::Indent", (void*)GUI_Indent);
        a_runtime->BindFunction("FlareEditor.GUI::Unindent", (void*)GUI_Unindent);

        a_runtime->BindFunction("FlareEditor.GUI::ShowStructView", (void*)GUI_ShowStructView);
        a_runtime->BindFunction("FlareEditor.GUI::ShowArrayView", (void*)GUI_ShowArrayView);

        a_runtime->BindFunction("FlareEditor.GUI::Tooltip", (void*)GUI_Tooltip);
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