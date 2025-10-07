///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Object/UClass.hpp>
#include <Engine/Core/Object/UObject.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::unordered_map<FString, std::unique_ptr<UClass>> UClass::s_classRegistry;

///////////////////////////////////////////////////////////////////////////////
UClass::UClass(const FString& name, UClass* super)
    : m_name(name)
    , m_super(super)
    , m_isRegistered(false)
{}

///////////////////////////////////////////////////////////////////////////////
bool UClass::IsChildOf(UClass* other) const
{
    const UClass* current = this;
    while (current)
    {
        if (current == other) { return true; }
        current = current->m_super;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
const FString& UClass::GetName(void) const { return m_name; }

///////////////////////////////////////////////////////////////////////////////
bool UClass::IsRegistered(void) const { return m_isRegistered; }

///////////////////////////////////////////////////////////////////////////////
void UClass::SetRegistered(bool registered) { m_isRegistered = registered; }

///////////////////////////////////////////////////////////////////////////////
const UClass* UClass::GetSuper(void) const { return m_super; }

///////////////////////////////////////////////////////////////////////////////
const UClass::DefinitionList& UClass::GetProperties(void) const
{
    return m_properties;
}

///////////////////////////////////////////////////////////////////////////////
const UClass::DefinitionList& UClass::GetFunctions(void) const
{
    return m_functions;
}

///////////////////////////////////////////////////////////////////////////////
UObject* UClass::CreateInstance(void) const
{
    if (m_createInstance) { return m_createInstance(); }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
void UClass::AddProperty(IProperty* property)
{
    if (property)
    {
        m_properties.push_back(
            std::make_pair(property->GetName(), EDefinitionSource::Class)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void UClass::AddFunction(const FString& functionName)
{
    if (!functionName.IsEmpty())
    {
        m_functions.push_back(
            std::make_pair(functionName, EDefinitionSource::Class)
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
UClass* UClass::RegisterClass(const FString& name, UClass* super)
{
    auto it = s_classRegistry.find(name);
    if (it != s_classRegistry.end()) { return it->second.get(); }

    auto newClass = std::make_unique<UClass>(name, super);
    UClass* classPtr = newClass.get();
    s_classRegistry[name] = std::move(newClass);

    const UClass* superClass = classPtr->GetSuper();
    while (superClass && superClass->IsRegistered())
    {
        for (const auto& [propName, source]: superClass->GetProperties())
        {
            if (source == EDefinitionSource::Super) { continue; }
            classPtr->m_properties.push_back(
                std::make_pair(propName, EDefinitionSource::Super)
            );
        }
        for (const auto& [funcName, source]: superClass->GetFunctions())
        {
            if (source == EDefinitionSource::Super) { continue; }
            classPtr->m_functions.push_back(
                std::make_pair(funcName, EDefinitionSource::Super)
            );
        }
        superClass = superClass->GetSuper();
    }

    return classPtr;
}

///////////////////////////////////////////////////////////////////////////////
UClass* UClass::FindClass(const FString& name)
{
    auto it = s_classRegistry.find(name);
    if (it != s_classRegistry.end()) { return it->second.get(); }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<UClass*> UClass::GetAllClasses(void)
{
    std::vector<UClass*> classes;
    classes.reserve(s_classRegistry.size());

    for (const auto& pair: s_classRegistry)
    {
        classes.push_back(pair.second.get());
    }

    return classes;
}

///////////////////////////////////////////////////////////////////////////////
std::vector<UClass*> UClass::GetDerivedClasses(UClass* baseClass)
{
    std::vector<UClass*> derivedClasses;

    if (baseClass == nullptr) { return derivedClasses; }

    for (const auto& pair: s_classRegistry)
    {
        UClass* cls = pair.second.get();
        if (cls != baseClass && cls->IsChildOf(baseClass))
        {
            derivedClasses.push_back(cls);
        }
    }

    return derivedClasses;
}

///////////////////////////////////////////////////////////////////////////////
void UClass::ClearRegistry(void) { s_classRegistry.clear(); }

}   // namespace tkd
