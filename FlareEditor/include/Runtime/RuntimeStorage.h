#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

class AssetLibrary;
class Model;
class PixelShader;
class RuntimeManager;
class VertexShader;

#include "Flare/RenderProgram.h"

class RuntimeStorage
{
private:
    AssetLibrary*                         m_assets;
    RuntimeManager*                       m_runtime;
    
    std::vector<Model*>                   m_models;
    
    std::vector<VertexShader*>            m_vertexShaders;
    std::vector<PixelShader*>             m_pixelShaders;

    std::vector<FlareBase::RenderProgram> m_renderPrograms;

protected:

public:
    RuntimeStorage(RuntimeManager* a_runtime, AssetLibrary* a_assets);
    ~RuntimeStorage();

    inline AssetLibrary* GetLibrary() const
    {
        return m_assets;
    }

    uint32_t GenerateVertexShader(const std::string_view& a_str);
    void DestroyVertexShader(uint32_t a_addr);

    uint32_t GeneratePixelShader(const std::string_view& a_str);
    void DestroyPixelShader(uint32_t a_addr);

    uint32_t GenerateRenderProgram(const FlareBase::RenderProgram& a_program);
    void DestroyRenderProgram(uint32_t a_addr);
    inline FlareBase::RenderProgram GetRenderProgram(uint32_t a_addr) const
    {
        return m_renderPrograms[a_addr];
    }
    inline void SetRenderProgram(uint32_t a_addr, const FlareBase::RenderProgram& a_program) 
    {
        m_renderPrograms[a_addr] = a_program;
    }

    uint32_t GenerateModel(const void* a_vertices, uint32_t a_vertexCount, const uint32_t* a_indices, uint32_t a_indexCount, uint16_t a_vertexStride);
    void DestroyModel(uint32_t a_addr);

    void Clear();
};