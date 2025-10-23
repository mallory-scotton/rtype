# UProperty System

## Overview

The **`UProperty`** system in the TKD Engine provides a powerful mechanism for runtime property reflection, serialization, and network replication. As a template class that implements the `IProperty` interface, `UProperty` enables type-safe storage of object properties with automatic change tracking, metadata flags, and seamless integration with the engine's reflection and networking systems.

`UProperty` serves as the bridge between compile-time type safety and runtime flexibility, allowing properties to be discovered, modified, and synchronized across network boundaries while maintaining performance and memory efficiency.

## Purpose

The UProperty system is designed to:
- Provide type-safe property storage with template metaprogramming
- Enable runtime property reflection and introspection
- Support automatic change detection and dirty flag tracking
- Integrate with network replication for multiplayer synchronization
- Provide serialization capabilities for save/load systems
- Support property metadata through flag systems
- Enable editor integration and debugging tools
- Maintain performance while providing reflection capabilities

## Core Architecture

### Template Design
```cpp
namespace tkd
{
    template <typename T>
    class UProperty : public IProperty
    {
        // Type-safe property implementation
    };
}
```

### Key Design Principles

1. **Type Safety**: Template-based compile-time type checking
2. **Change Tracking**: Automatic dirty flag management
3. **Network Awareness**: Built-in replication support
4. **Serialization**: Binary serialization for network and storage
5. **Reflection**: Runtime property discovery and manipulation
6. **Performance**: Minimal overhead compared to raw member variables
7. **Flexibility**: Support for any serializable type

## UProperty Member Variables

### Instance Members

#### m_name
```cpp
FString m_name;
```
- **Type**: `FString`
- **Purpose**: Human-readable property identifier
- **Default**: `"<Unnamed>"`
- **Usage**: Property lookup, debugging, editor display

#### m_value
```cpp
ValueType m_value;
```
- **Type**: `T` (template parameter)
- **Purpose**: The actual property value storage
- **Default**: Default-constructed `T()`
- **Usage**: Type-safe value storage and access

#### m_owner
```cpp
UObject& m_owner;
```
- **Type**: `UObject&` (reference)
- **Purpose**: Reference to the object that owns this property
- **Usage**: Property registration, path generation, ownership validation

#### m_flags
```cpp
EPropertyFlags m_flags;
```
- **Type**: `EPropertyFlags`
- **Purpose**: Metadata flags controlling property behavior
- **Default**: `EPropertyFlags::None`
- **Usage**: Replication control, editor visibility, serialization options

#### m_isDirty
```cpp
bool m_isDirty;
```
- **Type**: `bool`
- **Purpose**: Change tracking flag
- **Default**: `false`
- **Usage**: Network replication, serialization optimization

## UProperty Type Aliases

### ValueType
```cpp
using ValueType = T;
```
- **Purpose**: Convenient alias for the template parameter type
- **Usage**: Type traits, template metaprogramming

### ThisType
```cpp
using ThisType = UProperty<T>;
```
- **Purpose**: Self-referential type alias
- **Usage**: Method chaining, return types

## UProperty Constructor

### Primary Constructor
```cpp
UProperty(
    UObject& owner,
    const FString& name = "<Unnamed>",
    const T& value = T(),
    EPropertyFlags flags = EPropertyFlags::None
);
```
- **Parameters**:
  - `owner`: Reference to the owning UObject (required)
  - `name`: Property name (default: `"<Unnamed>"`)
  - `value`: Initial value (default: default-constructed T)
  - `flags`: Property flags (default: None)
- **Behavior**:
  1. Initializes all member variables
  2. Registers property with owner object via `owner.RegisterProperty(this)`
  3. Adds property to owner's class metadata if class not registered

## Conversion Operators

### Value Conversion (const)
```cpp
operator const T&(void) const;
```
- **Returns**: Const reference to the property value
- **Usage**: Implicit conversion to value type for reading

### Function Call Operator (const)
```cpp
const T& operator()(void) const;
```
- **Returns**: Const reference to the property value
- **Usage**: Explicit value access syntax `property()`

## Assignment Operators

### Copy Assignment
```cpp
ThisType& operator=(const T& value);
```
- **Behavior**:
  1. Compares new value with current value
  2. If different, updates value and marks property dirty
  3. Returns reference for chaining
- **Usage**: `property = newValue;`

### Move Assignment
```cpp
ThisType& operator=(T&& value);
```
- **Behavior**: Same as copy assignment but uses move semantics
- **Usage**: `property = std::move(newValue);`

## Access Operators

### Arrow Operator (Mutable)
```cpp
T* operator->(void);
```
- **Returns**: Pointer to the value
- **Behavior**: Marks property dirty before returning
- **Usage**: Direct member access that triggers change tracking

### Arrow Operator (Const)
```cpp
const T* operator->(void) const;
```
- **Returns**: Const pointer to the value
- **Usage**: Read-only member access

## Core Access Methods

### GetName / SetName
```cpp
virtual const FString& GetName(void) const override;
virtual void SetName(const FString& name) override;
```
- **Purpose**: Property identification
- **Usage**: Reflection, debugging, serialization

### GetValue (Const)
```cpp
const T& GetValue(void) const;
```
- **Returns**: Const reference to value
- **Usage**: Read-only value access

### GetValue (Mutable)
```cpp
T& GetValue(void);
```
- **Returns**: Reference to value
- **Behavior**: Marks property dirty
- **Usage**: Direct value modification

### Get (Const)
```cpp
const T& Get(void) const;
```
- **Returns**: Const reference to value
- **Usage**: Read access without triggering dirty flag

## Owner and Path Methods

### GetOwner
```cpp
virtual const UObject& GetOwner(void) const override;
virtual UObject& GetOwner(void) override;
```
- **Returns**: Reference to owning object
- **Usage**: Property ownership validation, path generation

### GetPropertyPath
```cpp
virtual FString GetPropertyPath(void) const override;
```
- **Returns**: Path in format `"OwnerID/PropertyName"`
- **Implementation**: `m_owner.GetObjectID() + "/" + m_name`
- **Usage**: Unique property identification, debugging

## String Conversion

### ToString
```cpp
virtual FString ToString(void) const override;
```
- **Returns**: String representation of the value
- **Implementation**: Uses `FString::ToString(m_value)`
- **Usage**: Debugging, logging, editor display

## Value Setting Methods

### SetValue (void* version)
```cpp
virtual void SetValue(const void* value, SizeT size) override;
```
- **Parameters**:
  - `value`: Pointer to new value data
  - `size`: Size of the data in bytes
- **Behavior**:
  1. Validates size matches `sizeof(T)`
  2. Copies value and marks dirty if changed
  3. Throws `std::invalid_argument` on size mismatch
- **Usage**: Runtime type-erased value setting

### SetValue (Typed versions)
```cpp
void SetValue(const T& value);
void SetValue(T&& value);
```
- **Purpose**: Type-safe value setting with change detection
- **Usage**: Direct value assignment with dirty flag management

## Flag Management System

### GetFlags / SetFlags
```cpp
virtual EPropertyFlags GetFlags(void) const override;
virtual void SetFlags(EPropertyFlags flags) override;
```
- **Purpose**: Access and modify property flags
- **Usage**: Runtime flag manipulation

### AddFlag
```cpp
virtual void AddFlag(EPropertyFlags flag) override;
```
- **Behavior**: Bitwise OR operation to add flag
- **Safety**: Checks if flag already present to avoid redundant operations

### RemoveFlag
```cpp
virtual void RemoveFlag(EPropertyFlags flag) override;
```
- **Behavior**: Bitwise AND with inverted flag to remove
- **Safety**: Checks if flag present before removal

### HasFlag
```cpp
virtual bool HasFlag(EPropertyFlags flag) const override;
```
- **Returns**: `true` if flag is set
- **Implementation**: Bitwise AND check
- **Usage**: Conditional behavior based on flags

## Change Tracking System

### IsDirty
```cpp
virtual bool IsDirty(void) const override;
```
- **Returns**: Current dirty state
- **Usage**: Check if property has been modified

### MarkDirty
```cpp
virtual void MarkDirty(void) override;
```
- **Behavior**: Sets `m_isDirty = true`
- **Usage**: Manual dirty flag setting

### ClearDirty
```cpp
virtual void ClearDirty(void) override;
```
- **Behavior**: Sets `m_isDirty = false`
- **Usage**: Reset dirty flag after processing changes

## Serialization System

### Serialize
```cpp
virtual std::vector<Byte> Serialize(void) const override;
```
- **Returns**: Binary serialized data
- **Implementation**:
  1. Creates byte vector
  2. Uses `FBinaryWriter` to serialize value
  3. Returns serialized data
- **Usage**: Network transmission, save/load systems

## EPropertyFlags Enumeration

The property flags system controls property behavior:

```cpp
enum class EPropertyFlags : UInt32
{
    None        = 0,          // No special behavior
    Replicated  = 1 << 0,     // Property is replicated over network
    Transient   = 1 << 1,     // Property not saved to disk
    EditorOnly  = 1 << 2,     // Property only visible in editor
    ReadOnly    = 1 << 3,     // Property cannot be modified at runtime
    // ... additional flags as needed
};
```

### Flag Usage Patterns

#### Replicated
- **Purpose**: Network synchronization
- **Behavior**: Property changes sent to clients
- **Authority**: Only server can modify replicated properties

#### Transient
- **Purpose**: Runtime-only data
- **Behavior**: Excluded from save/load operations
- **Use Case**: Temporary state, cached calculations

#### EditorOnly
- **Purpose**: Development tools
- **Behavior**: Hidden in release builds
- **Use Case**: Debug information, editor state

#### ReadOnly
- **Purpose**: Immutable properties
- **Behavior**: Assignment operators disabled
- **Use Case**: Constants, computed values

## Usage Examples

### Basic Property Declaration
```cpp
class APlayer : public UObject
{
public:
    UPROPERTY()
    int health = 100;

    UPROPERTY()
    FString playerName = "Player";

    UPROPERTY(Replicated)
    FVector3 position;

    APlayer()
        : UObject("Player")
        , health(*this, "health", 100)
        , playerName(*this, "playerName", "Player")
        , position(*this, "position", FVector3::Zero, EPropertyFlags::Replicated)
    {}
};
```

### Property Access Patterns
```cpp
APlayer* player = new APlayer();

// Direct assignment (triggers dirty flag)
player->health = 75;

// Access operators
int currentHealth = player->health;        // Conversion operator
int healthValue = player->health();        // Function call operator
int* healthPtr = player->health.operator->(); // Direct pointer access

// Explicit methods
player->health.SetValue(50);               // Type-safe setting
const int& healthRef = player->health.GetValue(); // Reference access
std::string healthStr = player->health.ToString(); // String conversion
```

### Flag Management
```cpp
// Check flags
if (player->position.HasFlag(EPropertyFlags::Replicated)) {
    FLogger::Info("Position is replicated");
}

// Modify flags
player->health.AddFlag(EPropertyFlags::ReadOnly);
player->health.RemoveFlag(EPropertyFlags::Transient);

// Set multiple flags
player->position.SetFlags(EPropertyFlags::Replicated | EPropertyFlags::Transient);
```

### Change Tracking
```cpp
// Check if modified
if (player->health.IsDirty()) {
    FLogger::Info("Health was modified");
    // Process health change
    player->health.ClearDirty(); // Reset flag
}

// Manual dirty marking
player->position.MarkDirty(); // Force replication
```

### Runtime Property Access
```cpp
// Get property by name
IProperty* healthProp = player->GetProperty("health");
if (healthProp) {
    // Type-safe access (assuming we know the type)
    UProperty<int>* intProp = static_cast<UProperty<int>*>(healthProp);
    int currentValue = *intProp; // Conversion operator

    // Generic access
    FString propName = healthProp->GetName();
    FString propPath = healthProp->GetPropertyPath();
    bool isReplicated = healthProp->HasFlag(EPropertyFlags::Replicated);
}
```

### Serialization
```cpp
// Serialize property
std::vector<Byte> healthData = player->health.Serialize();

// Manual serialization with FBinaryWriter
FBinaryWriter writer(data);
writer.Write(player->health.GetValue()); // Direct value serialization

// Deserialize (assuming we have the data)
FBinaryReader reader(healthData);
int deserializedHealth;
reader.Read(deserializedHealth);
player->health.SetValue(deserializedHealth);
```

### Network Replication Setup
```cpp
class AReplicatedActor : public UObject
{
public:
    UPROPERTY(Replicated)
    FVector3 position;

    UPROPERTY(Replicated)
    int score = 0;

    UPROPERTY() // Not replicated
    float internalTimer = 0.0f;

    AReplicatedActor()
        : UObject("ReplicatedActor")
        , position(*this, "position", FVector3::Zero, EPropertyFlags::Replicated)
        , score(*this, "score", 0, EPropertyFlags::Replicated)
        , internalTimer(*this, "internalTimer", 0.0f, EPropertyFlags::None)
    {}

    // Get properties that need replication
    void GetReplicatedProperties(TVector<IProperty*>& outProps) {
        for (const auto& [name, prop] : GetProperties()) {
            if (prop->HasFlag(EPropertyFlags::Replicated)) {
                outProps.PushBack(prop);
            }
        }
    }
};
```

### Editor Integration
```cpp
class AEditableActor : public UObject
{
public:
    UPROPERTY()
    FString displayName = "Actor";

    UPROPERTY(EditorOnly)
    bool debugMode = false;

    UPROPERTY(ReadOnly)
    UUID uniqueID;

    AEditableActor()
        : UObject("EditableActor")
        , displayName(*this, "displayName", "Actor")
        , debugMode(*this, "debugMode", false, EPropertyFlags::EditorOnly)
        , uniqueID(*this, "uniqueID", UUID::V4(), EPropertyFlags::ReadOnly)
    {}
};
```

### Complex Property Types
```cpp
class AComplexActor : public UObject
{
public:
    UPROPERTY(Replicated)
    FVector3 velocity;

    UPROPERTY()
    std::vector<FString> inventory;

    UPROPERTY(Transient)
    FMatrix4x4 cachedTransform;

    AComplexActor()
        : UObject("ComplexActor")
        , velocity(*this, "velocity", FVector3::Zero, EPropertyFlags::Replicated)
        , inventory(*this, "inventory", {})
        , cachedTransform(*this, "cachedTransform", FMatrix4x4::Identity, EPropertyFlags::Transient)
    {}

    void UpdateVelocity(const FVector3& newVelocity) {
        velocity = newVelocity; // Triggers replication if changed
        UpdateCachedTransform();
    }

private:
    void UpdateCachedTransform() {
        // Update cached transform (transient, not replicated)
        cachedTransform = /* compute transform */;
    }
};
```

### Property Validation
```cpp
class AValidatedActor : public UObject
{
public:
    UPROPERTY()
    int health = 100;

    AValidatedActor()
        : UObject("ValidatedActor")
        , health(*this, "health", 100)
    {}

    void TakeDamage(int damage) {
        int newHealth = health.GetValue() - damage;
        newHealth = std::max(0, newHealth); // Clamp to minimum

        if (newHealth != health.GetValue()) {
            health.SetValue(newHealth);
            OnHealthChanged();
        }
    }

private:
    void OnHealthChanged() {
        if (health.GetValue() <= 0) {
            // Handle death
            FLogger::Info("Actor died!");
        }
    }
};
```

## Property Replication System

### Replication Process
1. **Change Detection**: Dirty flag indicates modification
2. **Authority Check**: Only server can modify replicated properties
3. **Serialization**: Property value converted to binary format
4. **Network Transmission**: Data sent to relevant clients
5. **Deserialization**: Clients receive and apply changes
6. **Dirty Flag Reset**: Flag cleared after successful replication

### Replication Flow
```
Server Side:
├── Property Modified → MarkDirty()
├── Replication Tick → Check IsDirty()
├── Serialize Value → Network Packet
└── Send to Clients → ClearDirty()

Client Side:
├── Receive Packet → Find Property
├── Deserialize Value → SetValue()
└── Update Local State
```

## Performance Considerations

### Memory Overhead
- **Per Property**: Additional storage for name, flags, dirty state
- **Type Information**: Template instantiation for each type
- **Registration**: Pointer storage in owner's property map

### Runtime Performance
- **Access Overhead**: Minimal compared to direct member access
- **Dirty Checking**: Simple boolean operations
- **Serialization**: Depends on type complexity
- **Flag Operations**: Bitwise operations are fast

### Optimization Strategies
- **Batch Operations**: Group property updates
- **Lazy Evaluation**: Defer expensive computations
- **Flag Caching**: Cache frequently checked flags
- **Type Specialization**: Optimize for common types

## Integration with Other Systems

### Reflection System
```cpp
// Properties integrate with UClass metadata
UClass* objectClass = GetClass();
if (objectClass && !objectClass->IsRegistered()) {
    objectClass->AddProperty(this);
}
```

### Networking System
```cpp
// Replicated properties work with network layer
void ReplicateProperties(UObject* obj) {
    TVector<IProperty*> replicatedProps;
    obj->GetLifetimeReplicatedProperties(replicatedProps);

    for (IProperty* prop : replicatedProps) {
        if (prop->IsDirty()) {
            // Send replication packet
            Network::SendPropertyUpdate(obj, prop);
            prop->ClearDirty();
        }
    }
}
```

### Serialization System
```cpp
// Properties support save/load
void SerializeObject(UObject* obj, FBinaryWriter& writer) {
    for (const auto& [name, prop] : obj->GetProperties()) {
        if (!prop->HasFlag(EPropertyFlags::Transient)) {
            writer.Write(prop->GetName());
            auto data = prop->Serialize();
            writer.Write(data);
        }
    }
}
```

## Best Practices

### Property Declaration
1. **Descriptive Names**: Use clear, descriptive property names
2. **Appropriate Flags**: Set correct flags for intended usage
3. **Type Selection**: Choose types that are serializable
4. **Access Patterns**: Consider how properties will be accessed

### Replication Design
1. **Minimal Replication**: Only replicate necessary data
2. **Authority Respect**: Don't modify replicated properties on clients
3. **Change Batching**: Group related property changes
4. **Update Frequency**: Consider network bandwidth impact

### Performance Optimization
1. **Flag Checking**: Cache frequently used flag checks
2. **Dirty Management**: Clear dirty flags promptly after processing
3. **Type Awareness**: Use appropriate types for performance
4. **Memory Layout**: Consider property access patterns

## Common Patterns

### Computed Properties
```cpp
class ACharacter : public UObject
{
public:
    UPROPERTY(Replicated)
    int baseHealth = 100;

    UPROPERTY(Replicated)
    int level = 1;

    // Computed property (not stored, calculated on demand)
    int GetMaxHealth() const {
        return baseHealth * level;
    }

    UPROPERTY(ReadOnly)
    int currentHealth;

    ACharacter()
        : UObject("Character")
        , baseHealth(*this, "baseHealth", 100, EPropertyFlags::Replicated)
        , level(*this, "level", 1, EPropertyFlags::Replicated)
        , currentHealth(*this, "currentHealth", 100, EPropertyFlags::ReadOnly)
    {}

    void LevelUp() {
        level = level.GetValue() + 1;
        currentHealth.SetValue(GetMaxHealth()); // Update computed value
    }
};
```

### Property Observers
```cpp
class AObservableActor : public UObject
{
public:
    UPROPERTY(Replicated)
    FVector3 position;

    AObservableActor()
        : UObject("ObservableActor")
        , position(*this, "position", FVector3::Zero, EPropertyFlags::Replicated)
    {}

    void SetPosition(const FVector3& newPos) {
        if (position.GetValue() != newPos) {
            FVector3 oldPos = position.GetValue();
            position.SetValue(newPos);
            OnPositionChanged(oldPos, newPos);
        }
    }

private:
    void OnPositionChanged(const FVector3& oldPos, const FVector3& newPos) {
        // Notify observers
        FLogger::Info("Position changed from {} to {}",
                     oldPos.ToString(), newPos.ToString());
    }
};
```

### Property Validation
```cpp
class AValidatedActor : public UObject
{
public:
    UPROPERTY()
    float speed = 1.0f;

    AValidatedActor()
        : UObject("ValidatedActor")
        , speed(*this, "speed", 1.0f)
    {}

    void SetSpeed(float newSpeed) {
        // Clamp to valid range
        newSpeed = std::clamp(newSpeed, 0.0f, 10.0f);

        if (std::abs(speed.GetValue() - newSpeed) > 0.01f) {
            speed.SetValue(newSpeed);
            OnSpeedChanged();
        }
    }

private:
    void OnSpeedChanged() {
        FLogger::Info("Speed changed to {}", speed.GetValue());
    }
};
```

## Diagram: UProperty Architecture

```
UProperty<T> Class Architecture
├── Template Parameter
│   └── T (ValueType)
├── Core Storage
│   ├── m_name (FString)
│   ├── m_value (T)
│   ├── m_owner (UObject&)
│   ├── m_flags (EPropertyFlags)
│   └── m_isDirty (bool)
├── Access Methods
│   ├── GetValue() / SetValue()
│   ├── operator=() / operator T&()
│   └── operator->()
├── Metadata Methods
│   ├── GetName() / SetName()
│   ├── GetFlags() / SetFlags()
│   └── GetPropertyPath()
├── Change Tracking
│   ├── IsDirty() / MarkDirty()
│   └── ClearDirty()
└── Serialization
    └── Serialize()
```

## Diagram: Property Replication Flow

```mermaid
graph TD
    A[Property Modified] --> B{Authority Check}
    B -->|Server| C[MarkDirty()]
    B -->|Client| D[Reject Change]
    C --> E{Replication Tick}
    E --> F{IsDirty?}
    F -->|Yes| G[Serialize Value]
    F -->|No| H[Skip]
    G --> I[Create Network Packet]
    I --> J[Send to Clients]
    J --> K[ClearDirty()]
    K --> L[Client Receives]
    L --> M[Find Property]
    M --> N[Deserialize Value]
    N --> O[SetValue()]
    O --> P[Update Local State]
```

## Troubleshooting

### Common Issues

1. **Type Mismatch**: Ensure template parameter matches actual usage
2. **Serialization Errors**: Verify type is serializable with FBinaryWriter
3. **Replication Failures**: Check authority and network initialization
4. **Memory Corruption**: Ensure proper object lifetimes
5. **Performance Issues**: Monitor dirty flag usage and serialization overhead

### Debug Techniques
```cpp
// Property inspection
void DebugProperty(IProperty* prop) {
    FLogger::Debug("Property: {}", prop->GetName());
    FLogger::Debug("Path: {}", prop->GetPropertyPath());
    FLogger::Debug("Flags: {}", static_cast<UInt32>(prop->GetFlags()));
    FLogger::Debug("Dirty: {}", prop->IsDirty());
    FLogger::Debug("Value: {}", prop->ToString());
}

// Replication debugging
void DebugReplication(UObject* obj) {
    TVector<IProperty*> replicated;
    obj->GetLifetimeReplicatedProperties(replicated);

    for (IProperty* prop : replicated) {
        if (prop->IsDirty()) {
            FLogger::Debug("Replicating dirty property: {}", prop->GetName());
        }
    }
}
```

This documentation provides a comprehensive overview of the UProperty system, covering its implementation, usage patterns, and integration within the TKD Engine's reflection and networking architecture.
