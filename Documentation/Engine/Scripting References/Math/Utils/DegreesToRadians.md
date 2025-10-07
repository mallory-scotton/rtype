# DegreesToRadians

Converts degrees to radians.

## Syntax

```cpp
template <typename T>
static T DegreesToRadians(const T degrees)
```

## Parameters

- `degrees`: The value in degrees.

## Return Value

The value converted to radians.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float degrees = 180.0f;
    float radians = Math<float>::DegreesToRadians(degrees); // radians ≈ 3.14159f
}
```
