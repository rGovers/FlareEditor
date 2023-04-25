#pragma once

#include <glad/glad.h>

class VertexShader;
class PixelShader;

class ShaderProgram
{
private:
    GLuint m_handle;

protected:

public:
    ShaderProgram(const VertexShader* a_vertexShader, const PixelShader* a_pixelShader);
    ~ShaderProgram();

    inline GLuint GetHandle() const
    {
        return m_handle;
    }
};