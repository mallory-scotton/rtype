///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/URessource.hpp>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
URessource::URessource(void)
    : m_graphicsFactory(nullptr)
    , m_isShuttingDown(false)
{}

///////////////////////////////////////////////////////////////////////////////
URessource::~URessource()
{
    BeginShutdown();

    // Clear all resources in proper order
    m_shaders.clear();
    m_textures.clear();
    m_assets.clear();
    m_pakFiles.clear();

    m_graphicsFactory = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
void URessource::BeginShutdown(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isShuttingDown = true;
}

///////////////////////////////////////////////////////////////////////////////
SizeT URessource::CleanupUnusedResources(void)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    SizeT cleaned = 0;

    // Remove expired texture references
    for (auto it = m_textures.begin(); it != m_textures.end();)
    {
        if (it->second.expired())
        {
            it = m_textures.erase(it);
            ++cleaned;
        }
        else { ++it; }
    }

    // Remove expired shader references
    for (auto it = m_shaders.begin(); it != m_shaders.end();)
    {
        if (it->second.expired())
        {
            it = m_shaders.erase(it);
            ++cleaned;
        }
        else { ++it; }
    }

    return cleaned;
}

///////////////////////////////////////////////////////////////////////////////
void URessource::SetGraphicsFactory(IGraphicsFactory* factory)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_graphicsFactory = factory;
}

///////////////////////////////////////////////////////////////////////////////
UAsset* URessource::LoadAsset(const FilePath& path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isShuttingDown) { return nullptr; }

    UAsset asset(path);
    if (!asset.Load()) { return nullptr; }

    const FString& uuid = asset.GetUUID();
    auto it = m_assets.find(uuid);
    if (it != m_assets.end()) { return it->second.get(); }

    m_assets[uuid] = std::make_unique<UAsset>(std::move(asset));
    return m_assets[uuid].get();
}

///////////////////////////////////////////////////////////////////////////////
UAsset* URessource::GetAsset(const FString& uuid) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto it = m_assets.find(uuid);
    if (it != m_assets.end()) { return it->second.get(); }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle URessource::LoadTexture(
    const TVariant<FilePath, FString, UAsset*>& source, const FRectanglei& area
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isShuttingDown || !m_graphicsFactory) { return FTextureHandle(); }

    FString id;
    std::shared_ptr<ITexture> texture;

    // Handle FilePath source
    if (std::holds_alternative<FilePath>(source))
    {
        const FilePath& path = std::get<FilePath>(source);
        id = path.string().c_str();

        // Check if already loaded
        auto it = m_textures.find(id);
        if (it != m_textures.end())
        {
            if (auto existing = it->second.lock())
            {
                return FTextureHandle(existing, id);
            }
        }

        // Check if file is in a pak
        for (const auto& [pakID, pakFile]: m_pakFiles)
        {
            if (pakFile && pakFile->IsOpen() && pakFile->HasAssetByName(id))
            {
                std::vector<Byte> data;
                pakFile->LoadAssetDataByName(id, data);
                if (data.empty()) { return FTextureHandle(); }
                auto uniqueTexture =
                    m_graphicsFactory->CreateTexture(data, area);
                if (!uniqueTexture) { return FTextureHandle(); }
                texture = std::shared_ptr<ITexture>(std::move(uniqueTexture));
                m_textures[id] = texture;
                return FTextureHandle(texture, id);
            }
        }

        // Create new texture
        auto uniqueTexture = m_graphicsFactory->CreateTexture(path, area);
        if (!uniqueTexture) { return FTextureHandle(); }

        texture = std::shared_ptr<ITexture>(std::move(uniqueTexture));
        m_textures[id] = texture;
    }
    // Handle UUID (from pak or asset)
    else if (std::holds_alternative<FString>(source))
    {
        const FString& uuid = std::get<FString>(source);
        id = uuid;

        // Check if already loaded
        auto it = m_textures.find(id);
        if (it != m_textures.end())
        {
            if (auto existing = it->second.lock())
            {
                return FTextureHandle(existing, id);
            }
        }

        // Check if file is in a pak
        for (const auto& [pakID, pakFile]: m_pakFiles)
        {
            if (pakFile && pakFile->IsOpen() && pakFile->HasAssetByName(id))
            {
                std::vector<Byte> data;
                pakFile->LoadAssetDataByName(id, data);
                if (data.empty()) { return FTextureHandle(); }
                auto uniqueTexture =
                    m_graphicsFactory->CreateTexture(data, area);
                if (!uniqueTexture) { return FTextureHandle(); }
                texture = std::shared_ptr<ITexture>(std::move(uniqueTexture));
                m_textures[id] = texture;
                return FTextureHandle(texture, id);
            }
        }

        // Try to find asset
        UAsset* asset = GetAsset(uuid);
        if (!asset) { return FTextureHandle(); }

        if (!asset->IsLoaded() && !asset->Load()) { return FTextureHandle(); }

        // Create texture from asset data
        const auto& data = asset->GetData();
        auto uniqueTexture = m_graphicsFactory->CreateTexture(data, area);
        if (!uniqueTexture) { return FTextureHandle(); }

        texture = std::shared_ptr<ITexture>(std::move(uniqueTexture));
        m_textures[id] = texture;
    }
    // Handle UAsset pointer
    else if (std::holds_alternative<UAsset*>(source))
    {
        UAsset* asset = std::get<UAsset*>(source);
        if (!asset) { return FTextureHandle(); }

        id = asset->GetUUID();

        // Check if already loaded
        auto it = m_textures.find(id);
        if (it != m_textures.end())
        {
            if (auto existing = it->second.lock())
            {
                return FTextureHandle(existing, id);
            }
        }

        if (!asset->IsLoaded() && !asset->Load()) { return FTextureHandle(); }

        // Create texture from asset data
        const auto& data = asset->GetData();
        auto uniqueTexture = m_graphicsFactory->CreateTexture(data, area);
        if (!uniqueTexture) { return FTextureHandle(); }

        texture = std::shared_ptr<ITexture>(std::move(uniqueTexture));
        m_textures[id] = texture;
    }
    else { return FTextureHandle(); }

    return FTextureHandle(texture, id);
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle URessource::GetTexture(const FString& id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto it = m_textures.find(id);
    if (it != m_textures.end())
    {
        if (auto texture = it->second.lock())
        {
            return FTextureHandle(texture, id);
        }
    }

    return FTextureHandle();
}

///////////////////////////////////////////////////////////////////////////////
FShaderHandle URessource::LoadShader(const TVariant<
                                     std::tuple<FilePath, EShaderType>,
                                     std::tuple<FilePath, FilePath>,
                                     std::tuple<FilePath, FilePath, FilePath>,
                                     FString,
                                     UAsset*>& source)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isShuttingDown || !m_graphicsFactory) { return FShaderHandle(); }

    FString id;
    std::shared_ptr<IShader> shader;

    // Handle single shader file
    if (std::holds_alternative<std::tuple<FilePath, EShaderType>>(source))
    {
        const auto& [path, type] =
            std::get<std::tuple<FilePath, EShaderType>>(source);
        id = path.string();

        // Check if already loaded
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            if (auto existing = it->second.lock())
            {
                return FShaderHandle(existing, id);
            }
        }

        auto uniqueShader = m_graphicsFactory->CreateShader(path, type);
        if (!uniqueShader) { return FShaderHandle(); }

        shader = std::shared_ptr<IShader>(std::move(uniqueShader));
        m_shaders[id] = shader;
    }
    // Handle vertex + fragment shaders
    else if (std::holds_alternative<std::tuple<FilePath, FilePath>>(source))
    {
        const auto& [vertPath, fragPath] =
            std::get<std::tuple<FilePath, FilePath>>(source);
        id = vertPath.string() + " + " + fragPath.string();

        // Check if already loaded
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            if (auto existing = it->second.lock())
            {
                return FShaderHandle(existing, id);
            }
        }

        auto uniqueShader =
            m_graphicsFactory->CreateShader(vertPath, fragPath);
        if (!uniqueShader) { return FShaderHandle(); }

        shader = std::shared_ptr<IShader>(std::move(uniqueShader));
        m_shaders[id] = shader;
    }
    // Handle vertex + geometry + fragment shaders
    else if (std::holds_alternative<std::tuple<FilePath, FilePath, FilePath>>(
                 source
             ))
    {
        const auto& [vertPath, geomPath, fragPath] =
            std::get<std::tuple<FilePath, FilePath, FilePath>>(source);
        id = vertPath.string() + " + " + geomPath.string() + " + " +
             fragPath.string();

        // Check if already loaded
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            if (auto existing = it->second.lock())
            {
                return FShaderHandle(existing, id);
            }
        }

        auto uniqueShader =
            m_graphicsFactory->CreateShader(vertPath, geomPath, fragPath);
        if (!uniqueShader) { return FShaderHandle(); }

        shader = std::shared_ptr<IShader>(std::move(uniqueShader));
        m_shaders[id] = shader;
    }
    // Handle UUID (from pak or asset)
    else if (std::holds_alternative<FString>(source))
    {
        const FString& uuid = std::get<FString>(source);
        id = uuid;

        // Check if already loaded
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            if (auto existing = it->second.lock())
            {
                return FShaderHandle(existing, id);
            }
        }

        // Try to find asset
        UAsset* asset = GetAsset(uuid);
        if (!asset) { return FShaderHandle(); }

        if (!asset->IsLoaded() && !asset->Load()) { return FShaderHandle(); }

        // Create shader from asset data
        const auto& data = asset->GetData();
        auto uniqueShader = m_graphicsFactory->CreateShader(
            data, EShaderType::Vertex
        );   // Default to vertex, may need improvement
        if (!uniqueShader) { return FShaderHandle(); }

        shader = std::shared_ptr<IShader>(std::move(uniqueShader));
        m_shaders[id] = shader;
    }
    // Handle UAsset pointer
    else if (std::holds_alternative<UAsset*>(source))
    {
        UAsset* asset = std::get<UAsset*>(source);
        if (!asset) { return FShaderHandle(); }

        id = asset->GetUUID();

        // Check if already loaded
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            if (auto existing = it->second.lock())
            {
                return FShaderHandle(existing, id);
            }
        }

        if (!asset->IsLoaded() && !asset->Load()) { return FShaderHandle(); }

        // Create shader from asset data
        const auto& data = asset->GetData();
        auto uniqueShader = m_graphicsFactory->CreateShader(
            data, EShaderType::Vertex
        );   // Default to vertex, may need improvement
        if (!uniqueShader) { return FShaderHandle(); }

        shader = std::shared_ptr<IShader>(std::move(uniqueShader));
        m_shaders[id] = shader;
    }
    else { return FShaderHandle(); }

    return FShaderHandle(shader, id);
}

///////////////////////////////////////////////////////////////////////////////
FShaderHandle URessource::GetShader(const FString& id) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto it = m_shaders.find(id);
    if (it != m_shaders.end())
    {
        if (auto shader = it->second.lock())
        {
            return FShaderHandle(shader, id);
        }
    }
    return FShaderHandle();
}

///////////////////////////////////////////////////////////////////////////////
bool URessource::LoadPak(const FilePath& pakPath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isShuttingDown) { return false; }

    FString id = pakPath.string();

    // Check if already loaded
    if (m_pakFiles.find(id) != m_pakFiles.end()) { return true; }

    auto pak = std::make_unique<UPak>(pakPath);
    if (!pak || !pak->IsOpen()) { return false; }

    m_pakFiles[id] = std::move(pak);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void URessource::UnloadPak(const FilePath& pakPath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    FString id = pakPath.string();
    m_pakFiles.erase(id);
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FilePath> URessource::GetLoadedPaks(void) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<FilePath> result;
    result.reserve(m_pakFiles.size());

    for (const auto& [path, pak]: m_pakFiles)
    {
        result.push_back(FilePath(path));
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
FTextureHandle
    URessource::GetTextureHandle(const FilePath& path, const FRectanglei& area)
{
    URessource& instance = URessource::GetInstance();
    FTextureHandle handle = instance.GetTexture(path.string());

    if (handle.IsValid()) { return handle; }
    return instance.LoadTexture(path, area);
}

}   // namespace tkd
