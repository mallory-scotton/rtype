///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Object/UObject.hpp>
#include <Engine/Core/Object/IFunction.hpp>
#include <Engine/Core/Object/IProperty.hpp>
#include <Engine/Core/Object/UClass.hpp>
#include <Engine/Core/Object/UProperty.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
std::vector<UObject*> UObject::s_registeredObjects;
std::mutex UObject::s_objectMutex;

///////////////////////////////////////////////////////////////////////////////
UObject::UObject(const FString& name, const UUID& uuid)
    : m_objectID(uuid)
    , m_name(name)
    , m_netRole(ENetRole::None)
    , m_networkID(0)
    , m_owningClientID(0)
    , m_netUpdateFrequency(10.0f)
    , m_timeSinceLastUpdate(0.0f)
    , m_hasSetUpdateFrequency(false)
{
    std::lock_guard<std::mutex> lock(s_objectMutex);
    s_registeredObjects.push_back(this);
}

///////////////////////////////////////////////////////////////////////////////
UObject::~UObject()
{
    std::lock_guard<std::mutex> lock(s_objectMutex);

    auto it = std::find(
        s_registeredObjects.begin(), s_registeredObjects.end(), this
    );
    if (it != s_registeredObjects.end()) { s_registeredObjects.erase(it); }
}

///////////////////////////////////////////////////////////////////////////////
UObject::operator UUID::DataType(void) const { return m_objectID.Data(); }

///////////////////////////////////////////////////////////////////////////////
UObject::operator FString(void) const { return m_name; }

///////////////////////////////////////////////////////////////////////////////
UObject::operator std::string(void) const
{
    return std::string(m_name.CStr());
}

///////////////////////////////////////////////////////////////////////////////
const UUID& UObject::GetUUID(void) const { return m_objectID; }

///////////////////////////////////////////////////////////////////////////////
void UObject::SetUUID(const UUID& uuid) { m_objectID = uuid; }

///////////////////////////////////////////////////////////////////////////////
void UObject::RegisterProperty(IProperty* property)
{
    if (property) { m_properties[property->GetName()] = property; }
}

///////////////////////////////////////////////////////////////////////////////
void UObject::RegisterFunction(IFunction* function)
{
    if (function) { m_functions[function->GetName()] = function; }
}

///////////////////////////////////////////////////////////////////////////////
const std::unordered_map<FString, IProperty*>& UObject::GetProperties(void
) const
{
    return m_properties;
}

///////////////////////////////////////////////////////////////////////////////
void UObject::GetLifetimeReplicatedProperties(
    TVector<IProperty*>& outProperties
)
{
    for (const auto& [name, property]: m_properties)
    {
        if (property->HasFlag(EPropertyFlags::Replicated))
        {
            outProperties.PushBack(property);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
const std::unordered_map<FString, IFunction*>& UObject::GetFunctions(void
) const
{
    return m_functions;
}

///////////////////////////////////////////////////////////////////////////////
IProperty* UObject::GetProperty(const FString& name) const
{
    auto it = m_properties.find(name);
    if (it != m_properties.end()) { return it->second; }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
IFunction* UObject::GetFunction(const FString& name) const
{
    auto it = m_functions.find(name);
    if (it != m_functions.end()) { return it->second; }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
std::string UObject::GetObjectID(void) const { return m_objectID.ToString(); }

///////////////////////////////////////////////////////////////////////////////

const FString& UObject::GetName(void) const { return m_name; }

///////////////////////////////////////////////////////////////////////////////
void UObject::SetName(const FString& name) { m_name = name; }

///////////////////////////////////////////////////////////////////////////////
Bool UObject::IsLocallyControlled(void) const
{
    return m_netRole == ENetRole::AutonomousProxy;
}

///////////////////////////////////////////////////////////////////////////////
Bool UObject::IsAuthority(void) const
{
    return m_netRole == ENetRole::Authority;
}

///////////////////////////////////////////////////////////////////////////////
Bool UObject::IsSimulated(void) const
{
    return m_netRole == ENetRole::SimulatedProxy;
}

///////////////////////////////////////////////////////////////////////////////
ENetRole UObject::GetNetRole(void) const { return m_netRole; }

///////////////////////////////////////////////////////////////////////////////
void UObject::SetNetRole(ENetRole role) { m_netRole = role; }

///////////////////////////////////////////////////////////////////////////////
void UObject::SetNetUpdateFrequency(Float32 frequency)
{
    m_netUpdateFrequency = frequency;
    m_hasSetUpdateFrequency = true;
}

///////////////////////////////////////////////////////////////////////////////
Float32 UObject::GetNetUpdateFrequency(void) const
{
    return m_netUpdateFrequency;
}

///////////////////////////////////////////////////////////////////////////////
void UObject::SetOwningClientID(UInt32 id) { m_owningClientID = id; }

///////////////////////////////////////////////////////////////////////////////
UInt32 UObject::GetOwningClientID(void) const { return m_owningClientID; }

///////////////////////////////////////////////////////////////////////////////
UInt32 UObject::GetNetworkID(void) const { return m_networkID; }

///////////////////////////////////////////////////////////////////////////////
void UObject::SetNetworkID(UInt32 id) { m_networkID = id; }

///////////////////////////////////////////////////////////////////////////////
const std::vector<UObject*>& UObject::GetRegisteredObjects(void)
{
    std::lock_guard<std::mutex> lock(s_objectMutex);
    return s_registeredObjects;
}

///////////////////////////////////////////////////////////////////////////////
UObject* UObject::FindByUUID(const UUID& uuid)
{
    std::lock_guard<std::mutex> lock(s_objectMutex);

    for (auto obj: s_registeredObjects)
    {
        if (obj && obj->GetUUID() == uuid) { return obj; }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
UObject* UObject::FindByName(const FString& name)
{
    std::lock_guard<std::mutex> lock(s_objectMutex);

    for (auto obj: s_registeredObjects)
    {
        if (obj && obj->GetName() == name) { return obj; }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(UObject)

}   // namespace tkd
