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
#include <Engine/Core/Object.hpp>
#include <Engine/Runtime/Components/UActorComponent.hpp>
#include <Engine/Runtime/Time/ITickable.hpp>
#include <memory>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief A base class for all actors in the world
///
///////////////////////////////////////////////////////////////////////////////
class AActor
    : public UObject
    , public ITickable
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Aliases
    ///////////////////////////////////////////////////////////////////////////
    using Component = std::shared_ptr<UActorComponent>;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    UProperty<FTransform> m_transform;   //<! The actor's transform
    UProperty<Bool> m_isActive;          //<! Whether the actor is active
    TVector<Component> m_components;     //<! The actor's components

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member Access
    ///////////////////////////////////////////////////////////////////////////
    AActor& self;   //<! Reference to self for event bindings

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class Member
    ///////////////////////////////////////////////////////////////////////////
    UFunction<void(AActor*)> OnActorBeginOverlap;
    UFunction<void(AActor*)> OnActorEndOverlap;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    AActor(const FString& name = "AActor");

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is first created
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void BeginPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Ticks the object
    ///
    /// \param deltaTime The time elapsed since the last tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void Tick(Float32 deltaTime) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Called when the object is being destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void EndPlay(void) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the actor's transform
    ///
    /// \return The actor's transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    const FTransform& GetTransform(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the actor's transform
    ///
    /// \param transform The new transform
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetTransform(const FTransform& transform);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if the actor is active
    ///
    /// \return True if the actor is active, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    Bool IsActive(void) const;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set whether the actor is active
    ///
    /// \param isActive True to set the actor as active, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    void SetActive(Bool isActive);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a component to the actor
    ///
    /// \tparam T The type of the component to add
    /// \tparam Args The types of the arguments to forward to the component's
    /// constructor
    ///
    /// \param args The arguments to forward to the component's constructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        auto newComponent = m_components.EmplaceBack(
            std::make_shared<T>(std::forward<Args>(args)...)
        );

        newComponent->SetOwner(this);

        return reinterpret_cast<T*>(newComponent.get());
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a component to the actor
    ///
    /// \param component The component to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    UActorComponent* AddComponent(Component component)
    {
        auto newComponent = m_components.EmplaceBack(std::move(component));
        newComponent->SetOwner(this);
        return newComponent.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Add a component to the actor
    ///
    /// \param component Pointer to the component to add
    ///
    ///////////////////////////////////////////////////////////////////////////
    UActorComponent* AddComponent(UActorComponent* component)
    {
        auto newComponent = m_components.EmplaceBack(component);
        newComponent->SetOwner(this);
        return newComponent.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the actor's components
    ///
    /// \return A constant reference to the vector of component pointers
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = UActorComponent>
    TVector<std::shared_ptr<T>> GetComponents(void) const
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        if constexpr (std::is_same<T, UActorComponent>::value)
        {
            return m_components;
        }

        TVector<std::shared_ptr<T>> filteredComponents;
        for (const auto& component: m_components)
        {
            if (auto casted = dynamic_cast<T*>(component.get()))
            {
                filteredComponents.EmplaceBack(std::make_shared<T>(*casted));
            }
        }
        return filteredComponents;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get a component of the actor by name
    ///
    /// \param name The name of the component to get
    ///
    /// \return A pointer to the component if found, nullptr otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T = UActorComponent>
    T* GetComponent(const FString& name) const
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        for (const auto& component: m_components)
        {
            if (component->GetName() == name)
            {
                return dynamic_cast<T*>(component.get());
            }
        }
        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all components from a type from the actor
    ///
    /// \tparam T The type of the components to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void RemoveComponents(void)
    {
        static_assert(
            std::is_base_of<UActorComponent, T>::value,
            "T must be derived from UActorComponent"
        );

        m_components.Erase(
            std::remove_if(
                m_components.Begin(),
                m_components.End(),
                [](const Component& component)
                { return dynamic_cast<T*>(component.get()) != nullptr; }
            ),
            m_components.End()
        );
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove a component by name from the actor
    ///
    /// \param name The name of the component to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RemoveComponent(const FString& name);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove a specific component from the actor
    ///
    /// \param component Pointer to the component to remove
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RemoveComponent(UActorComponent* component);

public:
    ///////////////////////////////////////////////////////////////////////////
    // Class metadata
    ///////////////////////////////////////////////////////////////////////////
    DECLARE_CLASS(AActor)
};

}   // namespace tkd
