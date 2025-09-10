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
// Ensure minimum C++ language standard version is met
///////////////////////////////////////////////////////////////////////////////
#if (defined(_MSVC_LANG) && _MSVC_LANG < 201703L) || (!defined(_MSVC_LANG) && __cplusplus < 201703L)
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
// Types definition
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config/Types.hpp>
