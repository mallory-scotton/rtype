///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers/FString.hpp>
#include <Engine/Core/Object/UObject.hpp>
#include <typeindex>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Enumeration for property flags.
///
///////////////////////////////////////////////////////////////////////////////
enum class EPropertyFlags : UInt32
{
    None = 0,                //<! No flags
    EditAnywhere = 1 << 0,   //<! Property can be edited anywhere
    Replicated = 1 << 1,     //<! Property is replicated over the network
    ReadOnly = 1 << 2        //<! Property is read-only
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Class representing a property with a name, type, and offset.
///
///////////////////////////////////////////////////////////////////////////////
class UProperty
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    FString m_name;           //<! Name of the property
    std::type_index m_type;   //<! Type of the property
    SizeT m_offset;   //<! Offset of the property in the containing class
    EPropertyFlags m_flags;   //<! Flags associated with the property

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name Name of the property
    /// \param type Type of the property
    /// \param offset Offset of the property in the containing class
    /// \param flags Flags associated with the property
    ///
    ///////////////////////////////////////////////////////////////////////////
    UProperty(
        const FString& name,
        const std::type_index& type,
        SizeT offset,
        EPropertyFlags flags = EPropertyFlags::None
    );

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the name of the property.
    ///
    /// \return The name of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FString& GetName(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the type of the property.
    ///
    /// \return The type of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const std::type_index& GetType(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the offset of the property in the containing class.
    ///
    /// \return The offset of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    SizeT GetOffset(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets a pointer to the value of the property for a given UObject
    /// instance.
    ///
    /// \tparam T The type to which the property value should be cast.
    ///
    /// \param obj Pointer to the UObject instance.
    ///
    /// \return A pointer to the value of the property cast to the specified
    /// type T.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    T* GetValuePtr(UObject* obj) const
    {
        if (obj == nullptr) { return nullptr; }
        auto* baseAddress = reinterpret_cast<UInt8*>(obj);
        return reinterpret_cast<T*>(baseAddress + m_offset);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets a const pointer to the value of the property for a given
    ///
    /// \tparam T The type to which the property value should be cast.
    ///
    /// \param obj Pointer to the UObject instance.
    ///
    /// \return A const pointer to the value of the property cast to the
    /// specified type T.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    const T* GetValuePtr(const UObject* obj) const
    {
        if (obj == nullptr) { return nullptr; }
        auto* baseAddress = reinterpret_cast<const UInt8*>(obj);
        return reinterpret_cast<const T*>(baseAddress + m_offset);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets a reference to the value of the property for a given
    ///
    /// \tparam T The type to which the property value should be cast.
    ///
    /// \param obj Pointer to the UObject instance.
    ///
    /// \return A reference to the value of the property cast to the specified
    /// type T.
    ///
    /// \throws std::runtime_error if the obj pointer is null.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    T& GetValue(UObject* obj) const
    {
        if (obj == nullptr)
        {
            throw std::runtime_error("Null object pointer");
        }
        auto* baseAddress = reinterpret_cast<UInt8*>(obj);
        return *reinterpret_cast<T*>(baseAddress + m_offset);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets a const reference to the value of the property for a given
    ///
    /// \tparam T The type to which the property value should be cast.
    ///
    /// \param obj Pointer to the UObject instance.
    ///
    /// \return A const reference to the value of the property cast to the
    /// specified type T.
    ///
    /// \throws std::runtime_error if the obj pointer is null.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    const T& GetValue(const UObject* obj) const
    {
        if (obj == nullptr)
        {
            throw std::runtime_error("Null object pointer");
        }
        auto* baseAddress = reinterpret_cast<const UInt8*>(obj);
        return *reinterpret_cast<const T*>(baseAddress + m_offset);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the value of the property for a given UObject instance.
    ///
    /// \tparam T The type of the value to set.
    ///
    /// \param obj Pointer to the UObject instance.
    /// \param value The new value to set.
    ///
    /// \throws std::runtime_error if the obj pointer is null.
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void SetValue(UObject* obj, const T& value) const
    {
        if (obj == nullptr)
        {
            throw std::runtime_error("Null object pointer");
        }
        auto* baseAddress = reinterpret_cast<UInt8*>(obj);
        *reinterpret_cast<T*>(baseAddress + m_offset) = value;
    }
};

}   // namespace tkd
