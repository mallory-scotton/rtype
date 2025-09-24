///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Archives/FIniArchive.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Value::Value(const std::string& value)
    : m_value(value)
{}

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Value::operator std::string(void) const { return m_value; }

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Value::operator int(void) const { return ConvertTo<int>(); }

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Value::operator bool(void) const { return ConvertTo<bool>(); }

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Value::operator float(void) const { return ConvertTo<float>(); }

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Value& FIniArchive::Value::operator=(const std::string& value)
{
    m_value = value;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
bool FIniArchive::Value::Empty(void) const noexcept { return m_value.empty(); }

///////////////////////////////////////////////////////////////////////////////
const std::string& FIniArchive::Value::Str(void) const noexcept
{
    return m_value;
}

///////////////////////////////////////////////////////////////////////////////
FIniArchive::Section::Section(bool caseSensitive)
    : m_caseSensitive(caseSensitive)
{}

///////////////////////////////////////////////////////////////////////////////
std::string FIniArchive::Section::NormalizeKey(const std::string& key) const
{
    if (m_caseSensitive) { return key; }
    std::string normalized = key;
    std::transform(
        normalized.begin(), normalized.end(), normalized.begin(), ::tolower
    );
    return normalized;
}

///////////////////////////////////////////////////////////////////////////////
void FIniArchive::Section::Set(const std::string& key, const Value& value)
{
    std::string normalizedKey = NormalizeKey(key);
    if (m_keys.find(normalizedKey) == m_keys.end())
    {
        m_keyOrder.push_back(key);
    }
    m_keys[normalizedKey] = value;
}

///////////////////////////////////////////////////////////////////////////////
std::optional<FIniArchive::Value>
    FIniArchive::Section::Get(const std::string& key) const
{
    std::string normalizedKey = NormalizeKey(key);
    auto it = m_keys.find(normalizedKey);
    return (it != m_keys.end()) ? std::make_optional(it->second)
                                : std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FIniArchive::Value& lhs, const FIniArchive::Value& rhs)
{
    return static_cast<std::string>(lhs) == static_cast<std::string>(rhs);
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FIniArchive::Value& lhs, const FIniArchive::Value& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const std::string& lhs, const FIniArchive::Value& rhs)
{
    return lhs == static_cast<std::string>(rhs);
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const std::string& lhs, const FIniArchive::Value& rhs)
{
    return !(lhs == rhs);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const FIniArchive::Value& lhs, const std::string& rhs)
{
    return static_cast<std::string>(lhs) == rhs;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const FIniArchive::Value& lhs, const std::string& rhs)
{
    return !(lhs == rhs);
}

}   // namespace tkd
