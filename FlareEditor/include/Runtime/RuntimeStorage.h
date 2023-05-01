#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

class AssetLibrary;
class Model;
class PixelShader;
class RuntimeManager;
class VertexShader;

class RuntimeStorage
{
private:
    AssetLibrary*              m_assets;
    RuntimeManager*            m_runtime;

    std::vector<Model*>        m_models;

    std::vector<VertexShader*> m_vertexShader;
    std::vector<PixelShader*>  m_pixelShader;

protected:

public:
    RuntimeStorage(RuntimeManager* a_runtime, AssetLibrary* a_assets);
    ~RuntimeStorage();

    inline AssetLibrary* GetLibrary() const
    {
        return m_assets;
    }

    uint32_t GenerateModel(const void* a_vertices, uint32_t a_vertexCount, const uint32_t* a_indices, uint32_t a_indexCount, uint16_t a_vertexStride);
    void DestroyModel(uint32_t a_addr);

    void Clear();
};