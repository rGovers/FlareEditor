#pragma once

#include <glad/glad.h>
#include <string>

class PixelShader
{
private:
    GLuint m_handle;
    
protected:

public:
    PixelShader(const std::string_view& a_str);
    ~PixelShader();

    inline GLuint GetHandle() const
    {
        return m_handle;
    }
};