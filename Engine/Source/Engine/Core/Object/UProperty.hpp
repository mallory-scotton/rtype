///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Core/Math.hpp>
#include <Engine/Core/Object/IProperty.hpp>
#include <Engine/Core/Object/UObject.hpp>
#include <Engine/Network/FBinaryWriter.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Template class for UProperty with type and flags.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class UProperty : public IProperty
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    using ValueType = T;             //<! Alias for the property type.
    using ThisType = UProperty<T>;   //<! Alias for this class type.

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    FString m_name;           //<! The name of the property.
    ValueType m_value;        //<! The value of the property.
    UObject& m_owner;         //<! Pointer to the owning UObject.
    EPropertyFlags m_flags;   //<! The flags associated with the property.
    bool m_isDirty = false;   //<! Indicates if the property has been modified.

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for UProperty.
    ///
    /// \param owner The owning UObject.
    /// \param name The name of the property.
    /// \param value The initial value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UProperty(
        UObject& owner,
        const FString& name = "<Unnamed>",
        const T& value = T(),
        EPropertyFlags flags = EPropertyFlags::None
    )
        : m_name(name)
        , m_value(value)
        , m_owner(owner)
        , m_flags(flags)
    {
        owner.RegisterProperty(this);

        auto ownerClass = owner.GetClass();
        if (ownerClass && !ownerClass->IsRegistered())
        {
            ownerClass->AddProperty(this);
        }
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Conversion operator to the property value type (const).
    ///
    /// \return The value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    operator const T&(void) const { return m_value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Conversion operator to the property value type (const).
    ///
    /// \return The value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& operator()(void) const { return m_value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator to set the property value.
    ///
    /// \param value The new value to assign to the property.
    ///
    /// \return Reference to this UProperty instance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ThisType& operator=(const T& value)
    {
        if (m_value != value)
        {
            m_value = value;
            MarkDirty();
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Move operator to set the property value.
    ///
    /// \param value The new value to assign to the property.
    ///
    /// \return Reference to this UProperty instance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    ThisType& operator=(T&& value)
    {
        if (m_value != value)
        {
            m_value = std::move(value);
            MarkDirty();
        }
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pointer access operator to the property value (mutable version).
    ///
    /// \return Pointer to the value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T* operator->(void)
    {
        MarkDirty();
        return &m_value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Pointer access operator to the property value (const version).
    ///
    /// \return Const pointer to the value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T* operator->(void) const { return &m_value; }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the name of the property.
    ///
    /// \return The name of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual const FString& GetName(void) const override { return m_name; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the name of the property.
    ///
    /// \param name The new name for the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetName(const FString& name) override { m_name = name; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the value of the property.
    ///
    /// \return The value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& GetValue(void) const { return m_value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the value of the property.
    ///
    /// \return The value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    T& GetValue(void)
    {
        MarkDirty();
        return m_value;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the value of the property without marking dirty.
    ///
    /// \return The value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    const T& Get(void) const { return m_value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the owner UObject of the property.
    ///
    /// \return The owner UObject of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual const UObject& GetOwner(void) const override { return m_owner; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the owner UObject of the property.
    ///
    /// \return The owner UObject of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual UObject& GetOwner(void) override { return m_owner; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the property path in the format "OwnerID/PropertyName".
    ///
    /// \return The property path as a FString.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FString GetPropertyPath(void) const override
    {
        return m_owner.GetObjectID() + "/" + m_name;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Converts the property to a string representation.
    ///
    /// \return The string representation of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual FString ToString(void) const override
    {
        return FString::ToString(m_value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Sets the value of the property.
    ///
    /// \param value The new value for the property.
    /// \param size The size of the value in bytes.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetValue(const void* value, SizeT size) override
    {
        if (size != sizeof(T))
        {
            throw std::invalid_argument(
                "Size mismatch in SetValue: expected " +
                std::to_string(sizeof(T)) + ", got " + std::to_string(size)
            );
        }
        T newValue = *static_cast<const T*>(value);
        if (m_value != newValue)
        {
            m_value = newValue;
            MarkDirty();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the value of the property.
    ///
    /// \param value The new value for the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetValue(const T& value)
    {
        if (m_value != value)
        {
            MarkDirty();
            m_value = value;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the value of the property.
    ///
    /// \param value The new value for the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetValue(T&& value)
    {
        if (m_value != value)
        {
            MarkDirty();
            m_value = std::move(value);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the flags associated with the property.
    ///
    /// \return The property flags.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual EPropertyFlags GetFlags(void) const override { return m_flags; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the flags for the property
    ///
    /// \param flags The flags to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void SetFlags(EPropertyFlags flags) override { m_flags = flags; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a flag for the property
    ///
    /// \param flag The flags to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void AddFlag(EPropertyFlags flag) override
    {
        if (!HasFlag(flag))
        {
            m_flags = static_cast<EPropertyFlags>(
                static_cast<UInt32>(m_flags) | static_cast<UInt32>(flag)
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove a flag for the property
    ///
    /// \param flag The flags to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void RemoveFlag(EPropertyFlags flag) override
    {
        if (HasFlag(flag))
        {
            m_flags = static_cast<EPropertyFlags>(
                static_cast<UInt32>(m_flags) & ~static_cast<UInt32>(flag)
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the property has a specific flag
    ///
    /// \param flag The flag to check
    ///
    /// \return True if the property has the flag, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool HasFlag(EPropertyFlags flag) const override
    {
        return (static_cast<UInt32>(m_flags) & static_cast<UInt32>(flag)) != 0;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the property is dirty (modified)
    ///
    /// \return True if the property is dirty, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual bool IsDirty(void) const override { return m_isDirty; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Mark the property as dirty (modified)
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void MarkDirty(void) override { m_isDirty = true; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clear the dirty flag for the property
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void ClearDirty(void) override { m_isDirty = false; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Serialize the property to a byte vector
    ///
    /// \return The serialized property data
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual std::vector<Byte> Serialize(void) const override
    {
        std::vector<Byte> data;
        FBinaryWriter writer(data);
        writer.Write(m_value);
        return data;
    }
};

}   // namespace tkd
