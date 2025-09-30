///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UPak.hpp>
#include <fstream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// File format constants
///////////////////////////////////////////////////////////////////////////////
namespace
{

constexpr UInt32 PAK_MAGIC = 0x5041434B;   // "PACK" in hex
constexpr UInt16 PAK_VERSION = 1;

}   // namespace

///////////////////////////////////////////////////////////////////////////////
UPak::UPak()
    : m_path("")
    , m_isOpen(false)
    , m_entryCount(0)
    , m_entries()
    , m_nameIndex()
    , m_dataStartOffset(0)
{}

///////////////////////////////////////////////////////////////////////////////
UPak::UPak(const FilePath& pakPath)
    : m_path(pakPath)
    , m_isOpen(false)
    , m_entryCount(0)
    , m_entries()
    , m_nameIndex()
    , m_dataStartOffset(0)
{
    Open(pakPath);
}

///////////////////////////////////////////////////////////////////////////////
UPak::~UPak() { Close(); }

///////////////////////////////////////////////////////////////////////////////
bool UPak::Open(const FilePath& pakPath)
{
    if (m_isOpen) { Close(); }

    m_path = pakPath;

    // Verify file exists
    std::ifstream file(m_path, std::ios::binary);
    if (!file.is_open()) { return false; }

    // Read and verify magic number
    UInt32 magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (!file || magic != PAK_MAGIC) { return false; }

    // Read and verify version
    UInt16 version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!file || version != PAK_VERSION) { return false; }

    // Read entry count
    file.read(reinterpret_cast<char*>(&m_entryCount), sizeof(m_entryCount));
    if (!file) { return false; }

    file.close();

    // Parse the index table
    if (!ParseIndex())
    {
        m_entryCount = 0;
        return false;
    }

    m_isOpen = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void UPak::Close(void)
{
    m_entries.clear();
    m_nameIndex.clear();
    m_entryCount = 0;
    m_dataStartOffset = 0;
    m_isOpen = false;
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::Create(const FilePath& pakPath, const std::vector<UAsset*>& assets)
{
    if (assets.empty()) { return false; }

    // Open file for writing
    std::ofstream file(pakPath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) { return false; }

    // Write header
    UInt32 magic = PAK_MAGIC;
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));

    UInt16 version = PAK_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    UInt32 entryCount = static_cast<UInt32>(assets.size());
    file.write(reinterpret_cast<const char*>(&entryCount), sizeof(entryCount));

    // Calculate offsets and write index table
    UInt64 currentOffset =
        sizeof(magic) + sizeof(version) + sizeof(entryCount);

    // First pass: calculate index table size
    UInt64 indexSize = 0;
    for (const auto* asset: assets)
    {
        indexSize += sizeof(UInt32) + asset->GetUUID().Size();   // UUID
        indexSize += sizeof(UInt32) + asset->GetName().Size();   // Name
        indexSize += sizeof(UInt8);                              // Type
        indexSize += sizeof(UInt64);                             // Offset
        indexSize += sizeof(UInt64);                             // Size
        indexSize += sizeof(UInt64);   // Compressed size
        indexSize += sizeof(UInt8);    // Is compressed
    }

    UInt64 dataStartOffset = currentOffset + indexSize;
    UInt64 dataOffset = dataStartOffset;

    // Write index entries
    std::vector<FPakEntry> entries;
    for (const auto* asset: assets)
    {
        FPakEntry entry;
        entry.uuid = asset->GetUUID();
        entry.name = asset->GetName();
        entry.type = asset->GetType();
        entry.offset = dataOffset;
        entry.size = asset->GetSize();
        entry.compressedSize = 0;   // No compression for now
        entry.isCompressed = false;

        // Write UUID
        UInt32 uuidLen = static_cast<UInt32>(entry.uuid.Size());
        file.write(reinterpret_cast<const char*>(&uuidLen), sizeof(uuidLen));
        file.write(entry.uuid.CStr(), uuidLen);

        // Write name
        UInt32 nameLen = static_cast<UInt32>(entry.name.Size());
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(entry.name.CStr(), nameLen);

        // Write type
        UInt8 assetType = static_cast<UInt8>(entry.type);
        file.write(
            reinterpret_cast<const char*>(&assetType), sizeof(assetType)
        );

        // Write offset and sizes
        file.write(
            reinterpret_cast<const char*>(&entry.offset), sizeof(entry.offset)
        );
        file.write(
            reinterpret_cast<const char*>(&entry.size), sizeof(entry.size)
        );
        file.write(
            reinterpret_cast<const char*>(&entry.compressedSize),
            sizeof(entry.compressedSize)
        );

        UInt8 compressed = entry.isCompressed ? 1 : 0;
        file.write(
            reinterpret_cast<const char*>(&compressed), sizeof(compressed)
        );

        entries.push_back(entry);
        dataOffset += entry.size;
    }

    // Write asset data
    for (size_t i = 0; i < assets.size(); ++i)
    {
        const auto* asset = assets[i];

        // Make sure asset is loaded
        if (!asset->IsLoaded())
        {
            // Can't write unloaded asset
            continue;
        }

        const auto& data = asset->GetData();
        if (!data.empty())
        {
            file.write(
                reinterpret_cast<const char*>(data.data()), data.size()
            );
        }
    }

    if (!file) { return false; }

    file.close();

    // Open the newly created pak
    return Open(pakPath);
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::AddAsset(TKD_MAYBE_UNUSED UAsset* asset)
{
    // Not implemented - would require pak file reconstruction
    // For now, use Create() with all assets
    return false;
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::LoadAssetData(const FString& uuid, std::vector<Byte>& outData)
{
    if (!m_isOpen) { return false; }

    auto it = m_entries.find(uuid);
    if (it == m_entries.end()) { return false; }

    const FPakEntry& entry = it->second;
    return ReadDataAtOffset(entry.offset, entry.size, outData);
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::LoadAssetDataByName(const FString& name, std::vector<Byte>& outData)
{
    if (!m_isOpen) { return false; }

    auto it = m_nameIndex.find(name);
    if (it == m_nameIndex.end()) { return false; }

    const FPakEntry& entry = it->second;
    return ReadDataAtOffset(entry.offset, entry.size, outData);
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<UAsset> UPak::CreateAsset(const FString& uuid)
{
    if (!m_isOpen) { return nullptr; }

    auto it = m_entries.find(uuid);
    if (it == m_entries.end()) { return nullptr; }

    const FPakEntry& entry = it->second;

    // Create asset with metadata only
    auto asset = std::make_unique<UAsset>(
        this, entry.uuid, entry.name, entry.type, entry.offset, entry.size
    );

    // Load the data from pak
    std::vector<Byte> data;
    if (ReadDataAtOffset(entry.offset, entry.size, data))
    {
        asset->SetData(std::move(data));
    }

    return asset;
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<UAsset> UPak::CreateAssetByName(const FString& name)
{
    if (!m_isOpen) { return nullptr; }

    auto it = m_nameIndex.find(name);
    if (it == m_nameIndex.end()) { return nullptr; }

    return CreateAsset(it->second.uuid);
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::HasAsset(const FString& uuid) const
{
    return m_entries.find(uuid) != m_entries.end();
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::HasAssetByName(const FString& name) const
{
    return m_nameIndex.find(name) != m_nameIndex.end();
}

///////////////////////////////////////////////////////////////////////////////
const FPakEntry* UPak::GetEntry(const FString& uuid) const
{
    auto it = m_entries.find(uuid);
    if (it == m_entries.end()) { return nullptr; }
    return &it->second;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FPakEntry> UPak::GetAllEntries(void) const
{
    std::vector<FPakEntry> entries;
    entries.reserve(m_entries.size());

    for (const auto& pair: m_entries) { entries.push_back(pair.second); }

    return entries;
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::ParseIndex(void)
{
    std::ifstream file(m_path, std::ios::binary);
    if (!file.is_open()) { return false; }

    // Skip header
    file.seekg(sizeof(UInt32) + sizeof(UInt16), std::ios::beg);

    // Read entry count
    UInt32 entryCount;
    file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount));
    if (!file) { return false; }

    // Read all entries
    for (UInt32 i = 0; i < entryCount; ++i)
    {
        FPakEntry entry;

        // Read UUID
        UInt32 uuidLen;
        file.read(reinterpret_cast<char*>(&uuidLen), sizeof(uuidLen));
        if (!file || uuidLen > 1024) { return false; }

        std::vector<char> uuidBuffer(uuidLen);
        file.read(uuidBuffer.data(), uuidLen);
        if (!file) { return false; }
        entry.uuid = std::string(uuidBuffer.begin(), uuidBuffer.end());

        // Read name
        UInt32 nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        if (!file || nameLen > 1024) { return false; }

        std::vector<char> nameBuffer(nameLen);
        file.read(nameBuffer.data(), nameLen);
        if (!file) { return false; }
        entry.name = std::string(nameBuffer.begin(), nameBuffer.end());

        // Read type
        UInt8 assetType;
        file.read(reinterpret_cast<char*>(&assetType), sizeof(assetType));
        if (!file) { return false; }
        entry.type = static_cast<EAssetType>(assetType);

        // Read offset and sizes
        file.read(
            reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset)
        );
        file.read(reinterpret_cast<char*>(&entry.size), sizeof(entry.size));
        file.read(
            reinterpret_cast<char*>(&entry.compressedSize),
            sizeof(entry.compressedSize)
        );

        UInt8 compressed;
        file.read(reinterpret_cast<char*>(&compressed), sizeof(compressed));
        entry.isCompressed = (compressed != 0);

        if (!file) { return false; }

        // Store entry
        m_entries[entry.uuid] = entry;
        m_nameIndex[entry.name] = entry;
    }

    // Calculate data start offset
    m_dataStartOffset = static_cast<UInt64>(file.tellg());

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UPak::ReadDataAtOffset(
    UInt64 offset, UInt64 size, std::vector<Byte>& outData
)
{
    std::ifstream file(m_path, std::ios::binary);
    if (!file.is_open()) { return false; }

    // Seek to offset
    file.seekg(offset, std::ios::beg);
    if (!file) { return false; }

    // Read data
    outData.resize(static_cast<SizeT>(size));
    file.read(reinterpret_cast<char*>(outData.data()), size);

    return file.good() || file.eof();
}

}   // namespace tkd
