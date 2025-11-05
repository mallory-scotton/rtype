///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <variant>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Primitive Types
///////////////////////////////////////////////////////////////////////////////
typedef signed char Int8;        // [-128; 127]
typedef unsigned char UInt8;     // [0; 255]
typedef signed short Int16;      // [-32,768; 32,767]
typedef unsigned short UInt16;   // [0; 65,535]
typedef signed int Int32;        // [-2,147,483,648; 2,147,483,647]
typedef unsigned int UInt32;     // [0; 4,294,967,295]
typedef signed long long
    Int64;   // [-9,223,372,036,854,775,808; 9,223,372,036,854,775,807]
typedef unsigned long long UInt64;   // [0; 18,446,744,073,709,551,615]
typedef float Float32;               // IEEE 754 single precision
typedef double Float64;              // IEEE 754 double precision
typedef bool Bool;                   // true or false
typedef unsigned char Byte;          // 8 bits
typedef uintptr_t UIntPtr;   // Unsigned integer type for pointer arithmetic

///////////////////////////////////////////////////////////////////////////////
// SizeT Type
///////////////////////////////////////////////////////////////////////////////
using SizeT = decltype(sizeof(0));

///////////////////////////////////////////////////////////////////////////////
// Std Types
///////////////////////////////////////////////////////////////////////////////
using FilePath = std::filesystem::path;
using SteadyClock = std::chrono::steady_clock;
using SystemClock = std::chrono::system_clock;
template <typename Rep, typename Period = std::ratio<1>>
using TDuration = std::chrono::duration<Rep, Period>;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Microseconds = std::chrono::microseconds;
using Nanoseconds = std::chrono::nanoseconds;
using TimePoint = SteadyClock::time_point;
template <typename Args>
using TFunction = std::function<Args>;
template <typename... Types>
using TVariant = std::variant<Types...>;
template <typename T>
using TUniquePtr = std::unique_ptr<T>;
template <typename T>
using TSharedPtr = std::shared_ptr<T>;
template <typename T>
using TWeakPtr = std::weak_ptr<T>;
template <typename T>
using TAtomic = std::atomic<T>;
template <typename T>
using TOptional = std::optional<T>;

using Json = nlohmann::json;

}   // namespace tkd
