# Singleton

## Description

The `Singleton` class template provides a thread-safe implementation of the Singleton design pattern in C++. It ensures that only one instance of a class exists throughout the application's lifetime and provides global access to that instance. This implementation uses `std::unique_ptr` for automatic memory management and `std::call_once` for thread-safe initialization.

The `Singleton` class inherits from `Instantiable`, which prevents copying and assignment to maintain the singleton invariant.

## Template Parameters

- **T**: The type of the class that will be made a singleton. This type must inherit from `Singleton<T>`.

## Constructors

### Protected Constructor
```cpp
Singleton(void) = default;
```
- **Description**: Default constructor, protected to prevent direct instantiation.
- **Access**: Protected

## Destructor

### Protected Destructor
```cpp
virtual ~Singleton() = default;
```
- **Description**: Default virtual destructor, protected to prevent direct deletion.
- **Access**: Protected

## Static Functions

### GetInstance (No Arguments)
```cpp
static T& GetInstance(void)
```
- **Description**: Returns a reference to the singleton instance. Creates the instance if it doesn't exist using the default constructor.
- **Return Type**: `T&` - Reference to the singleton instance
- **Thread Safety**: Thread-safe due to `std::call_once`

### GetInstance (With Arguments)
```cpp
template <typename... Args>
static T& GetInstance(Args&&... args)
```
- **Description**: Returns a reference to the singleton instance. Creates the instance if it doesn't exist, forwarding the provided arguments to the constructor. Subsequent calls ignore the arguments and return the existing instance.
- **Template Parameters**:
  - **Args**: Variadic template parameter pack for constructor arguments
- **Parameters**:
  - **args**: Arguments to forward to the constructor (only used on first creation)
- **Return Type**: `T&` - Reference to the singleton instance
- **Thread Safety**: Thread-safe due to `std::call_once`

### CreateInstance
```cpp
template <typename... Args>
static T& CreateInstance(Args&&... args)
```
- **Description**: Alias for `GetInstance` with arguments, provided for clarity when creating an instance with constructor arguments.
- **Template Parameters**:
  - **Args**: Variadic template parameter pack for constructor arguments
- **Parameters**:
  - **args**: Arguments to forward to the constructor
- **Return Type**: `T&` - Reference to the singleton instance
- **Thread Safety**: Thread-safe due to `std::call_once`

### HasInstance
```cpp
static Bool HasInstance(void)
```
- **Description**: Checks if the singleton instance has been created.
- **Return Type**: `Bool` - `true` if instance exists, `false` otherwise

### DestroyInstance
```cpp
static void DestroyInstance(void)
```
- **Description**: Destroys the singleton instance and resets the once flag, allowing for recreation. Useful for controlled cleanup and testing.
- **Return Type**: `void`

### GetInstancePtr
```cpp
static T* GetInstancePtr(void)
```
- **Description**: Returns a raw pointer to the singleton instance, or `nullptr` if not created.
- **Return Type**: `T*` - Raw pointer to the instance or `nullptr`

## Example Usage

### Basic Usage
```cpp
#include <Engine/Core/Utils/Singleton.hpp>

// Define a singleton class
class MyManager : public tkd::Singleton<MyManager>
{
public:
    void DoSomething() {
        // Implementation
    }
};

// Usage
int main() {
    MyManager& manager = MyManager::GetInstance();
    manager.DoSomething();
    return 0;
}
```

### Usage with Constructor Arguments
```cpp
#include <Engine/Core/Utils/Singleton.hpp>

// Define a singleton class with constructor arguments
class ConfigManager : public tkd::Singleton<ConfigManager>
{
public:
    ConfigManager(const std::string& configPath) : m_configPath(configPath) {
        // Load configuration
    }

    const std::string& GetConfigPath() const { return m_configPath; }

private:
    std::string m_configPath;
};

// Usage
int main() {
    ConfigManager& config = ConfigManager::CreateInstance("/path/to/config.ini");
    std::cout << "Config path: " << config.GetConfigPath() << std::endl;
    return 0;
}
```

### Checking Instance Existence
```cpp
#include <Engine/Core/Utils/Singleton.hpp>

class Logger : public tkd::Singleton<Logger>
{
    // Implementation
};

void SomeFunction() {
    if (Logger::HasInstance()) {
        Logger::GetInstance().Log("Instance exists");
    } else {
        Logger::GetInstance().Log("Instance created");
    }
}
```

### Manual Destruction
```cpp
#include <Engine/Core/Utils/Singleton.hpp>

class ResourceManager : public tkd::Singleton<ResourceManager>
{
    // Implementation with resources to clean up
};

void Cleanup() {
    ResourceManager::DestroyInstance();
    // Resources are now cleaned up
}
```

## Notes

- The singleton instance is created lazily (on first access).
- Thread-safe initialization is guaranteed using `std::call_once`.
- Memory is automatically managed using `std::unique_ptr`.
- Copying and assignment are disabled through the `Instantiable` base class.
- The `DestroyInstance` function allows for controlled cleanup, which is useful for testing or application shutdown scenarios.
