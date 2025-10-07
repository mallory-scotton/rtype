# RadiansToDegrees

Converts radians to degrees.

## Syntax

```cpp
template <typename T>
static T RadiansToDegrees(const T radians)
```

## Parameters

- `radians`: The value in radians.

## Return Value

The value converted to degrees.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float radians = 3.14159f;
    float degrees = Math<float>::RadiansToDegrees(radians); // degrees ≈ 180.0f
}
```
