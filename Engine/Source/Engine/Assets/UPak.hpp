///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UAsset.hpp>
#include <Engine/Config.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Structure representing an entry in the package file.
///
///////////////////////////////////////////////////////////////////////////////
struct FPakEntry
{
    FString uuid;      //<! Unique identifier of the asset.
    FString name;      //<! Name of the asset.
    EAssetType type;   //<! Type of the asset.
    UInt64 offset;     //<! Offset in the pak file where asset data starts.
    UInt64 size;       //<! Size of the asset data in bytes.
    UInt64 compressedSize;   //<! Compressed size (0 if not compressed).
    bool isCompressed;       //<! Flag indicating if asset is compressed.

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FPakEntry(void)
        : uuid("")
        , name("")
        , type(EAssetType::Unknown)
        , offset(0)
        , size(0)
        , compressedSize(0)
        , isCompressed(false)
    {}
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Class representing a package file containing multiple assets.
///
/// The UPak class manages a collection of assets stored in a single .pak file.
/// It provides efficient access to individual assets through an index table
/// stored at the beginning of the file.
///
/// File Format (.pak):
/// - Magic Number (4 bytes): 0x5041434B ("PACK")
/// - Version (2 bytes): Format version
/// - Entry Count (4 bytes): Number of assets in the package
/// - Index Table:
///   - For each entry:
///     - UUID Length (4 bytes) + UUID Data
///     - Name Length (4 bytes) + Name Data
///     - Asset Type (1 byte)
///     - Data Offset (8 bytes)
///     - Data Size (8 bytes)
///     - Compressed Size (8 bytes)
///     - Is Compressed (1 byte)
/// - Asset Data Section:
///   - Raw binary data for all assets (sequentially)
///
///////////////////////////////////////////////////////////////////////////////
class UPak
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Members
    ///////////////////////////////////////////////////////////////////////////
    FilePath m_path;            //<! Path to the pak file.
    bool m_isOpen;              //<! Flag indicating if pak is open.
    UInt32 m_entryCount;        //<! Number of entries in the pak.
    std::unordered_map<FString, FPakEntry>
        m_entries;              //<! Map of UUID to pak entries.
    std::unordered_map<FString, FPakEntry>
        m_nameIndex;            //<! Map of name to pak entries.
    UInt64 m_dataStartOffset;   //<! Offset where asset data starts.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UPak(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor that opens a pak file.
    ///
    /// \param pakPath The file path to the pak file.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UPak(const FilePath& pakPath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~UPak();

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Opens and parses a pak file.
    ///
    /// \param pakPath The file path to the pak file.
    ///
    /// \return True if the pak was opened successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Open(const FilePath& pakPath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Closes the pak file and clears the index.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Close(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a new pak file from a list of assets.
    ///
    /// \param pakPath The file path where the pak should be saved.
    /// \param assets Vector of assets to include in the package.
    ///
    /// \return True if the pak was created successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Create(const FilePath& pakPath, const std::vector<UAsset*>& assets);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Adds an asset to an existing pak file.
    ///
    /// \param asset The asset to add to the package.
    ///
    /// \return True if the asset was added successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool AddAsset(UAsset* asset);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Loads an asset from the pak file by UUID.
    ///
    /// \param uuid The unique identifier of the asset.
    /// \param outData Output vector to store the loaded data.
    ///
    /// \return True if the asset was loaded successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LoadAssetData(const FString& uuid, std::vector<Byte>& outData);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Loads an asset from the pak file by name.
    ///
    /// \param name The name of the asset.
    /// \param outData Output vector to store the loaded data.
    ///
    /// \return True if the asset was loaded successfully, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool LoadAssetDataByName(const FString& name, std::vector<Byte>& outData);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a UAsset instance from pak data.
    ///
    /// Creates an asset with metadata from the pak and provides methods
    /// to load the actual data on demand.
    ///
    /// \param uuid The unique identifier of the asset.
    ///
    /// \return Unique pointer to the created asset, or nullptr on failure.
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::unique_ptr<UAsset> CreateAsset(const FString& uuid);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a UAsset instance from pak data by name.
    ///
    /// \param name The name of the asset.
    ///
    /// \return Unique pointer to the created asset, or nullptr on failure.
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::unique_ptr<UAsset> CreateAssetByName(const FString& name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if an asset exists in the pak.
    ///
    /// \param uuid The unique identifier of the asset.
    ///
    /// \return True if the asset exists, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool HasAsset(const FString& uuid) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if an asset exists in the pak by name.
    ///
    /// \param name The name of the asset.
    ///
    /// \return True if the asset exists, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool HasAssetByName(const FString& name) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the entry information for an asset.
    ///
    /// \param uuid The unique identifier of the asset.
    ///
    /// \return Pointer to the entry, or nullptr if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FPakEntry* GetEntry(const FString& uuid) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the entry information for an asset by name.
    ///
    /// \param name The name of the asset.
    ///
    /// \return Pointer to the entry, or nullptr if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FPakEntry* GetEntryByName(const FString& name) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets all entries in the pak.
    ///
    /// \return Vector of all pak entries.
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::vector<FPakEntry> GetAllEntries(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an asset from the pak by UUID.
    ///
    /// \param uuid The unique identifier of the asset.
    ///
    /// \return Unique pointer to the asset, or nullptr if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TUniquePtr<UAsset> GetAsset(const FString& uuid);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get an asset from the pak by name.
    ///
    /// \param name The name of the asset.
    ///
    /// \return Unique pointer to the asset, or nullptr if not found.
    ///
    ///////////////////////////////////////////////////////////////////////////
    TUniquePtr<UAsset> GetAssetByName(const FString& name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the number of assets in the pak.
    ///
    /// \return Number of assets.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UInt32 GetEntryCount(void) const { return m_entryCount; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the pak is currently open.
    ///
    /// \return True if open, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsOpen(void) const { return m_isOpen; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the path to the pak file.
    ///
    /// \return The file path.
    ///
    ///////////////////////////////////////////////////////////////////////////
    FilePath GetPath(void) const { return m_path; }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Parses the pak file index table.
    ///
    /// \return True if parsing was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ParseIndex(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reads asset data from the pak at a specific offset.
    ///
    /// \param offset The offset in the file.
    /// \param size The size of data to read.
    /// \param outData Output vector to store the data.
    ///
    /// \return True if read was successful, false otherwise.
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ReadDataAtOffset(
        UInt64 offset, UInt64 size, std::vector<Byte>& outData
    );
};

}   // namespace tkd
