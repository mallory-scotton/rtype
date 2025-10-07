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

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Template class for UProperty with type and flags.
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, EPropertyFlags Flags = EPropertyFlags::None>
class UProperty : public IProperty
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    using ValueType = T;                    //<! Alias for the property type.
    using ThisType = UProperty<T, Flags>;   //<! Alias for this class type.

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    FString m_name;      //<! The name of the property.
    ValueType m_value;   //<! The value of the property.
    UObject& m_owner;    //<! Pointer to the owning UObject.

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
        UObject& owner, const FString& name = "<Unnamed>", const T& value = T()
    )
        : m_name(name)
        , m_value(value)
        , m_owner(owner)
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
    /// \brief Conversion operator to the property value type.
    ///
    /// \return The value of the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    operator T(void) const { return m_value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Assignment operator to set the property value.
    ///
    /// \param value The new value to assign to the property.
    ///
    /// \return Reference to this UProperty instance.
    ///
    ///////////////////////////////////////////////////////////////////////////
    UProperty<T, Flags>& operator=(const T& value)
    {
        m_value = value;
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
    UProperty<T, Flags>& operator=(T&& value)
    {
        m_value = std::move(value);
        return *this;
    }

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
    T& GetValue(void) { return m_value; }

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
        m_value = *static_cast<const T*>(value);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the value of the property.
    ///
    /// \param value The new value for the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetValue(const T& value) { m_value = value; }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the value of the property.
    ///
    /// \param value The new value for the property.
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetValue(T&& value) { m_value = std::move(value); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the flags associated with the property.
    ///
    /// \return The property flags.
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual EPropertyFlags GetFlags(void) const override { return Flags; }
};

}   // namespace tkd
