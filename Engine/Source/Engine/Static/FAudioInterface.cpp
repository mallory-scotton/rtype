///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Static/FAudioInterface.hpp>
#include <shared_mutex>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
TUniquePtr<IAudioManager> FAudioInterface::s_manager = nullptr;
std::shared_mutex FAudioInterface::s_mutex;

///////////////////////////////////////////////////////////////////////////////
Bool FAudioInterface::Initialize(void)
{
    std::lock_guard lock(s_mutex);
#if TKD_ENGINE_CLIENT
    if (s_manager) { return true; }
    s_manager = std::make_unique<SFML::AudioManager>();
    if (!s_manager || !s_manager->Initialize()) { return false; }
#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////
IAudioManager* FAudioInterface::GetAudioManager(void)
{
    return s_manager.get();
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound(
    const FilePath& filePath, Float32 volume, Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound(filePath, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound(
    const std::vector<Byte>& data, Float32 volume, Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound(data, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound(
    const Byte* data, SizeT size, Float32 volume, Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound(data, size, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource>
    FAudioInterface::PlaySound(const UAsset* asset, Float32 volume, Bool loop)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound(asset, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound3D(
    const FilePath& filePath,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound3D(filePath, position, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound3D(
    const std::vector<Byte>& data,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound3D(data, position, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound3D(
    const Byte* data,
    SizeT size,
    const FVector3& position,
    Float32 volume,
    Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound3D(data, size, position, volume, loop);
}

///////////////////////////////////////////////////////////////////////////////
TSharedPtr<IAudioSource> FAudioInterface::PlaySound3D(
    const UAsset* asset, const FVector3& position, Float32 volume, Bool loop
)
{
    std::shared_lock lock(s_mutex);
    if (!s_manager) { return nullptr; }
    return s_manager->PlaySound3D(asset, position, volume, loop);
}

}   // namespace tkd
