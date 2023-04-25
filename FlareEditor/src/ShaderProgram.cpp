#include "ShaderProgram.h"

#include "Logger.h"
#include "PixelShader.h"
#include "VertexShader.h"

ShaderProgram::ShaderProgram(const VertexShader* a_vertexShader, const PixelShader* a_pixelShader)
{
    m_handle = glCreateProgram();

    if (a_vertexShader != nullptr)
    {
        glAttachShader(m_handle, a_vertexShader->GetHandle());
    }

    if (a_pixelShader != nullptr)
    {
        glAttachShader(m_handle, a_pixelShader->GetHandle());
    }

    glLinkProgram(m_handle);

    GLint success;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint len;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &len);

        char* buff = new char[len];
        glGetProgramInfoLog(m_handle, (GLsizei)len, NULL, buff);

        Logger::Error(buff);
    }

    if (a_vertexShader != nullptr)
    {
        glDetachShader(m_handle, a_vertexShader->GetHandle());
    }

    if (a_pixelShader != nullptr)
    {
        glDetachShader(m_handle, a_pixelShader->GetHandle());
    }
}
ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_handle);
}