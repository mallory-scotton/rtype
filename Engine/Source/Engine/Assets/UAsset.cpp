///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Assets/UAsset.hpp>
#include <cstring>
#include <fstream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UAsset::UAsset(const FilePath& uassetPath)
    : m_uuid("")
    , m_type(EAssetType::Unknown)
    , m_size(0)
    , m_name("")
    , m_path(uassetPath)
    , m_isLoaded(false)
    , m_data()
{
    if (!Parse(uassetPath))
    {
        throw std::runtime_error(
            "Failed to parse asset file: " + uassetPath.string()
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
bool UAsset::Load(void)
{
    // Already loaded
    if (m_isLoaded) { return true; }

    // Open file in binary mode
    std::ifstream file(m_path, std::ios::binary);
    if (!file.is_open()) { return false; }

    // Read magic number
    UInt32 magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != __internal::UASSET_MAGIC) { return false; }

    // Read version
    UInt16 version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != __internal::UASSET_VERSION) { return false; }

    // Skip to data section (after header)
    // Read asset type
    UInt8 assetType;
    file.read(reinterpret_cast<char*>(&assetType), sizeof(assetType));

    // Read and skip UUID
    UInt32 uuidLen;
    file.read(reinterpret_cast<char*>(&uuidLen), sizeof(uuidLen));
    file.seekg(uuidLen, std::ios::cur);

    // Read and skip name
    UInt32 nameLen;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    file.seekg(nameLen, std::ios::cur);

    // Read and skip path
    UInt32 pathLen;
    file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    file.seekg(pathLen, std::ios::cur);

    // Read data size
    UInt64 dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

    // Allocate and read data
    if (dataSize > 0)
    {
        m_data.resize(static_cast<SizeT>(dataSize));
        file.read(reinterpret_cast<char*>(m_data.data()), dataSize);

        if (!file)
        {
            m_data.clear();
            return false;
        }
    }

    m_isLoaded = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UAsset::Unload(void)
{
    if (!m_isLoaded) { return true; }

    // Clear data and free memory
    m_data.clear();
    m_data.shrink_to_fit();
    m_isLoaded = false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UAsset::Parse(const FilePath& uassetPath)
{
    // Open file in binary mode
    std::ifstream file(uassetPath, std::ios::binary);
    if (!file.is_open()) { return false; }

    // Read and verify magic number
    UInt32 magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (!file || magic != __internal::UASSET_MAGIC) { return false; }

    // Read and verify version
    UInt16 version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!file || version != __internal::UASSET_VERSION) { return false; }

    // Read asset type
    UInt8 assetType;
    file.read(reinterpret_cast<char*>(&assetType), sizeof(assetType));
    if (!file) { return false; }
    m_type = static_cast<EAssetType>(assetType);

    // Read UUID
    UInt32 uuidLen;
    file.read(reinterpret_cast<char*>(&uuidLen), sizeof(uuidLen));
    if (!file || uuidLen > 1024)   // Sanity check
    {
        return false;
    }

    std::vector<char> uuidBuffer(uuidLen);
    file.read(uuidBuffer.data(), uuidLen);
    if (!file) { return false; }
    m_uuid = std::string(uuidBuffer.begin(), uuidBuffer.end());

    // Read name
    UInt32 nameLen;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (!file || nameLen > 1024)   // Sanity check
    {
        return false;
    }

    std::vector<char> nameBuffer(nameLen);
    file.read(nameBuffer.data(), nameLen);
    if (!file) { return false; }
    m_name = std::string(nameBuffer.begin(), nameBuffer.end());

    // Read path (stored in file, but we already have it)
    UInt32 pathLen;
    file.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    if (!file || pathLen > 2048)   // Sanity check
    {
        return false;
    }
    file.seekg(pathLen, std::ios::cur);   // Skip path data

    // Read data size
    UInt64 dataSize;
    file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
    if (!file) { return false; }
    m_size = static_cast<SizeT>(dataSize);

    // Don't load data yet - only parse header
    m_isLoaded = false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool UAsset::Save(const FilePath& savePath)
{
    // Open file for writing in binary mode
    std::ofstream file(savePath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) { return false; }

    // Write magic number
    UInt32 magic = __internal::UASSET_MAGIC;
    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));

    // Write version
    UInt16 version = __internal::UASSET_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // Write asset type
    UInt8 assetType = static_cast<UInt8>(m_type);
    file.write(reinterpret_cast<const char*>(&assetType), sizeof(assetType));

    // Write UUID
    UInt32 uuidLen = static_cast<UInt32>(m_uuid.Size());
    file.write(reinterpret_cast<const char*>(&uuidLen), sizeof(uuidLen));
    file.write(m_uuid.CStr(), uuidLen);

    // Write name
    UInt32 nameLen = static_cast<UInt32>(m_name.Size());
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    file.write(m_name.CStr(), nameLen);

    // Write path
    FString pathStr = m_path.string();
    UInt32 pathLen = static_cast<UInt32>(pathStr.Size());
    file.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
    file.write(pathStr.CStr(), pathLen);

    // Write data size
    UInt64 dataSize = static_cast<UInt64>(m_data.size());
    file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));

    // Write actual data
    if (!m_data.empty())
    {
        file.write(
            reinterpret_cast<const char*>(m_data.data()), m_data.size()
        );
    }

    if (!file) { return false; }

    // Update internal size
    m_size = m_data.size();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void UAsset::SetData(const std::vector<Byte>& data)
{
    m_data = data;
    m_size = data.size();
    m_isLoaded = true;
}

///////////////////////////////////////////////////////////////////////////////
void UAsset::SetData(std::vector<Byte>&& data)
{
    m_size = data.size();
    m_data = std::move(data);
    m_isLoaded = true;
}

///////////////////////////////////////////////////////////////////////////////
void UAsset::SetMetadata(
    const FString& uuid, const FString& name, EAssetType type
)
{
    m_uuid = uuid;
    m_name = name;
    m_type = type;
}

}   // namespace tkd
