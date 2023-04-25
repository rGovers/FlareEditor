#pragma once

#include <glad/glad.h>
#include <string>

class VertexShader
{
private:
    GLuint m_handle;
    
protected:

public:
    VertexShader(const std::string_view& a_str);
    ~VertexShader();

    inline GLuint GetHandle() const
    {
        return m_handle;
    }
};