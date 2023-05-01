#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Window.h"

#include <cstdint>
#include <glad/glad.h>

class RuntimeManager;
class ShaderProgram;

class EditorWindow : public Window
{
private:
    static uint32_t RefCount;
    static ShaderProgram* GridShader;

    RuntimeManager* m_runtime;

    glm::vec3       m_translation;
    glm::quat       m_rotation;

    uint32_t        m_width;
    uint32_t        m_height;

    glm::vec2       m_prevMousePos;

    float           m_scroll;

    GLuint          m_textureHandle;
    GLuint          m_depthTextureHandle;
    GLuint          m_framebufferHandle;

protected:

public:
    EditorWindow(RuntimeManager* a_runtime);
    virtual ~EditorWindow();

    virtual void Update(double a_delta);
};