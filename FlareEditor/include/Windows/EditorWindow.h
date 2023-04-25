#pragma once

#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Window.h"

#include <cstdint>
#include <glad/glad.h>

class EditorWindow : public Window
{
private:
    glm::vec3 m_translation;
    glm::quat m_rotation;

    uint32_t  m_width;
    uint32_t  m_height;

    GLuint    m_textureHandle;
    GLuint    m_depthTextureHandle;
    GLuint    m_framebufferHandle;

protected:

public:
    EditorWindow();
    virtual ~EditorWindow();

    virtual void Update();
};