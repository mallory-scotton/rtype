///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Utils/FileSystem.hpp>
#include <algorithm>
#include <Engine/Config.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <time.h>
#if defined(TKD_SYSTEM_WINDOWS)
    #include <Windows.h>
#elif defined(TKD_SYSTEM_LINUX)
    #include <limits.h>
    #include <unistd.h>
#elif defined(TKD_SYSTEM_MACOS)
    #include <limits.h>
    #include <mach-o/dyld.h>
    #include <unistd.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Alias Member
///////////////////////////////////////////////////////////////////////////////
namespace fs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::Mkdir(const FilePath& path)
{
    std::error_code ec;
    return fs::create_directory(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::Mkdirs(const FilePath& path)
{
    std::error_code ec;
    return fs::create_directories(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::FileExists(const FilePath& path)
{
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_regular_file(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::DirectoryExists(const FilePath& path)
{
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_directory(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::IsDirectory(const FilePath& path)
{
    std::error_code ec;
    return fs::is_directory(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::IsFile(const FilePath& path)
{
    std::error_code ec;
    return fs::is_regular_file(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::IsReadable(const FilePath& path)
{
    std::error_code ec;
    auto perms = fs::status(path, ec).permissions();
    if (ec) { return false; }
#ifdef TKD_SYSTEM_WINDOWS
    return (perms & fs::perms::owner_read) != fs::perms::none;
#else
    return (perms & fs::perms::group_read) != fs::perms::none ||
           (perms & fs::perms::others_read) != fs::perms::none ||
           (perms & fs::perms::owner_read) != fs::perms::none;
#endif
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::IsWritable(const FilePath& path)
{
    std::error_code ec;
    auto perms = fs::status(path, ec).permissions();
    if (ec) { return false; }
#ifdef TKD_SYSTEM_WINDOWS
    return (perms & fs::perms::owner_write) != fs::perms::none;
#else
    return (perms & fs::perms::group_write) != fs::perms::none ||
           (perms & fs::perms::others_write) != fs::perms::none ||
           (perms & fs::perms::owner_write) != fs::perms::none;
#endif
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::IsExecutable(const FilePath& path)
{
    std::error_code ec;
    auto perms = fs::status(path, ec).permissions();
    if (ec) { return false; }
#ifdef TKD_SYSTEM_WINDOWS
    return (perms & fs::perms::owner_exec) != fs::perms::none;
#else
    return (perms & fs::perms::group_exec) != fs::perms::none ||
           (perms & fs::perms::others_exec) != fs::perms::none ||
           (perms & fs::perms::owner_exec) != fs::perms::none;
#endif
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::CreateFile(const FilePath& path)
{
    std::error_code ec;
    if (fs::exists(path, ec) || ec) { return false; }
    std::ofstream file(path);
    return file.good();
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::CreateDirectory(const FilePath& path)
{
    std::error_code ec;
    return fs::create_directory(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::CreateDirectories(const FilePath& path)
{
    std::error_code ec;
    return fs::create_directories(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::RemoveFile(const FilePath& path)
{
    std::error_code ec;
    return fs::remove(path, ec) && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::RemoveDirectory(const FilePath& path)
{
    std::error_code ec;
    return fs::remove_all(path, ec) > 0 && !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::CopyFile(const FilePath& source, const FilePath& destination)
{
    std::error_code ec;
    fs::copy_file(
        source, destination, fs::copy_options::overwrite_existing, ec
    );
    return !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::CopyDirectory(
    const FilePath& source, const FilePath& destination, bool recursive
)
{
    std::error_code ec;
    if (!fs::exists(source, ec) || !fs::is_directory(source, ec) || ec)
    {
        return false;
    }
    if ((fs::exists(destination, ec) && !fs::is_directory(destination, ec)) ||
        ec)
    {
        return false;
    }
    if (!fs::exists(destination, ec))
    {
        if (!fs::create_directories(destination, ec) || ec) { return false; }
    }
    for (const auto& entry: fs::directory_iterator(source, ec))
    {
        if (ec) { return false; }
        const auto& path = entry.path();
        auto dest = destination / path.filename();
        if (fs::is_directory(path, ec))
        {
            if (ec) { return false; }
            if (recursive)
            {
                if (!CopyDirectory(path, dest, true)) { return false; }
            }
        }
        else if (fs::is_regular_file(path, ec))
        {
            if (ec) { return false; }
            fs::copy_file(
                path, dest, fs::copy_options::overwrite_existing, ec
            );
            if (ec) { return false; }
        }
        else
        {
            // Skip other types (symlinks, etc.)
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::MoveFile(const FilePath& source, const FilePath& destination)
{
    std::error_code ec;
    fs::rename(source, destination, ec);
    return !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::MoveDirectory(
    const FilePath& source, const FilePath& destination
)
{
    std::error_code ec;
    fs::rename(source, destination, ec);
    return !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::RenameFile(const FilePath& oldName, const FilePath& newName)
{
    std::error_code ec;
    fs::rename(oldName, newName, ec);
    return !ec;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::RenameDirectory(
    const FilePath& oldName, const FilePath& newName
)
{
    std::error_code ec;
    fs::rename(oldName, newName, ec);
    return !ec;
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetCurrentWorkingDirectory(void)
{
    std::error_code ec;
    auto path = fs::current_path(ec);
    if (ec) { return FilePath(); }
    return path;
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetExecutablePath(void)
{
#ifdef TKD_SYSTEM_WINDOWS
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) { return FilePath(); }
    return FilePath(std::string(buffer, length));
#elif defined(TKD_SYSTEM_LINUX)
    char buffer[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length == -1) { return FilePath(); }
    buffer[length] = '\0';
    return FilePath(std::string(buffer));
#elif defined(TKD_SYSTEM_MACOS)
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) { return FilePath(); }
    return FilePath(std::string(buffer));
#else
    return FilePath();
#endif
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetExecutableDirectory(void)
{
    auto execPath = GetExecutablePath();
    if (execPath.empty()) { return FilePath(); }
    return execPath.parent_path();
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetHomeDirectory(void)
{
#ifdef TKD_SYSTEM_WINDOWS
    const char* homeDrive = std::getenv("HOMEDRIVE");
    const char* homePath = std::getenv("HOMEPATH");
    if (homeDrive && homePath)
    {
        return FilePath(std::string(homeDrive) + std::string(homePath));
    }
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) { return FilePath(std::string(userProfile)); }
    return FilePath();
#else
    const char* home = std::getenv("HOME");
    if (home) { return FilePath(std::string(home)); }
    return FilePath();
#endif
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetTempDirectory(void)
{
    std::error_code ec;
    auto tempPath = fs::temp_directory_path(ec);
    if (ec) { return FilePath(); }
    return tempPath;
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetLocalAppDataDirectory(void)
{
#ifdef TKD_SYSTEM_WINDOWS
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData) { return FilePath(std::string(localAppData)); }
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile)
    {
        return FilePath(std::string(userProfile) + "\\AppData\\Local");
    }
    return FilePath();
#else
    // On Unix systems, use XDG_DATA_HOME or fallback to ~/.local/share
    const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
    if (xdgDataHome) { return FilePath(std::string(xdgDataHome)); }
    auto home = GetHomeDirectory();
    if (!home.empty()) { return home / ".local" / "share"; }
    return FilePath();
#endif
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetRoamingAppDataDirectory(void)
{
#ifdef TKD_SYSTEM_WINDOWS
    const char* appData = std::getenv("APPDATA");
    if (appData) { return FilePath(std::string(appData)); }
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile)
    {
        return FilePath(std::string(userProfile) + "\\AppData\\Roaming");
    }
    return FilePath();
#else
    // On Unix systems, use XDG_CONFIG_HOME or fallback to ~/.config
    const char* xdgConfigHome = std::getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome) { return FilePath(std::string(xdgConfigHome)); }
    auto home = GetHomeDirectory();
    if (!home.empty()) { return home / ".config"; }
    return FilePath();
#endif
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetLastAppDataDirectory(void)
{
#ifdef TKD_SYSTEM_WINDOWS
    const char* programData = std::getenv("PROGRAMDATA");
    if (programData) { return FilePath(std::string(programData)); }
    const char* allUsersProfile = std::getenv("ALLUSERSPROFILE");
    if (allUsersProfile) { return FilePath(std::string(allUsersProfile)); }
    return FilePath("C:\\ProgramData");
#else
    // On Unix systems, try common system-wide data directories
    if (fs::exists("/usr/local/share"))
    {
        return FilePath("/usr/local/share");
    }
    if (fs::exists("/usr/share")) { return FilePath("/usr/share"); }
    return FilePath("/usr/local/share");
#endif
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetUniqueTempFile(
    const FilePath& prefix, const FilePath& suffix
)
{
    auto tempDir = GetTempDirectory();
    if (tempDir.empty()) { return FilePath(); }
    for (int i = 0; i < 100; ++i)
    {
        auto uniqueName =
            prefix.string() + std::to_string(std::rand()) + suffix.string();
        auto tempFilePath = tempDir / uniqueName;
        if (!fs::exists(tempFilePath)) { return tempFilePath; }
    }
    return FilePath();
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetUniqueTempDirectory(const FilePath& prefix)
{
    auto tempDir = GetTempDirectory();
    if (tempDir.empty()) { return FilePath(); }
    for (int i = 0; i < 100; ++i)
    {
        auto uniqueName = prefix.string() + std::to_string(std::rand());
        auto tempDirPath = tempDir / uniqueName;
        if (!fs::exists(tempDirPath))
        {
            if (fs::create_directory(tempDirPath)) { return tempDirPath; }
        }
    }
    return FilePath();
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetAbsolutePath(const FilePath& path)
{
    std::error_code ec;
    auto absPath = fs::absolute(path, ec);
    if (ec) { return FilePath(); }
    return absPath;
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath
    FileSystem::GetRelativePath(const FilePath& from, const FilePath& to)
{
    std::error_code ec;
    auto relPath = fs::relative(to, from, ec);
    if (ec) { return FilePath(); }
    return relPath;
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetParentPath(const FilePath& path)
{
    return path.parent_path();
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetFileName(const FilePath& path)
{
    return path.filename();
}

///////////////////////////////////////////////////////////////////////////////
FileSystem::FilePath FileSystem::GetFileExtension(const FilePath& path)
{
    return path.extension();
}

///////////////////////////////////////////////////////////////////////////////
SizeT FileSystem::GetFileSize(const FilePath& path)
{
    std::error_code ec;
    auto size = fs::file_size(path, ec);
    if (ec) { return static_cast<SizeT>(-1); }
    return size;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath> FileSystem::ListFiles(const FilePath& path)
{
    std::vector<FilePath> files;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return files;
    }
    for (const auto& entry: fs::directory_iterator(path, ec))
    {
        if (ec) { return files; }
        if (fs::is_regular_file(entry.path(), ec) && !ec)
        {
            files.push_back(entry.path());
        }
    }
    return files;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath>
    FileSystem::ListDirectories(const FilePath& path)
{
    std::vector<FilePath> directories;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return directories;
    }
    for (const auto& entry: fs::directory_iterator(path, ec))
    {
        if (ec) { return directories; }
        if (fs::is_directory(entry.path(), ec) && !ec)
        {
            directories.push_back(entry.path());
        }
    }
    return directories;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath> FileSystem::ListEntries(const FilePath& path)
{
    std::vector<FilePath> entries;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return entries;
    }
    for (const auto& entry: fs::directory_iterator(path, ec))
    {
        if (ec) { return entries; }
        entries.push_back(entry.path());
    }
    return entries;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath> FileSystem::ListFilesByExtension(
    const FilePath& path, const FilePath& extension
)
{
    std::vector<FilePath> files;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return files;
    }
    for (const auto& entry: fs::directory_iterator(path, ec))
    {
        if (ec) { return files; }
        if (fs::is_regular_file(entry.path(), ec) && !ec &&
            entry.path().extension() == extension)
        {
            files.push_back(entry.path());
        }
    }
    return files;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath>
    FileSystem::RecursiveListFiles(const FilePath& path)
{
    std::vector<FilePath> files;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return files;
    }
    for (const auto& entry: fs::recursive_directory_iterator(
             path, fs::directory_options::skip_permission_denied, ec
         ))
    {
        if (ec) { return files; }
        if (fs::is_regular_file(entry.path(), ec) && !ec)
        {
            files.push_back(entry.path());
        }
    }
    return files;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath>
    FileSystem::RecursiveListDirectories(const FilePath& path)
{
    std::vector<FilePath> directories;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return directories;
    }
    for (const auto& entry: fs::recursive_directory_iterator(
             path, fs::directory_options::skip_permission_denied, ec
         ))
    {
        if (ec) { return directories; }
        if (fs::is_directory(entry.path(), ec) && !ec)
        {
            directories.push_back(entry.path());
        }
    }
    return directories;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath>
    FileSystem::RecursiveListEntries(const FilePath& path)
{
    std::vector<FilePath> entries;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return entries;
    }
    for (const auto& entry: fs::recursive_directory_iterator(
             path, fs::directory_options::skip_permission_denied, ec
         ))
    {
        if (ec) { return entries; }
        entries.push_back(entry.path());
    }
    return entries;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<FileSystem::FilePath> FileSystem::RecursiveListFilesByExtension(
    const FilePath& path, const FilePath& extension
)
{
    std::vector<FilePath> files;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec) || ec)
    {
        return files;
    }
    for (const auto& entry: fs::recursive_directory_iterator(
             path, fs::directory_options::skip_permission_denied, ec
         ))
    {
        if (ec) { return files; }
        if (fs::is_regular_file(entry.path(), ec) && !ec &&
            entry.path().extension() == extension)
        {
            files.push_back(entry.path());
        }
    }
    return files;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<UInt8> FileSystem::ReadFileToBytes(const FilePath& path)
{
    std::vector<UInt8> data;
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_regular_file(path, ec) || ec)
    {
        return data;
    }
    std::ifstream file(path, std::ios::binary);
    if (!file) { return data; }
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    data.resize(size);
    if (!file.read(reinterpret_cast<char*>(data.data()), size))
    {
        data.clear();
    }
    return data;
}

///////////////////////////////////////////////////////////////////////////////
FString FileSystem::ReadFileToString(const FilePath& path)
{
    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_regular_file(path, ec) || ec)
    {
        return FString();
    }
    std::ifstream file(path);
    if (!file) { return FString(); }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str().c_str();
}

///////////////////////////////////////////////////////////////////////////////
FString FileSystem::ReadFile(const FilePath& path)
{
    return ReadFileToString(path);
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::WriteBytesToFile(
    const FilePath& path, const std::vector<UInt8>& data
)
{
    std::ofstream file(path, std::ios::binary);
    if (!file) { return false; }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::WriteStringToFile(const FilePath& path, const FString& data)
{
    std::ofstream file(path);
    if (!file) { return false; }
    file << data.CStr();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::WriteFile(const FilePath& path, const FString& data)
{
    return WriteStringToFile(path, data);
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::AppendBytesToFile(
    const FilePath& path, const std::vector<UInt8>& data
)
{
    std::ofstream file(path, std::ios::binary | std::ios::app);
    if (!file) { return false; }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::AppendStringToFile(const FilePath& path, const FString& data)
{
    std::ofstream file(path, std::ios::app);
    if (!file) { return false; }
    file << data.CStr();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FileSystem::AppendToFile(const FilePath& path, const FString& data)
{
    return AppendStringToFile(path, data);
}

}   // namespace tkd
