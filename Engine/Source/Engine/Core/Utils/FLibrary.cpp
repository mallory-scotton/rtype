///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Utils/FLibrary.hpp>
#include <algorithm>
#ifdef TKD_SYSTEM_WINDOWS
    #include <errhandlingapi.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::unordered_map<FilePath, FLibrary::WLibraryPtr>
    FLibrary::s_loadedLibraries;

///////////////////////////////////////////////////////////////////////////////
FLibrary::FLibrary(const FilePath& path)
    : m_handle(nullptr)
    , m_path(NormalizePath(path))
    , m_name(ExtractLibraryName(m_path))
    , m_isLoaded(false)
    , m_lastError()
{}

///////////////////////////////////////////////////////////////////////////////
FLibrary::~FLibrary()
{
    if (m_isLoaded) { Unload(); }

    auto it = s_loadedLibraries.find(m_path);
    if (it != s_loadedLibraries.end() && it->second.expired())
    {
        s_loadedLibraries.erase(it);
    }
}

///////////////////////////////////////////////////////////////////////////////
FLibrary::FLibrary(FLibrary&& other) noexcept
    : m_handle(other.m_handle)
    , m_path(std::move(other.m_path))
    , m_name(std::move(other.m_name))
    , m_isLoaded(other.m_isLoaded)
    , m_lastError(std::move(other.m_lastError))
    , m_symbols(std::move(other.m_symbols))
{
    other.Reset();
}

///////////////////////////////////////////////////////////////////////////////
FLibrary& FLibrary::operator=(FLibrary&& other) noexcept
{
    if (this != &other)
    {
        Unload();

        m_handle = other.m_handle;
        m_path = std::move(other.m_path);
        m_name = std::move(other.m_name);
        m_isLoaded = other.m_isLoaded;
        m_lastError = std::move(other.m_lastError);
        m_symbols = std::move(other.m_symbols);

        other.Reset();
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
FLibrary::FLibraryPtr FLibrary::Load(const FilePath& path)
{
    FilePath normalizedPath = NormalizePath(path);

    // Check if already loaded
    auto it = s_loadedLibraries.find(normalizedPath);
    if (it != s_loadedLibraries.end())
    {
        if (auto existing = it->second.lock()) { return existing; }
        else
        {
            // Weak pointer expired, remove it
            s_loadedLibraries.erase(it);
        }
    }

    // Create new library instance
    auto library = std::shared_ptr<FLibrary>(new FLibrary(normalizedPath));

    // Try to load it
    ELibraryStatus result = library->Load();
    if (result != ELibraryStatus::Success)
    {
        return nullptr;   // Load failed
    }

    // Add to registry
    s_loadedLibraries[normalizedPath] = library;

    return library;
}

///////////////////////////////////////////////////////////////////////////////
FLibrary::FLibraryPtr FLibrary::LoadFromName(const FString& name)
{
    FilePath fileName = name.CStr();
    std::vector<FilePath> pathsToTry = {
        fileName,
        fileName / LIBRARY_EXTENSION,
        "lib" / fileName / LIBRARY_EXTENSION,
        "./lib" / fileName / LIBRARY_EXTENSION,
        "libs/" / fileName / LIBRARY_EXTENSION,
        "plugins/" / fileName / LIBRARY_EXTENSION,
    };

    for (const auto& path: pathsToTry)
    {
        if (FileSystem::FileExists(path)) { return Load(path); }
    }

    // If no file found, try to load anyway (system might find it in
    // PATH/LD_LIBRARY_PATH)
    return Load(FilePath(name.CStr()));
}

///////////////////////////////////////////////////////////////////////////////
bool FLibrary::IsLibraryLoaded(const FilePath& path)
{
    std::string normalizedPath = NormalizePath(path).string();
    auto it = s_loadedLibraries.find(normalizedPath);
    return it != s_loadedLibraries.end() && !it->second.expired();
}

///////////////////////////////////////////////////////////////////////////////
void FLibrary::UnloadAll()
{
    // Clear the registry - this will not unload libraries immediately
    // but will prevent them from being found by future Load() calls
    s_loadedLibraries.clear();
}

///////////////////////////////////////////////////////////////////////////////
ELibraryStatus FLibrary::Load(void)
{
    if (m_isLoaded) { return ELibraryStatus::AlreadyLoaded; }

    if (m_path.empty())
    {
        SetLastError("Invalid library path");
        return ELibraryStatus::InvalidPath;
    }

    // Check if file exists (skip for system libraries)
    if (!FileSystem::FileExists(m_path))
    {
        SetLastError("Library file not found: " + FString(m_path.string()));
        return ELibraryStatus::FileNotFound;
    }

    // Load the library
    m_handle = LOAD_LIBRARY_IMPL(m_path.string().c_str());
    if (!m_handle)
    {
        SetLastError("Failed to load library: " + GetSystemError());
        return ELibraryStatus::LoadFailed;
    }

    m_isLoaded = true;
    m_symbols.clear();
    return ELibraryStatus::Success;
}

///////////////////////////////////////////////////////////////////////////////
void FLibrary::Unload(void)
{
    if (m_handle && m_isLoaded) { FREE_LIBRARY_IMPL(m_handle); }
    Reset();
}

///////////////////////////////////////////////////////////////////////////////
void* FLibrary::GetRawFunction(const FString& name) const
{
    if (!m_isLoaded || !m_handle)
    {
        SetLastError("Library not loaded");
        return nullptr;
    }

    // Check cache first
    auto cacheIt = m_symbols.find(name);
    if (cacheIt != m_symbols.end()) { return cacheIt->second; }

    // Get function from library
    void* funcPtr = GET_PROC_ADDRESS_IMPL(m_handle, name.CStr());
    if (funcPtr)
    {
        // Cache the result
        m_symbols[name] = funcPtr;
    }
    else
    {
        SetLastError("Function '" + name + "' not found: " + GetSystemError());
    }

    return funcPtr;
}

///////////////////////////////////////////////////////////////////////////////
bool FLibrary::HasFunction(const FString& name) const
{
    return GetRawFunction(name) != nullptr;
}

///////////////////////////////////////////////////////////////////////////////
FilePath FLibrary::NormalizePath(const FilePath& path)
{
    std::string normalized = path.string();

    // Convert backslashes to forward slashes on all platforms
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    // Resolve relative paths
    try
    {
        if (std::filesystem::exists(normalized))
        {
            normalized = std::filesystem::absolute(normalized).string();
            std::replace(normalized.begin(), normalized.end(), '\\', '/');
        }
    }
    catch (...)
    {
        // If filesystem operations fail, just return the original path
    }

    return normalized;
}

///////////////////////////////////////////////////////////////////////////////
void FLibrary::Reset(void)
{
    m_handle = nullptr;
    m_isLoaded = false;
    m_symbols.clear();
    m_lastError.Clear();
}

///////////////////////////////////////////////////////////////////////////////
void FLibrary::SetLastError(const FString& error) const
{
    m_lastError = error;
}

///////////////////////////////////////////////////////////////////////////////
FString FLibrary::GetSystemError(void) const
{
#ifdef TKD_SYSTEM_WINDOWS
    DWORD errorCode = GetLastError();
    if (errorCode == 0) { return "Unknown error"; }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    FString message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
#else
    const char* error = dlerror();
    return error ? FString(error) : "Unknown error";
#endif
}

///////////////////////////////////////////////////////////////////////////////
FString FLibrary::ExtractLibraryName(const FilePath& path)
{
    std::string name = path.stem().string();

    // Remove "lib" prefix if present
    if (name.length() > 3 && name.substr(0, 3) == "lib")
    {
        name = name.substr(3);
    }

    return FString(name.c_str());
}

///////////////////////////////////////////////////////////////////////////////
bool FLibrary::IsLoaded(void) const { return m_isLoaded; }

///////////////////////////////////////////////////////////////////////////////
const FString& FLibrary::GetLastErrorMessage(void) const
{
    return m_lastError;
}

///////////////////////////////////////////////////////////////////////////////
const FString& FLibrary::GetName(void) const { return m_name; }

///////////////////////////////////////////////////////////////////////////////
const FilePath& FLibrary::GetPath(void) const { return m_path; }

///////////////////////////////////////////////////////////////////////////////
FString FLibrary::GetPlatformExtension(void)
{
    return FString(LIBRARY_EXTENSION);
}

}   // namespace tkd
