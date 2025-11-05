///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <Engine/Core/Utils/FileSystem.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#if defined(TKD_SYSTEM_WINDOWS)
    #include <windows.h>
    #define LIBRARY_HANDLE HMODULE
    #define LOAD_LIBRARY_IMPL(path) LoadLibraryA(path)
    #define GET_PROC_ADDRESS_IMPL GetProcAddress
    #define FREE_LIBRARY_IMPL FreeLibrary
    #define GET_LAST_ERROR_IMPL() GetLastError()
    #define LIBRARY_EXTENSION ".dll"
#else
    #include <dlfcn.h>
    #define LIBRARY_HANDLE void*
    #define LOAD_LIBRARY_IMPL(path) dlopen(path, RTLD_LAZY)
    #define GET_PROC_ADDRESS_IMPL dlsym
    #define FREE_LIBRARY_IMPL dlclose
    #define GET_LAST_ERROR_IMPL() dlerror()
    #define LIBRARY_EXTENSION ".so"
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Status codes for dynamic library operations
///
///////////////////////////////////////////////////////////////////////////////
enum class ELibraryStatus : UInt8
{
    Success,          //<! Operation succeeded
    FileNotFound,     //<! File not found
    LoadFailed,       //<! Library load failed
    SymbolNotFound,   //<! Symbol not found
    AlreadyLoaded,    //<! Library is already loaded
    NotLoaded,        //<! Library is not loaded
    InvalidPath,      //<! Invalid library path
    UnknownError      //<! Unknown error occurred
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class FLibrary
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Type Definitions
    ///////////////////////////////////////////////////////////////////////////
    using FLibraryPtr = std::shared_ptr<FLibrary>;   //<! Shared pointer type
    using WLibraryPtr = std::weak_ptr<FLibrary>;     //<! Weak pointer type

private:
    ///////////////////////////////////////////////////////////////////////////
    // Static Class Member
    ///////////////////////////////////////////////////////////////////////////
    static std::unordered_map<FilePath, WLibraryPtr>
        s_loadedLibraries;   // <! Map of loaded libraries

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    LIBRARY_HANDLE m_handle;       //<! Handle to the loaded library
    FilePath m_path;               //<! Path to the library file
    FString m_name;                //<! Name of the library
    bool m_isLoaded;               //<! Flag indicating the loading status
    mutable FString m_lastError;   //<! Last error message
    mutable std::unordered_map<FString, void*> m_symbols;   //<! Cached symbols

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor that initializes the FLibrary with the given path.
    ///
    /// \param path Path to the dynamic library
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit FLibrary(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Destructor that unloads the library if it is loaded.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~FLibrary();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move constructor to transfer ownership of the library.
    ///
    /// \param other The FLibrary instance to move from
    ///
    ///////////////////////////////////////////////////////////////////////////
    FLibrary(FLibrary&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Deleted copy constructor to prevent copying of FLibrary
    ///
    ///////////////////////////////////////////////////////////////////////////
    FLibrary(const FLibrary&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move assignment operator to transfer ownership of the library.
    ///
    /// \param other The FLibrary instance to move from
    ///
    /// \return Reference to the current FLibrary instance
    ///
    ///////////////////////////////////////////////////////////////////////////
    FLibrary& operator=(FLibrary&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Deleted copy assignment operator to prevent copying of FLibrary
    ///
    ///////////////////////////////////////////////////////////////////////////
    FLibrary& operator=(const FLibrary&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load a library from the specified path.
    ///
    /// \param path Path to the dynamic library
    ///
    /// \return A shared pointer to the loaded FLibrary instance, or nullptr if
    /// loading failed
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FLibraryPtr Load(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Load a library by its name, searching in standard locations.
    ///
    /// \param name The name of the library to load
    ///
    /// \return A shared pointer to the loaded FLibrary instance, or nullptr if
    /// loading failed
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FLibraryPtr LoadFromName(const FString& name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a library is already loaded.
    ///
    /// \param path Path to the dynamic library
    ///
    /// \return True if the library is loaded, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool IsLibraryLoaded(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unloads all loaded libraries.
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void UnloadAll(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the platform-specific library file extension.
    ///
    /// \return The library file extension for the current platform
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString GetPlatformExtension(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Normalizes the given library path to ensure it has the correct
    /// extension and format for the current platform.
    ///
    /// \param path The library path to normalize
    ///
    /// \return The normalized library path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath NormalizePath(const FilePath& path);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Loads the library if it is not already loaded.
    ///
    /// \return ELibraryStatus indicating the result of the operation
    ///
    ///////////////////////////////////////////////////////////////////////////
    ELibraryStatus Load(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Unloads the currently loaded library.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Unload(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the library is currently loaded.
    ///
    /// \return True if the library is loaded, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool IsLoaded(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a raw function pointer from the loaded library.
    ///
    /// \param name The name of the function to retrieve
    ///
    /// \return Pointer to the function, or nullptr if not found
    ///
    ///////////////////////////////////////////////////////////////////////////
    void* GetRawFunction(const FString& name) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the library contains a function with the specified
    /// name.
    ///
    /// \param name The name of the function to check
    ///
    /// \return True if the function is found, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool HasFunction(const FString& name) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the path of the loaded library.
    ///
    /// \return The path of the library
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FilePath& GetPath(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the name of the library.
    ///
    /// \return The name of the library
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FString& GetName(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the last error message.
    ///
    /// \return The last error message
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FString& GetLastErrorMessage(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a function from the loaded library and casts it to the
    /// specified function pointer type.
    ///
    /// \tparam FuncType Function pointer type (e.g., void(*)())
    ///
    /// \param name Name of the function to retrieve
    /// \param out Pointer to store the retrieved function
    ///
    /// \return ELibraryStatus indicating the result of the operation
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename FuncType>
    ELibraryStatus GetFunction(const FString& name, FuncType*& out) const
    {
        // Check if the library is loaded
        if (!m_isLoaded)
        {
            SetLastError("Library is not loaded");
            return ELibraryStatus::NotLoaded;
        }

        // Check if the function is already cached
        void* symbol = GetRawFunction(name);
        if (!symbol)
        {
            SetLastError("Function '" + name + "' not found");
            return ELibraryStatus::SymbolNotFound;
        }

        // Cast the symbol to the requested function type
        out = reinterpret_cast<FuncType*>(symbol);
        return ELibraryStatus::Success;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves a function from the loaded library and wraps it in a
    /// std::function for easier usage.
    ///
    /// \tparam FuncType Function signature (e.g., void())
    ///
    /// \param name Name of the function to retrieve
    ///
    /// \return std::function wrapping the retrieved function, or nullptr if
    /// the function is not found or the library is not loaded
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename FuncType>
    std::function<FuncType> GetFunctionWrapper(const FString& name) const
    {
        FuncType* func = nullptr;
        if (GetFunction(name, func) != ELibraryStatus::Success || !func)
        {
            return nullptr;
        }
        return std::function<FuncType>(func);
    }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Resets the FLibrary instance to its initial state.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void Reset(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the last error message.
    ///
    /// \param error The error message to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetLastError(const FString& error) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Retrieves the last system error message.
    ///
    /// \return The last system error message
    ///
    ///////////////////////////////////////////////////////////////////////////
    FString GetSystemError(void) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Extracts the library name from the given path.
    ///
    /// \param path The path to extract the library name from
    ///
    /// \return The extracted library name
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString ExtractLibraryName(const FilePath& path);
};

}   // namespace tkd
