#include "Runtime/RuntimeStorage.h"

#include "AssetLibrary.h"
#include "ColladaLoader.h"
#include "Model.h"
#include "OBJLoader.h"
#include "Runtime/RuntimeManager.h"
#include "Vertices.h"

static RuntimeStorage* Instance = nullptr;

#define RUNTIMESTORAGE_RUNTIME_ATTACH(ret, namespace, klass, name, code, ...) a_runtime->BindFunction(RUNTIME_FUNCTION_STRING(namespace, klass, name), (void*)RUNTIME_FUNCTION_NAME(klass, name));

#define RUNTIMESTORAGE_BINDING_FUNCTION_TABLE(F) \
    F(void, FlareEngine.Rendering, Model, DestroyModel, { Instance->DestroyModel(a_addr); }, uint32_t a_addr)

RUNTIMESTORAGE_BINDING_FUNCTION_TABLE(RUNTIME_FUNCTION_DEFINITION);

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

    uint32_t addr = -1;

    std::vector<FlareBase::Vertex> vertices;
    std::vector<uint32_t> indices;

    const std::filesystem::path p = std::filesystem::path(str);

    AssetLibrary* library = Instance->GetLibrary();
    if (p.extension() == ".obj")
    {
        const char* dat;
        uint32_t size;
        library->GetAsset(p, &size, &dat);

        if (FlareBase::OBJLoader_LoadData(dat, size, &vertices, &indices))
        {
            addr = Instance->GenerateModel(vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), sizeof(FlareBase::Vertex));
        }
    }
    else if (p.extension() == ".dae")
    {
        const char* dat;
        uint32_t size;
        library->GetAsset(p, &size, &dat);
    }

    mono_free(str);

    return addr;
}

RuntimeStorage::RuntimeStorage(RuntimeManager* a_runtime, AssetLibrary* a_assets)
{
    m_assets = a_assets;
    m_runtime = a_runtime;

    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Model, GenerateModel);
    BIND_FUNCTION(a_runtime, FlareEngine.Rendering, Model, GenerateFromFile);

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