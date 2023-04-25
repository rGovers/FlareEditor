#include "PixelShader.h"

#include "Logger.h"

PixelShader::PixelShader(const std::string_view& a_str)
{
    m_handle = glCreateShader(GL_FRAGMENT_SHADER);

    const char* d = a_str.data();
    const GLint len = (GLint)a_str.size();

    glShaderSource(m_handle, 1, &d, &len);
    glCompileShader(m_handle);

    GLint success;
    glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint logSize = 0;
        glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &logSize);

        char* buffer = new char[logSize];
        glGetShaderInfoLog(m_handle, (GLsizei)logSize, NULL, buffer);

        Logger::Error(buffer);

        delete[] buffer;
    }
}
PixelShader::~PixelShader()
{
    glDeleteShader(m_handle);
}