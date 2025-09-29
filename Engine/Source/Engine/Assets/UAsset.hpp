///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <memory>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////////////////////////////////
class UPak;

///////////////////////////////////////////////////////////////////////////////
// Namespace __internal
///////////////////////////////////////////////////////////////////////////////
namespace __internal
{

constexpr UInt32 UASSET_MAGIC = 0x54455355;             //<! "USET" in hex
constexpr UInt16 UASSET_VERSION = 1;
constexpr SizeT UASSET_HEADER_SIZE = sizeof(UInt32) +   //<! Magic
                                     sizeof(UInt16) +   //<! Version
                                     sizeof(UInt8) +    //<! Asset type
                                     sizeof(UInt32) +   //<! UUID length
                                     sizeof(UInt32) +   //<! Name length
                                     sizeof(UInt32) +   //<! Path length
                                     sizeof(UInt64);    //<! Data size

}   // namespace __internal

///////////////////////////////////////////////////////////////////////////////
/// \brief Enumeration of asset types
///
///////////////////////////////////////////////////////////////////////////////
enum class EAssetType : UInt8
{
    Unknown = 0,   //<! Unknown asset type.
    Level,         //<! Level asset type.
    Texture,       //<! Texture asset type.
    Audio          //<! Audio asset type.
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Enumeration of asset source types
///
///////////////////////////////////////////////////////////////////////////////
enum class EAssetSource : UInt8
{
    Standalone,   //<! Asset loaded from standalone .uasset file
    Package       //<! Asset loaded from .pak package file
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Class representing an asset in the engine.
///
/// The UAsset class handles asset metadata and data management. By default,
/// only the header (metadata) is loaded to efficiently track assets. The
/// actual asset data can be loaded/unloaded on demand.
///
/// Assets can be loaded from either:
/// - Standalone .uasset files
/// - Packaged .pak files (more efficient for distribution)
///
/// File Format (.uasset):
/// - Magic Number (4 bytes): 0x54455355 ("USET")
/// - Version (2 bytes): Format version
/// - Asset Type (1 byte): EAssetType enum value
/// - UUID Length (4 bytes) + UUID Data (variable)
/// - Name Length (4 bytes) + Name Data (variable)
/// - Path Length (4 bytes) + Path Data (variable)
/// - Data Size (8 bytes): Size of asset data in bytes
/// - Asset Data (variable): Raw binary data
///
///////////////////////////////////////////////////////////////////////////////
class UAsset
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    FString m_uuid;             //<! Unique identifier for the asset.
    EAssetType m_type;          //<! Type of the asset.
    SizeT m_size;               //<! Size of the asset data in bytes.
    FString m_name;             //<! Name of the asset.
    FilePath m_path;            //<! File path of the asset.
    bool m_isLoaded;            //<! Flag indicating if the asset is loaded.
    std::vector<Byte> m_data;   //<! Raw data of the asset.
    EAssetSource m_source;      //<! Source type of the asset.
    UPak* m_pakFile;            //<! Pointer to pak file if from package.
    UInt64 m_pakOffset;         //<! Offset in pak file.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for UAsset from standalone file.
    ///
    /// Parses the asset file header to extract metadata without loading
    /// the full asset data.
    ///
    /// \param uassetPath The file path to the asset.
    ///
    /// \throws std::runtime_error if the asset file cannot be parsed.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UAsset(const FilePath& uassetPath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for UAsset from pak file.
    ///
    /// Creates an asset reference that will load data from a pak file
    /// at the specified offset when Load() is called.
    ///
    /// \param pakFile Pointer to the parent pak file.
    /// \param uuid Unique identifier of the asset.
    /// \param name Name of the asset.
    /// \param type Type of the asset.
    /// \param offset Offset in the pak file where data is stored.
    /// \param size Size of the asset data.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UAsset(
        UPak* pakFile,
        const FString& uuid,
        const FString& name,
        EAssetType type,
        UInt64 offset,
        UInt64 size
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor for manual setup.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UAsset();

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Loads the asset data from the file or pak.
    ///
    /// Reads the complete asset data into memory. If the asset is already
    /// loaded, this function returns true immediately.
    ///
    /// \return True if the asset was loaded successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Load(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unloads the asset data, freeing associated resources.
    ///
    /// Clears the asset data from memory while preserving metadata. The
    /// asset can be reloaded later using Load().
    ///
    /// \return True if the asset was unloaded successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Unload(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Saves the asset to a .uasset file.
    ///
    /// Writes the asset metadata and data to the specified file path in
    /// the .uasset binary format.
    ///
    /// \param savePath The file path where the asset should be saved.
    ///
    /// \return True if the asset was saved successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Save(const FilePath& savePath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the UUID of the asset.
    ///
    /// \return The UUID of the asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString GetUUID(void) const { return m_uuid; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the type of the asset.
    ///
    /// \return The type of the asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    EAssetType GetType(void) const { return m_type; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the size of the asset data in bytes.
    ///
    /// \return The size of the asset data.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT GetSize(void) const { return m_size; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the name of the asset.
    ///
    /// \return The name of the asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString GetName(void) const { return m_name; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the file path of the asset.
    ///
    /// \return The file path of the asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FilePath GetPath(void) const { return m_path; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the asset is currently loaded.
    ///
    /// \return True if the asset is loaded, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsLoaded(void) const { return m_isLoaded; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the raw data of the asset.
    ///
    /// \return The raw data of the asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const std::vector<Byte>& GetData(void) const { return m_data; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the source type of the asset.
    ///
    /// \return The source type (Standalone or Package).
    ///
    ///////////////////////////////////////////////////////////////////////////
    EAssetSource GetSource(void) const { return m_source; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the asset is from a pak file.
    ///
    /// \return True if from pak, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsFromPak(void) const { return m_source == EAssetSource::Package; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the asset data (copy version).
    ///
    /// \param data The data to set for this asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetData(const std::vector<Byte>& data);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the asset data (move version).
    ///
    /// \param data The data to set for this asset (moved).
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetData(std::vector<Byte>&& data);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the asset metadata.
    ///
    /// \param uuid The UUID of the asset.
    /// \param name The name of the asset.
    /// \param type The type of the asset.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void
        SetMetadata(const FString& uuid, const FString& name, EAssetType type);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parses the asset file to extract metadata and data.
    ///
    /// \param uassetPath The file path to the asset.
    ///
    /// \return True if parsing was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Parse(const FilePath& uassetPath);
};

}   // namespace tkd
