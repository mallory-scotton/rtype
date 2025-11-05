///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Assets/UPak.hpp>
#include <Engine/Assets/UResourceHandle.hpp>
#include <Engine/Audio/Interfaces.hpp>
#include <Engine/Config.hpp>
#include <Engine/Core.hpp>
#include <Engine/Core/Utils/Singleton.hpp>
#include <Engine/Renderer/Interfaces/IGraphicsFactory.hpp>
#include <Engine/Renderer/Interfaces/IShader.hpp>
#include <Engine/Renderer/Interfaces/ITexture.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <variant>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Resource types that can be loaded
///
///////////////////////////////////////////////////////////////////////////////
enum class EResourceSource
{
    FilePath,   //<! Load from a file path
    UAsset,     //<! Load from a .uasset file
    Pak,        //<! Load from a .pak file
    Memory      //<! Load from memory
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Resource manager with automatic lifetime management
///
/// Features:
/// - Reference counting for automatic unloading
/// - Support for loading from FilePath, UAsset, or Pak
/// - Thread-safe resource access
/// - Automatic cleanup of unused resources
/// - Safe shutdown order
///
///////////////////////////////////////////////////////////////////////////////
class URessource : public TSingleton<URessource>
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    using TSharedMap = std::unordered_map<FString, std::weak_ptr<T>>;
    template <typename T>
    using TUniqueMap = std::unordered_map<FString, TUniquePtr<T>>;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    IGraphicsFactory* m_graphicsFactory;   //<! Graphics factory
    IAudioManager* m_audioManager;         //<! Audio manager
    bool m_isShuttingDown;                 //<! Shutdown flag
    mutable std::mutex m_mutex;            //<! Thread safety mutex

    TSharedMap<ITexture> m_textures;       //<! Loaded textures (weak refs)
    TSharedMap<IShader> m_shaders;         //<! Loaded shaders (weak refs)
    TSharedMap<IAudioBuffer> m_buffers;    //<! Loaded audio buffers
    TSharedMap<IAudioSource> m_sources;    //<! Loaded audio sources

    TUniqueMap<UPak> m_pakFiles;           //<! Loaded pak files
    TUniqueMap<UAsset> m_assets;           //<! Loaded assets

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    URessource(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~URessource();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Begin shutdown - stops accepting new resource loads
    ///
    ///////////////////////////////////////////////////////////////////////////
    void BeginShutdown(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Cleanup expired resource references
    ///
    /// \return Number of resources cleaned up
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT CleanupUnusedResources(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the graphics factory
    ///
    /// \param factory Pointer to the graphics factory
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetGraphicsFactory(IGraphicsFactory* factory);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the audio manager
    ///
    /// \param manager Pointer to the audio manager
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetAudioManager(IAudioManager* manager);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load an asset from file or pak
    ///
    /// \param path Path to the asset file or UUID if from pak
    ///
    /// \return Reference to the loaded asset, or nullptr on failure
    ///
    ///////////////////////////////////////////////////////////////////////////
    UAsset* LoadAsset(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a loaded asset by UUID
    ///
    /// \param uuid UUID of the asset
    ///
    /// \return Pointer to the asset, or nullptr if not found
    ///
    ///////////////////////////////////////////////////////////////////////////
    UAsset* GetAsset(const FString& uuid) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a loaded asset by name
    ///
    /// \param name Name of the asset
    ///
    /// \return Pointer to the asset, or nullptr if not found
    ///
    ///////////////////////////////////////////////////////////////////////////
    TUniquePtr<UAsset> GetAssetFromName(const FString& name) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load a texture from various sources with reference counting
    ///
    /// \param source Source of the texture (file path, UUID, or asset)
    /// \param area Area of the texture to load (optional)
    ///
    /// \return Handle to the loaded texture, or invalid handle on failure
    ///
    ///////////////////////////////////////////////////////////////////////////
    FTextureHandle LoadTexture(
        const TVariant<FilePath, FString, UAsset*>& source,
        const FRectanglei& area = FRectanglei::Zero
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a texture handle by identifier
    ///
    /// \param id Identifier of the texture (file path or asset UUID)
    ///
    /// \return Handle to the texture, or invalid handle if not found
    ///
    ///////////////////////////////////////////////////////////////////////////
    FTextureHandle GetTexture(const FString& id) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load a shader from various sources with reference counting
    ///
    /// \param source Source of the shader (file path(s), UUID, or asset)
    ///
    /// \return Handle to the loaded shader, or invalid handle on failure
    ///
    ///////////////////////////////////////////////////////////////////////////
    FShaderHandle LoadShader(
        const TVariant<
            std::tuple<FilePath, EShaderType>,
            std::tuple<FilePath, FilePath>,
            std::tuple<FilePath, FilePath, FilePath>,
            FString,
            UAsset*>& source
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a shader handle by identifier
    ///
    /// \param id Identifier of the shader (file path(s) or UUID)
    ///
    /// \return Handle to the shader, or invalid handle if not found
    ///
    ///////////////////////////////////////////////////////////////////////////
    FShaderHandle GetShader(const FString& id) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load an audio buffer from various sources with reference
    /// counting
    ///
    /// \param source Source of the audio (file path, UUID, or asset)
    ///
    /// \return Handle to the loaded audio buffer, or invalid handle on failure
    ///
    ///////////////////////////////////////////////////////////////////////////
    FAudioBufferHandle
        LoadAudioBuffer(const TVariant<FilePath, FString, UAsset*>& source);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an audio buffer handle by identifier
    ///
    /// \param id Identifier of the audio buffer (file path or asset UUID)
    ///
    /// \return Handle to the audio buffer, or invalid handle if not found
    ///
    ///////////////////////////////////////////////////////////////////////////
    FAudioBufferHandle GetAudioBuffer(const FString& id) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load or mount a pak file
    ///
    /// \param pakPath Path to the .pak file
    ///
    /// \return True if the pak was loaded successfully
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LoadPak(const FilePath& pakPath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unload a pak file
    ///
    /// \param pakPath Path to the .pak file
    ///
    ///////////////////////////////////////////////////////////////////////////
    void UnloadPak(const FilePath& pakPath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get list of all loaded pak files
    ///
    /// \return Vector of pak file paths
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<FilePath> GetLoadedPaks(void) const;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static helper to load a texture directly
    ///
    /// \param path Path to the texture file
    /// \param area Area of the texture to load (optional)
    ///
    /// \return Handle to the loaded texture, or invalid handle on failure
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FTextureHandle GetTextureHandle(
        const FilePath& path, const FRectanglei& area = FRectanglei::Zero
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Static template helper to load a resource of type T
    ///
    /// \tparam T Resource type (ITexture or IShader)
    ///
    /// \param path Path to the resource file
    ///
    /// \return Resource handle of type T, or invalid handle on failure
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T,
        typename = std::enable_if_t<std::is_same_v<T, ITexture>>>
    static TResourceHandle<T> LoadResource(const FilePath& path)
    {
        if constexpr (std::is_same_v<T, ITexture>)
        {
            auto handle = GetInstance().GetTexture(path.string());
            if (handle.IsValid()) { return handle; }
            return GetInstance().LoadTexture(path);
        }
        return TResourceHandle<T>();
    }
};

}   // namespace tkd
