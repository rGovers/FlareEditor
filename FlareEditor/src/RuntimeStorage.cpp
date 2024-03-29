#include "Runtime/RuntimeStorage.h"

#include <stb_image.h>

#include "AssetLibrary.h"
#include "Flare/ColladaLoader.h"
#include "Flare/OBJLoader.h"
#include "Model.h"
#include "PixelShader.h"
#include "Runtime/RuntimeManager.h"
#include "ShaderGenerator.h"
#include "ShaderStorage.h"
#include "Texture.h"
#include "VertexShader.h"

static RuntimeStorage* Instance = nullptr;

#define RUNTIMESTORAGE_RUNTIME_ATTACH(ret, namespace, klass, name, code, ...) a_runtime->BindFunction(RUNTIME_FUNCTION_STRING(namespace, klass, name), (void*)RUNTIME_FUNCTION_NAME(klass, name));

#define RUNTIMESTORAGE_BINDING_FUNCTION_TABLE(F) \
    F(void, FlareEngine.Rendering, VertexShader, DestroyShader, { Instance->DestroyVertexShader(a_addr); }, uint32_t a_addr) \
    F(void, FlareEngine.Rendering, PixelShader, DestroyShader, { Instance->DestroyPixelShader(a_addr); }, uint32_t a_addr) \
    \
    F(FlareBase::RenderProgram, FlareEngine.Rendering, Material, GetProgramBuffer, { return Instance->GetRenderProgram(a_addr); }, uint32_t a_addr) \
    F(void, FlareEngine.Rendering, Material, SetProgramBuffer, { Instance->SetRenderProgram(a_addr, a_program); }, uint32_t a_addr, FlareBase::RenderProgram a_program) \
    F(void, FlareEngine.Rendering, Material, SetTexture, { Instance->SetProgramTexture(a_addr, a_slot, a_samplerAddr); }, uint32_t a_addr, uint32_t a_slot, uint32_t a_samplerAddr) \
    \
    F(void, FlareEngine.Rendering, Texture, DestroyTexture, { Instance->DestroyTexture(a_addr); }, uint32_t a_addr) \
    \
    /* Have not had to but I dont not believe there is a way to seperate the texture and sampler in OpenGL */ \
    F(uint32_t, FlareEngine.Rendering, TextureSampler, GenerateTextureSampler, { return a_texture; }, uint32_t a_texture, uint32_t a_filter, uint32_t a_addressMode) \
    F(uint32_t, FlareEngine.Rendering, TextureSampler, GenerateRenderTextureSampler, { return 0; }, uint32_t a_renderTexture, uint32_t a_textureIndex, uint32_t a_filter, uint32_t a_addressMode) \
    F(uint32_t, FlareEngine.Rendering, TextureSampler, GenerateRenderTextureDepthSampler, { return 0; }, uint32_t a_renderTexture, uint32_t a_filter, uint32_t a_addressMode) \
    F(void, FlareEngine.Rendering, TextureSampler, DestroySampler, { }, uint32_t a_addr) \
    \
    F(void, FlareEngine.Rendering, Model, DestroyModel, { Instance->DestroyModel(a_addr); }, uint32_t a_addr)

RUNTIMESTORAGE_BINDING_FUNCTION_TABLE(RUNTIME_FUNCTION_DEFINITION);

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(VertexShader, GenerateFromFile), MonoString* a_path)
{
    char* str = mono_string_to_utf8(a_path);

    const std::filesystem::path p = std::filesystem::path(str);

    mono_free(str);

    AssetLibrary* library = Instance->GetLibrary();
    if (p.extension() == ".fvert")
    {
        uint32_t size; 
        const char* dat;
        library->GetAsset(p, &size, &dat);

        const std::string s = GLSL_FromFShader(std::string_view(dat, size));

        return Instance->GenerateVertexShader(s);
    }
    else if (p.extension() == ".vert")
    {
        uint32_t size; 
        const char* dat;
        library->GetAsset(p, &size, &dat);

        return Instance->GenerateVertexShader(dat);
    }

    return -1;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(PixelShader, GenerateFromFile), MonoString* a_path)
{
    char* str = mono_string_to_utf8(a_path);

    const std::filesystem::path p = std::filesystem::path(str);

    mono_free(str);

    AssetLibrary* library = Instance->GetLibrary();
    if (p.extension() == ".fpix" || p.extension() == ".ffrag")
    {
        uint32_t size;
        const char* dat;

        library->GetAsset(p, &size, &dat);

        const std::string s = GLSL_FromFShader(std::string_view(dat, size));

        return Instance->GeneratePixelShader(s);
    }
    else if (p.extension() == ".pix" || p.extension() == ".frag")
    {
        uint32_t size; 
        const char* dat;
        library->GetAsset(p, &size, &dat);

        return Instance->GeneratePixelShader(dat);
    }

    return -1;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(Material, GenerateProgram), uint32_t a_vertexShader, uint32_t a_pixelShader, uint16_t a_vertexStride, MonoArray* a_attribute, MonoArray* a_shaderInputs, uint32_t a_cullMode, uint32_t a_primitiveMode, uint32_t a_enableColorBlending)
{
    FlareBase::RenderProgram program;
    program.VertexShader = a_vertexShader;
    program.PixelShader = a_pixelShader;
    program.VertexStride = a_vertexStride;
    program.CullingMode = (FlareBase::e_CullMode)a_cullMode;
    program.PrimitiveMode = (FlareBase::e_PrimitiveMode)a_primitiveMode;
    program.EnableColorBlending = (uint8_t)a_enableColorBlending;
    program.Data = nullptr;
    program.Flags = 0;

    program.VertexInputCount = 0;
    program.VertexAttribs = nullptr;
    program.ShaderBufferInputCount = 0;
    program.ShaderBufferInputs = nullptr;

    if (a_attribute != nullptr)
    {
        program.VertexInputCount = (uint16_t)mono_array_length(a_attribute);
        program.VertexAttribs = new FlareBase::VertexInputAttrib[program.VertexInputCount];

        for (uint16_t i = 0; i < program.VertexInputCount; ++i)
        {
            program.VertexAttribs[i] = mono_array_get(a_attribute, FlareBase::VertexInputAttrib, i);
        }
    }

    if (a_shaderInputs != nullptr)
    {
        program.ShaderBufferInputCount = (uint16_t)mono_array_length(a_shaderInputs);
        program.ShaderBufferInputs = new FlareBase::ShaderBufferInput[program.ShaderBufferInputCount];

        for (uint16_t i = 0; i < program.ShaderBufferInputCount; ++i)
        {
            program.ShaderBufferInputs[i] = mono_array_get(a_shaderInputs, FlareBase::ShaderBufferInput, i);
        }
    }

    return Instance->GenerateRenderProgram(program);
}
FLARE_MONO_EXPORT(void, RUNTIME_FUNCTION_NAME(Material, DestroyProgram), uint32_t a_addr)
{
    FlareBase::RenderProgram program = Instance->GetRenderProgram(a_addr);

    if (program.VertexAttribs != nullptr)
    {
        delete[] program.VertexAttribs;
    }

    if (program.ShaderBufferInputs != nullptr)
    {
        delete[] program.ShaderBufferInputs;
    }

    Instance->DestroyRenderProgram(a_addr);
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(Model, GenerateModel), MonoArray* a_vertices, MonoArray* a_indices, uint16_t a_vertexStride)
{
    const uint32_t vertexCount = (uint32_t)mono_array_length(a_vertices);
    const uint32_t indexCount = (uint32_t)mono_array_length(a_indices);

    const uint32_t vertexSize = vertexCount * a_vertexStride;

    char* vertices = new char[vertexSize];
    for (uint32_t i = 0; i < vertexSize; ++i)
    {
        vertices[i] = *mono_array_addr_with_size(a_vertices, 1, i);
    }

    uint32_t* indices = new uint32_t[indexCount];
    for (uint32_t i = 0; i < indexCount; ++i)
    {
        indices[i] = mono_array_get(a_indices, uint32_t, i);
    }

    const uint32_t addr = Instance->GenerateModel(vertices, vertexCount, indices, indexCount, a_vertexStride);

    delete[] vertices;
    delete[] indices;

    return addr;
}
FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(Model, GenerateFromFile), MonoString* a_path)
{
    char* str = mono_string_to_utf8(a_path);

    const std::filesystem::path p = std::filesystem::path(str);

    mono_free(str);

    std::vector<FlareBase::Vertex> vertices;
    std::vector<uint32_t> indices;

    AssetLibrary* library = Instance->GetLibrary();
    if (p.extension() == ".obj")
    {
        const char* dat;
        uint32_t size;
        library->GetAsset(p, &size, &dat);

        if (dat != nullptr && size > 0 && FlareBase::OBJLoader_LoadData(dat, size, &vertices, &indices))
        {
            return Instance->GenerateModel(vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), sizeof(FlareBase::Vertex));
        }
    }
    else if (p.extension() == ".dae")
    {
        const char* dat;
        uint32_t size;
        library->GetAsset(p, &size, &dat);

        if (dat != nullptr && size > 0 && FlareBase::ColladaLoader_LoadData(dat, size, &vertices, &indices))
        {
            return Instance->GenerateModel(vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), sizeof(FlareBase::Vertex));
        }
    }

    return -1;
}

FLARE_MONO_EXPORT(uint32_t, RUNTIME_FUNCTION_NAME(Texture, GenerateFromFile), MonoString* a_path)
{
    char* str = mono_string_to_utf8(a_path);
    const std::filesystem::path p = std::filesystem::path(str);
    mono_free(str);

    AssetLibrary* library = Instance->GetLibrary();
    if (p.extension() == ".png")
    {
        const char* dat;
        uint32_t size;

        if (dat != nullptr && size > 0)
        {
            int width;
            int height;
            int channels;

            const stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)dat, (int)size, &width, &height, &channels, STBI_rgb_alpha);

            return Instance->GenerateTexture((uint32_t)width, (uint32_t)height, (unsigned char*)pixels);
        }
    }

    return -1;
}

RuntimeStorage::RuntimeStorage(RuntimeManager* a_runtime, AssetLibrary* a_assets)
{
    m_assets = a_assets;
    m_runtime = a_runtime;

    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, VertexShader, GenerateFromFile);
    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, PixelShader, GenerateFromFile);

    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Material, GenerateProgram);
    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Material, DestroyProgram);

    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Model, GenerateModel);
    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Model, GenerateFromFile);

    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Texture, GenerateFromFile);

    RUNTIMESTORAGE_BINDING_FUNCTION_TABLE(RUNTIMESTORAGE_RUNTIME_ATTACH);

    Instance = this;
}
RuntimeStorage::~RuntimeStorage()
{
    Clear();
}

void RuntimeStorage::Clear()
{
    for (Model* mdl : m_models)
    {
        if (mdl != nullptr)
        {
            delete mdl;
        }
    }
    m_models.clear();

    for (VertexShader* v : m_vertexShaders)
    {
        if (v != nullptr)
        {
            delete v;
        }
    }
    m_vertexShaders.clear();

    for (PixelShader* p : m_pixelShaders)
    {
        if (p != nullptr)
        {
            delete p;
        }
    }
    m_pixelShaders.clear();
}

uint32_t RuntimeStorage::GenerateVertexShader(const std::string_view& a_str)
{
    VertexShader* vShader = VertexShader::GenerateShader(a_str);
    if (vShader == nullptr)
    {
        return -1;
    }

    const uint32_t shaderCount = (uint32_t)m_vertexShaders.size();
    for (uint32_t i = 0; i < shaderCount; ++i)
    {
        if (m_vertexShaders[i] == nullptr)
        {
            m_vertexShaders[i] = vShader;

            return i;
        }
    }

    m_vertexShaders.emplace_back(vShader);

    return shaderCount;
}
void RuntimeStorage::DestroyVertexShader(uint32_t a_addr)
{
    delete m_vertexShaders[a_addr];
    m_vertexShaders[a_addr] = nullptr;
}

uint32_t RuntimeStorage::GeneratePixelShader(const std::string_view& a_str)
{
    PixelShader* pShader = PixelShader::GenerateShader(a_str);
    if (pShader == nullptr)
    {
        return -1;
    }

    const uint32_t shaderCount = (uint32_t)m_pixelShaders.size();
    for (uint32_t i = 0; i < shaderCount; ++i)
    {
        if (m_pixelShaders[i] == nullptr)
        {
            m_pixelShaders[i] = pShader;

            return i;
        }
    }

    m_pixelShaders.emplace_back(pShader);

    return shaderCount;
}
void RuntimeStorage::DestroyPixelShader(uint32_t a_addr)
{
    delete m_pixelShaders[a_addr];
    m_pixelShaders[a_addr] = nullptr;
}

uint32_t RuntimeStorage::GenerateRenderProgram(const FlareBase::RenderProgram& a_program)
{
    FlareBase::RenderProgram program = a_program;
    ShaderStorage* storage = new ShaderStorage(this);
    program.Data = storage;

    const uint32_t programCount = (uint32_t)m_renderPrograms.size();
    for (uint32_t i = 0; i < programCount; ++i)
    {
        if (m_renderPrograms[i].Flags & 0b1 << FlareBase::RenderProgram::FreeFlag)
        {            
            m_renderPrograms[i] = program;

            return i;
        }
    }

    m_renderPrograms.emplace_back(program);

    return programCount;
}
void RuntimeStorage::SetProgramTexture(uint32_t a_addr, uint32_t a_slot, uint32_t a_textureAddr)
{
    const FlareBase::RenderProgram& program = m_renderPrograms[a_addr];

    ShaderStorage* storage = (ShaderStorage*)program.Data;
    storage->SetTexture(a_slot, m_textures[a_textureAddr]);
}
void RuntimeStorage::DestroyRenderProgram(uint32_t a_addr)
{
    FlareBase::RenderProgram& program = m_renderPrograms[a_addr];
    if (program.Flags & 0b1 << FlareBase::RenderProgram::DestroyFlag)
    {
        DestroyVertexShader(program.VertexShader);
        DestroyPixelShader(program.PixelShader);
    }

    delete (ShaderStorage*)program.Data;
    program.Data = nullptr;

    program.Flags = 0b1 << FlareBase::RenderProgram::FreeFlag;
}

uint32_t RuntimeStorage::GenerateModel(const void* a_vertices, uint32_t a_vertexCount, const uint32_t* a_indices, uint32_t a_indexCount, uint16_t a_vertexStride)
{
    Model* mdl = new Model(a_vertices, a_vertexCount, (GLuint*)a_indices, a_indexCount, a_vertexStride);

    const uint32_t modelCount = (uint32_t)m_models.size();
    for (uint32_t i = 0; i < modelCount; ++i)
    {
        if (m_models[i] == nullptr)
        {
            m_models[i] = mdl;

            return i;
        }
    }

    m_models.emplace_back(mdl);

    return modelCount;
}
void RuntimeStorage::DestroyModel(uint32_t a_addr)
{
    delete m_models[a_addr];
    m_models[a_addr] = nullptr;
}

uint32_t RuntimeStorage::GenerateTexture(uint32_t a_width, uint32_t a_height, const unsigned char* a_data)
{
    Texture* texture = new Texture(a_width, a_height, a_data);

    const uint32_t textureCount = (uint32_t)m_textures.size();
    for (uint32_t i = 0; i < textureCount; ++i)
    {
        if (m_textures[i] == nullptr)
        {
            m_textures[i] = texture;

            return i;
        }
    }

    m_textures.emplace_back(texture);

    return textureCount;
}
void RuntimeStorage::DestroyTexture(uint32_t a_addr)
{
    delete m_textures[a_addr];
    m_textures[a_addr] = nullptr;
}