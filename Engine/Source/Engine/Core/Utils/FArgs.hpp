///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <any>
#include <Engine/Core/Utils/Singleton.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Args manager for command line argument parsing
///
///////////////////////////////////////////////////////////////////////////////
class FArgs : public TSingleton<FArgs>
{
private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Structure to hold flag information
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Flags
    {
        std::string vlong;
        std::string vshort;
        std::string description;
        bool mandatory;
        std::function<void(const std::string&)> setter;
        std::function<std::string()> getter;
        bool found;
        bool isVector;
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    // Private members
    ///////////////////////////////////////////////////////////////////////////
    std::vector<Flags> m_flags;
    std::unordered_map<std::string, size_t> m_flagMap;
    int m_exitCode;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    FArgs(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~FArgs() = default;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a flag with its long and short forms
    ///
    /// \param flags The flag name (will generate --flag and -f)
    /// \param description Description of the flag
    /// \param reference Reference to the variable to modify
    /// \param mandatory Whether this flag is mandatory
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void AddFlags(
        const std::string& flags,
        const std::string& description,
        T& reference,
        bool mandatory = true
    )
    {
        Flags flag;
        flag.vlong = "--" + flags;
        flag.vshort = "-" + flags.substr(0, 1);
        flag.description = description;
        flag.mandatory = mandatory;
        flag.found = false;
        flag.isVector = false;

        // Create a setter function that converts string to T
        flag.setter = [&reference](const std::string& value)
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                reference = value;
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                reference = std::stoi(value);
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                reference = std::stof(value);
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                reference = std::stod(value);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                reference = (value == "true" || value == "1");
            }
            else if constexpr (std::is_arithmetic_v<T>)
            {
                reference = static_cast<T>(std::stod(value));
            }
        };

        // Create a getter function that returns the value as string
        flag.getter = [&reference]() -> std::string
        {
            if constexpr (std::is_same_v<T, std::string>) { return reference; }
            else if constexpr (std::is_same_v<T, bool>)
            {
                return reference ? "true" : "false";
            }
            else if constexpr (std::is_arithmetic_v<T>)
            {
                return std::to_string(reference);
            }
            return "";
        };

        size_t index = m_flags.size();
        m_flags.push_back(flag);
        m_flagMap[flag.vlong] = index;
        m_flagMap[flag.vshort] = index;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a vector flag with its long and short forms
    ///
    /// \param flags The flag name (will generate --flag and -f)
    /// \param description Description of the flag
    /// \param reference Reference to the vector to modify
    /// \param mandatory Whether this flag is mandatory
    /// \param delimiter The delimiter to split values (default: comma)
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void AddVectorFlags(
        const std::string& flags,
        const std::string& description,
        std::vector<T>& reference,
        bool mandatory = false,
        const std::string& delimiter = ","
    )
    {
        Flags flag;
        flag.vlong = "--" + flags;
        flag.vshort = "-" + flags.substr(0, 1);
        flag.description = description;
        flag.mandatory = mandatory;
        flag.found = false;
        flag.isVector = true;

        // Create a setter function that converts string to vector<T>
        flag.setter = [&reference, delimiter](const std::string& value)
        {
            reference.clear();
            std::stringstream ss(value);
            std::string item;

            while (std::getline(ss, item, delimiter[0]))
            {
                // Trim whitespace
                item.erase(0, item.find_first_not_of(" \t"));
                item.erase(item.find_last_not_of(" \t") + 1);

                if (!item.empty())
                {
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        reference.push_back(item);
                    }
                    else if constexpr (std::is_same_v<T, bool>)
                    {
                        return reference.push_back(
                            item == "true" || item == "1"
                        );
                    }
                    else if constexpr (std::is_same_v<T, int>)
                    {
                        reference.push_back(std::stoi(item));
                    }
                    else if constexpr (std::is_same_v<T, float>)
                    {
                        reference.push_back(std::stof(item));
                    }
                    else if constexpr (std::is_same_v<T, double>)
                    {
                        reference.push_back(std::stod(item));
                    }
                    else if constexpr (std::is_arithmetic_v<T>)
                    {
                        return reference.push_back(
                            static_cast<T>(std::stod(value))
                        );
                    }
                }
            }
        };

        // Create a getter function that returns the vector as string
        flag.getter = [&reference, delimiter]() -> std::string
        {
            if (reference.empty()) { return "[]"; }

            std::string result = "[";
            for (size_t i = 0; i < reference.size(); ++i)
            {
                if (i > 0) { result += delimiter + " "; }

                if constexpr (std::is_same_v<T, std::string>)
                {
                    result += reference[i];
                }
                else { result += std::to_string(reference[i]); }
            }
            result += "]";
            return result;
        };

        size_t index = m_flags.size();
        m_flags.push_back(flag);
        m_flagMap[flag.vlong] = index;
        m_flagMap[flag.vshort] = index;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Process command line arguments
    ///
    /// \param argc Argument count
    /// \param argv Argument vector
    ///
    /// \return true if processing was successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool Process(int argc, char* argv[]);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The exit code of the program
    ///
    /// \return The exit code
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetExitCode(void) const;

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Print usage information
    ///
    /// \param prog The program name
    ///
    ///////////////////////////////////////////////////////////////////////////
    void PrintUsage(char* prog) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if all mandatory flags were provided
    ///
    /// \return true if all mandatory flags are present
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool ValidateMandatoryFlags(void) const;
};

}   // namespace tkd
