///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Version
///////////////////////////////////////////////////////////////////////////////
#define TKD_VERSION_MAJOR      1
#define TKD_VERSION_MINOR      0
#define TKD_VERSION_PATCH      0
#define TKD_VERSION_IS_RELEASE false

///////////////////////////////////////////////////////////////////////////////
// Version String
///////////////////////////////////////////////////////////////////////////////
#define TKD_SHORT_VERSION_STRING TKD_VERSION_MAJOR "." TKD_VERSION_MINOR
#define TKD_VERSION_STRING TKD_SHORT_VERSION_STRING "." TKD_VERSION_PATCH

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#if defined(TKD_EXTRA_CONFIG)
    #include TKD_EXTRA_CONFIG
#endif

///////////////////////////////////////////////////////////////////////////////
// Identify the operating system
///////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
    #define TKD_SYSTEM_WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#elif defined(__APPLE__) && defined(__MACH__)
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define TKD_SYSTEM_IOS
    #elif TARGET_OS_MAC
        #define TKD_SYSTEM_MACOS
    #else
        #error This Apple operating system is not supported by TKD library
    #endif
#elif defined(__unix__)
    #if defined(__ANDROID__)
        #define TKD_SYSTEM_ANDROID
    #elif defined(__linux__)
        #define TKD_SYSTEM_LINUX
    #elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
        #define TKD_SYSTEM_FREEBSD
    #elif defined(__OpenBSD__)
        #define TKD_SYSTEM_OPENBSD
    #elif defined(__NetBSD__)
        #define TKD_SYSTEM_NETBSD
    #else
        #error This UNIX operating system is not supported by TKD library
    #endif
#else
    #error This operating system is not supported by TKD library
#endif

///////////////////////////////////////////////////////////////////////////////
// Weak linking support
///////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__) || defined(__clang__)
    #define TKD_WEAK __attribute__((__weak__))
#elif defined(_MSC_VER)
    #define TKD_WEAK __declspec(selectany)
#else
    #define TKD_WEAK
#endif

///////////////////////////////////////////////////////////////////////////////
// Ensure minimum C++ language standard version is met
///////////////////////////////////////////////////////////////////////////////
#if (defined(_MSVC_LANG) && _MSVC_LANG < 201703L) || \
    (!defined(_MSVC_LANG) && __cplusplus < 201703L)
    #error "Enable C++17 or newer for your compiler"
#endif

///////////////////////////////////////////////////////////////////////////////
// Portable debug macro
///////////////////////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
    #define TKD_DEBUG
#endif

///////////////////////////////////////////////////////////////////////////////
// Helpers to create portable import / export macros for each module
///////////////////////////////////////////////////////////////////////////////
#if !defined(TKD_STATIC)
    #if defined(TKD_SYSTEM_WINDOWS)
        #define TKD_API_EXPORT __declspec(dllexport)
        #define TKD_API_IMPORT __declspec(dllimport)
        #ifdef _MSC_VER
            // Using standard library types in our own exported types is okay
            #pragma warning(disable : 4251)
            // Exporting types derived from the standard library is okay
            #pragma warning(disable : 4275)
        #endif
    #else
        #define TKD_API_EXPORT __attribute__((__visibility__("default")))
        #define TKD_API_IMPORT __attribute__((__visibility__("default")))
    #endif
#else
    #define TKD_API_EXPORT
    #define TKD_API_IMPORT
#endif

///////////////////////////////////////////////////////////////////////////////
// Force Inline Configuration
///////////////////////////////////////////////////////////////////////////////
#if defined(TKD_FORCE_INLINE)
    #define FORCEINLINE inline
#else
    #define FORCEINLINE
#endif

///////////////////////////////////////////////////////////////////////////////
// Engine Build Type
///////////////////////////////////////////////////////////////////////////////
#if defined(TKD_ENGINE_WITH_GRAPHICS)
    #define TKD_ENGINE_SERVER false
    #define TKD_ENGINE_CLIENT true
    #define TKD_ENGINE_IF_CLIENT(code) code
    #define TKD_ENGINE_IF_SERVER(code)
#else
    #define TKD_ENGINE_SERVER true
    #define TKD_ENGINE_CLIENT false
    #define TKD_ENGINE_IF_CLIENT(code)
    #define TKD_ENGINE_IF_SERVER(code) code
#endif

///////////////////////////////////////////////////////////////////////////////
// Utility to define unused variables and avoid warnings
///////////////////////////////////////////////////////////////////////////////
#define TKD_UNUSED(x) (void)(x)

///////////////////////////////////////////////////////////////////////////////
// Maybe unused attribute to avoid warnings
///////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__) || defined(__clang__)
    #define TKD_MAYBE_UNUSED __attribute__((unused))
#else
    #define TKD_MAYBE_UNUSED
#endif

///////////////////////////////////////////////////////////////////////////////
// Disable copy constructors and assignment operators
///////////////////////////////////////////////////////////////////////////////
#define TKD_DISABLE_COPY(Class)          \
    Class(const Class&) = delete;       \
    Class& operator=(const Class&) = delete;

///////////////////////////////////////////////////////////////////////////////
// Disable move constructors and assignment operators
///////////////////////////////////////////////////////////////////////////////
#define TKD_DISABLE_MOVE(Class)          \
    Class(Class&&) = delete;            \
    Class& operator=(Class&&) = delete;

///////////////////////////////////////////////////////////////////////////////
// Disable copy and move constructors and assignment operators
///////////////////////////////////////////////////////////////////////////////
#define TKD_DISABLE_COPY_MOVE(Class)     \
    TKD_DISABLE_COPY(Class)             \
    TKD_DISABLE_MOVE(Class)

///////////////////////////////////////////////////////////////////////////////
// No discard attribute to avoid warnings
///////////////////////////////////////////////////////////////////////////////
#if defined(__GNUC__) || defined(__clang__)
    #define TKD_NODISCARD [[nodiscard]]
#else
    #define TKD_NODISCARD
#endif

///////////////////////////////////////////////////////////////////////////////
// Types, Macros and Constants definition
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config/Constants.hpp>
#include <Engine/Config/Macros.hpp>
#include <Engine/Config/Types.hpp>
