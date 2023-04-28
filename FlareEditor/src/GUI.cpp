#include "GUI.h"

#include <glm/glm.hpp>
#include <string>

#include "FlareImGui.h"
#include "Logger.h"
#include "Runtime/RuntimeManager.h"

static GUI* Instance = nullptr;

static constexpr uint32_t BufferSize = 4096;

struct IDStack
{
    IDStack(const std::string_view& a_id)
    {
        ImGui::PushID(a_id.data());
    }
    ~IDStack()
    {
        ImGui::PopID();
    }
};

#define STACK_ID(str) const IDStack idStackTVal = IDStack(str)
#define STACK_G_ID(str) STACK_ID(Instance->GetID() + (str))

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetCheckbox), MonoString* a_str, uint32_t* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    bool b = (bool)*a_value;

    STACK_G_ID(str);
    bool ret = false;
    if (ImGui::Checkbox(str, &b))
    {
        ret = true;
        *a_value = (uint32_t)b;
    }

    return ret;
}
FLARE_MONO_EXPORT(MonoString*, RUNTIME_FUNCTION_NAME(GUI, GetDef), MonoString* a_str, MonoString* a_preview, MonoString* a_value)
{
    char* str = mono_string_to_utf8(a_str);
    char* preview = mono_string_to_utf8(a_preview);
    char* val = mono_string_to_utf8(a_value);

    MonoString* outBuff = nullptr;
    STACK_G_ID(str);
    if (ImGui::Button(preview))
    {
        Logger::Error("Not implemented use drag and drop");
    }
    
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DefPath");
        if (payload != nullptr)
        {
            outBuff = mono_string_from_utf32((mono_unichar4*)payload->Data);
        } 

        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    ImGui::Text(str);

    return outBuff;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetInt), MonoString* a_str, int32_t* a_value)
{
    char* str = mono_string_to_utf8(a_str);
    
    STACK_G_ID(str);
    const bool ret = ImGui::InputInt(str, (int*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetUInt), MonoString* a_str, uint32_t* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    const bool ret = ImGui::InputInt(str, (int*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetFloat), MonoString* a_str, float* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    const bool ret = ImGui::DragFloat(str, a_value);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetVec2), MonoString* a_str, glm::vec2* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    const bool ret = ImGui::DragFloat2(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetVec3), MonoString* a_str, glm::vec3* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    const bool ret = ImGui::DragFloat3(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetVec4), MonoString* a_str, glm::vec4* a_value)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    const bool ret = ImGui::DragFloat4(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetColor), MonoString* a_str, glm::vec4* a_value)
{
    char* str = mono_string_to_utf8(a_str);
    
    STACK_G_ID(str);
    const bool ret = ImGui::ColorEdit4(str, (float*)a_value);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(MonoString*, RUNTIME_FUNCTION_NAME(GUI, GetString), MonoString* a_str, MonoString* a_value)
{
    char buffer[BufferSize];

    char* label = mono_string_to_utf8(a_str);
    char* str = mono_string_to_utf8(a_value);
    
    MonoString* outBuff = nullptr;

    STACK_G_ID(str);
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

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetStringList), MonoString* a_str, MonoArray* a_list, int32_t* a_selected)
{
    char* str = mono_string_to_utf8(a_str);
    const uintptr_t size = mono_array_length(a_list);
    if (*a_selected >= size)
    {
        *a_selected = 0;
    }

    char* selectedStr = mono_string_to_utf8(mono_array_get(a_list, MonoString*, *a_selected));

    bool ret = false;
    bool combo = false;

    {
        STACK_G_ID(str);

        if (ImGui::BeginCombo(str, selectedStr))
        {
            for (int32_t i = 0; i < size; ++i)
            {
                const bool selected = i == *a_selected;

                char* selectableStr = mono_string_to_utf8(mono_array_get(a_list, MonoString*, i));

                STACK_G_ID(std::string(str) + "[" + std::to_string(i) + "]");
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
    }   

    mono_free(str);

    return ret;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, ResetButton), MonoString* a_str)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    bool ret = FlareImGui::ImageButton(str, "Textures/Icons/Icon_Reset.png", glm::vec2(16.0f));

    ImGui::SameLine();

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, NIndent))
{
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
}
FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, Indent))
{
    ImGui::Indent();
}
FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, Unindent))
{
    ImGui::Unindent();
}

FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, Tooltip), MonoString* a_title, MonoString* a_str)
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

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, ShowStructView), MonoString* a_str)
{
    bool ret = false;

    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    if (ImGui::CollapsingHeader(str))
    {
        ret = true;
    }

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, ShowArrayView), MonoString* a_str, uint32_t* a_addValue)
{
    bool ret = false;

    char* str = mono_string_to_utf8(a_str);

    {
        STACK_G_ID(std::string(str) + "_Add");
        *a_addValue = (uint32_t)ImGui::Button("+");
    }
    

    ImGui::SameLine();
    
    STACK_G_ID(str);
    if (ImGui::CollapsingHeader(str))
    {
        ret = true;
    }   

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, PushID), MonoString* a_str)
{
    char* str = mono_string_to_utf8(a_str);

    Instance->PushID(str);

    mono_free(str);
}
FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, PopID))
{
    Instance->PopID();
}

FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, Label), MonoString* a_str)
{
    char* str = mono_string_to_utf8(a_str);

    ImGui::Text(str);

    mono_free(str);
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, GetSelectable), MonoString* a_str)
{
    char* str = mono_string_to_utf8(a_str);

    STACK_G_ID(str);
    const bool ret = ImGui::Selectable(str);

    mono_free(str);

    return (uint32_t)ret;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(GUI, NodeI), MonoString* a_str)
{
    char* str = mono_string_to_utf8(a_str);
    
    STACK_G_ID(str);
    const bool ret = ImGui::TreeNode(str);

    mono_free(str);

    return (uint32_t)ret;
}
FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, PopNode))
{
    ImGui::TreePop();
}
FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(GUI, SameLine))
{
    ImGui::SameLine();
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
        
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetCheckbox);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetDef);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetInt);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetUInt);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetFloat);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetVec2);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetVec3);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetVec4);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetColor);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetString);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetStringList);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, ResetButton);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, NIndent);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, Indent);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, Unindent);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, ShowStructView);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, ShowArrayView);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, Tooltip);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, PushID);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, PopID);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, Label);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, GetSelectable);

        BIND_FUNCTION(a_runtime, FlareEditor, GUI, NodeI);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, PopNode);
        BIND_FUNCTION(a_runtime, FlareEditor, GUI, SameLine);
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

std::string GUI::GetID() const
{
    std::string str;

    for (const std::string& s : Instance->m_id)
    {
        str += s;
    }

    return str;
}