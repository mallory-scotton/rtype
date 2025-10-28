///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Core/Object/UClass.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Helper class for automatic class registration
///
/// \tparam T The type of the class to register
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TClassRegistrar
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor for the class registrar.
    ///
    /// \param className The name of the class to register
    /// \param superClass Pointer to the superclass (default is nullptr)
    ///
    ///////////////////////////////////////////////////////////////////////////
    TClassRegistrar(const FString& className, UClass* superClass = nullptr)
    {
        UClass* registeredClass = UClass::RegisterClass(className, superClass);

        // Create a new instance of the class to register its properties
        std::unique_ptr<T> instance = std::make_unique<T>();

        // Reset the instance to avoid memory leaks
        instance.reset();

        // Set the class has registered
        registeredClass->SetRegistered(true);

        // Set the function to create an instance of the class
        registeredClass->SetCreateFunction<T>();
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Macro to declare a class as reflectable
///
/// Place this in the class header inside the class body
///
///////////////////////////////////////////////////////////////////////////////
#define DECLARE_CLASS(ClassName) \
public: static tkd::UClass* StaticClass(void); \
public: virtual tkd::UClass* GetClass(void) const { return StaticClass(); } \
private: static tkd::TClassRegistrar<ClassName> s_classRegistrar;

///////////////////////////////////////////////////////////////////////////////
/// \brief Macro to declare a class as reflectable
///
/// Place this in the class header inside the class body
///
///////////////////////////////////////////////////////////////////////////////
#define DECLARE_CLASS_WITH_SUPER(ClassName, SuperClass) \
public: static tkd::UClass* StaticClass(void); \
public: virtual tkd::UClass* GetClass(void) const { return StaticClass(); } \
private: static tkd::TClassRegistrar<ClassName> s_classRegistrar; \
public: using Super = SuperClass;

///////////////////////////////////////////////////////////////////////////////
/// \brief Macro to implement class registration
///
/// Place this in the .cpp file for the class
///
/// \param ClassName The name of the class
///
///////////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_CLASS(ClassName) \
    tkd::UClass* ClassName::StaticClass(void) \
    { \
        return tkd::UClass::FindClass(#ClassName); \
    } \
    tkd::TClassRegistrar<ClassName> ClassName::s_classRegistrar( \
        #ClassName, \
        nullptr \
    );

///////////////////////////////////////////////////////////////////////////////
/// \brief Macro to implement class registration with a superclass
///
/// Place this in the .cpp file for the class
///
/// \param ClassName The name of the class
/// \param SuperClassName The name of the superclass
///
///////////////////////////////////////////////////////////////////////////////
#define IMPLEMENT_CLASS_WITH_SUPER(ClassName, SuperClassName) \
    tkd::UClass* ClassName::StaticClass(void) \
    { \
        return tkd::UClass::FindClass(#ClassName); \
    } \
    tkd::TClassRegistrar<ClassName> ClassName::s_classRegistrar( \
        #ClassName, \
        SuperClassName::StaticClass() \
    );

///////////////////////////////////////////////////////////////////////////////
/// \brief Macro to manually register a class at runtime
///
/// Useful for classes that need delayed registration
///
///////////////////////////////////////////////////////////////////////////////
#define REGISTER_CLASS(ClassName) \
    tkd::UClass::RegisterClass(#ClassName)->SetCreateFunction<ClassName>()

///////////////////////////////////////////////////////////////////////////////
/// \brief Macro to manually register a class with a superclass at runtime
///
///////////////////////////////////////////////////////////////////////////////
#define REGISTER_CLASS_WITH_SUPER(ClassName, SuperClassName) \
    tkd::UClass::RegisterClass(#ClassName, SuperClassName::StaticClass()) \
        ->SetCreateFunction<ClassName>()

}   // namespace tkd
