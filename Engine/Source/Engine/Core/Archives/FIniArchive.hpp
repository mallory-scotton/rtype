///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cctype>
#include <Engine/Config.hpp>
#include <exception>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief INI archive class for reading and writing INI files
///
///////////////////////////////////////////////////////////////////////////////
class FIniArchive
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Configuration for the INI archive format
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Config
    {
        char commentChar = ';';            //<! Comment character
        char alternateCommentChar = '#';   //<! Alternate comment character
        char sectionStartChar = '[';       //<! Section start character
        char sectionEndChar = ']';         //<! Section end character
        char assignmentChar = '=';         //<! Assignment character
        bool allowEmptyValues = true;      //<! Allow keys with empty values
        bool caseSentitive = false;   //<! Case sensitive keys and sections
        bool trimWhitespace =
            true;    //<! Trim whitespace around keys and values
        bool allowDuplicateKeys =
            false;   //<! Allow duplicate keys in a section
        bool allowMultiLineValues = false;   //<! Allow multi-line values
        std::string multilineContinuation =
            "\\";   //<! Multi-line continuation character
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    class Value
    {
    private:
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        std::string m_value;   //<! Value string

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        ///////////////////////////////////////////////////////////////////////
        Value(void) = default;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Construct a Value from a string
        ///
        /// \param value The string value
        ///
        ///////////////////////////////////////////////////////////////////////
        Value(const std::string& value);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Construct a Value from various types using template magic
        ///
        /// \tparam T The type of the value to construct from
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        Value(const T& value)
        {
            if constexpr (std::is_same_v<T, std::string>) { m_value = value; }
            else if constexpr (std::is_same_v<T, bool>)
            {
                m_value = value ? "true" : "false";
            }
            else if constexpr (std::is_arithmetic_v<T>)
            {
                m_value = std::to_string(value);
            }
        }

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to a string
        ///
        ///////////////////////////////////////////////////////////////////////
        operator std::string(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to an integer
        ///
        ///////////////////////////////////////////////////////////////////////
        operator int(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to a boolean
        ///
        ///////////////////////////////////////////////////////////////////////
        operator bool(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to a float
        ///
        ///////////////////////////////////////////////////////////////////////
        operator float(void) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Assign a string value
        ///
        /// \param value The string value to assign
        ///
        /// \return Reference to this Value
        ///
        ///////////////////////////////////////////////////////////////////////
        Value& operator=(const std::string& value);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Assign a value using template magic
        ///
        /// \tparam T The type of the value to assign
        ///
        /// \param value The value to assign
        ///
        /// \return Reference to this Value
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        Value& operator=(const T& value)
        {
            *this = Value(value);
            return *this;
        }

    private:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to the specified type using template magic
        ///
        /// \tparam T The type to convert to
        ///
        /// \return The converted value
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T ConvertTo(void) const
        {
            if constexpr (std::is_same_v<T, std::string>) { return m_value; }
            else if constexpr (std::is_same_v<T, bool>)
            {
                std::string lowerValue = m_value;
                std::transform(
                    lowerValue.begin(),
                    lowerValue.end(),
                    lowerValue.begin(),
                    ::tolower
                );
                return lowerValue == "true" || lowerValue == "1" ||
                       lowerValue == "yes" || lowerValue == "on";
            }
            else if constexpr (std::is_integral_v<T>)
            {
                try
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        return static_cast<T>(std::stoll(m_value));
                    }
                    else { return static_cast<T>(std::stoull(m_value)); }
                }
                catch (const std::exception&)
                {
                    throw std::runtime_error(
                        "Cannot convert '" + m_value + "' to integer type"
                    );
                }
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                try
                {
                    if constexpr (std::is_same_v<T, float>)
                    {
                        return std::stof(m_value);
                    }
                    else if constexpr (std::is_same_v<T, double>)
                    {
                        return std::stod(m_value);
                    }
                    else { return static_cast<T>(std::stold(m_value)); }
                }
                catch (const std::exception&)
                {
                    throw std::runtime_error(
                        "Cannot convert '" + m_value +
                        "' to floating point type"
                    );
                }
            }
            else
            {
                static_assert(
                    std::is_same_v<T, void>,
                    "Unsupported type for INI value conversion"
                );
            }
        }

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to the specified type using template magic
        ///
        /// \tparam T The type to convert to
        ///
        /// \return The converted value
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T As(void) const
        {
            return ConvertTo<T>();
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to the specified type using template
        /// magic, returning a default value if conversion fails
        ///
        /// \tparam T The type to convert to
        ///
        /// \param defaultValue
        ///
        /// \return The converted value or the default value if conversion
        /// fails
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T As(const T& defaultValue) const
        {
            return AsOr<T>(defaultValue);
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Convert the value to the specified type using template
        /// magic, returning a default value if conversion fails
        ///
        /// \tparam T The type to convert to
        ///
        /// \param defaultValue
        ///
        /// \return The converted value or the default value if conversion
        /// fails
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T AsOr(const T& defaultValue) const
        {
            try
            {
                return ConvertTo<T>();
            }
            catch (const std::exception&)
            {
                return defaultValue;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Check if the value is empty
        ///
        /// \return True if the value is empty, false otherwise
        ///
        ///////////////////////////////////////////////////////////////////////
        bool Empty(void) const noexcept;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get the string representation of the value
        ///
        /// \return The string representation of the value
        ///
        ///////////////////////////////////////////////////////////////////////
        const std::string& Str(void) const noexcept;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Section class representing a section in the INI file
    ///
    ///////////////////////////////////////////////////////////////////////////
    class Section
    {
    private:
        ///////////////////////////////////////////////////////////////////////
        // Class Members
        ///////////////////////////////////////////////////////////////////////
        std::unordered_map<std::string, Value> m_keys;   //<! Key-value pairs
        std::vector<std::string> m_keyOrder;             //<! Order of keys
        bool m_caseSensitive;   //<! Case sensitivity flag

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /// \param caseSensitive True if the section is case sensitive, false
        /// otherwise
        ///
        ///////////////////////////////////////////////////////////////////////
        Section(bool caseSensitive = false);

    private:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Normalize a key based on the case sensitivity setting
        ///
        /// \param key The key to normalize
        ///
        /// \return The normalized key
        ///
        ///////////////////////////////////////////////////////////////////////
        std::string NormalizeKey(const std::string& key) const;

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Set a key-value pair in the section
        ///
        /// \param key The key to set
        /// \param value The value to set
        ///
        ///////////////////////////////////////////////////////////////////////
        void Set(const std::string& key, const Value& value);

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get a value by key
        ///
        /// \param key The key to get
        ///
        /// \return The value associated with the key, if it exists
        ///
        ///////////////////////////////////////////////////////////////////////
        std::optional<Value> Get(const std::string& key) const;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get a value by key, returning a default value if the key
        /// does not exist
        ///
        /// \tparam T The type to get
        ///
        /// \param key The key to get
        /// \param defaultValue The default value to return if the key does not
        /// exist
        ///
        /// \return The value associated with the key or the default value
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T GetOr(const std::string& key, const T& defaultValue) const
        {
            auto value = Get(key);
            return value ? value->AsOr<T>(defaultValue) : defaultValue;
        }
    };
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare two INI values for equality
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool operator==(const FIniArchive::Value& lhs, const FIniArchive::Value& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare two INI values for inequality
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are not equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FIniArchive::Value& lhs, const FIniArchive::Value& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare the INI value with a string value
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool operator==(const std::string& lhs, const FIniArchive::Value& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare the INI value with a string value
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are not equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool operator!=(const std::string& lhs, const FIniArchive::Value& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare the INI value with a string value
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool operator==(const FIniArchive::Value& lhs, const std::string& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare the INI value with a string value
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are not equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FIniArchive::Value& lhs, const std::string& rhs);

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare the INI value with another value using template magic
///
/// \tparam T The type to compare to
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
typename std::enable_if_t<
    !std::is_same_v<T, FIniArchive::Value> &&
        !std::is_same_v<T, std::string> && std::is_arithmetic_v<T>,
    bool>
    operator==(const FIniArchive::Value& lhs, const T& rhs)
{
    return lhs.As<T>() == rhs;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Compare the INI value with another value using template magic
///
/// \tparam T The type to compare to
///
/// \param lhs The left-hand side value
/// \param rhs The right-hand side value
///
/// \return True if the values are not equal, false otherwise
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
typename std::enable_if_t<
    !std::is_same_v<T, FIniArchive::Value> &&
        !std::is_same_v<T, std::string> && std::is_arithmetic_v<T>,
    bool>
    operator!=(const FIniArchive::Value& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

}   // namespace tkd
