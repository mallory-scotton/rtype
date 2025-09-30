# LerpAngle

Linearly interpolates between two angles.

## Syntax

```cpp
template <typename T>
static T LerpAngle(const T a, const T b, const T alpha)
```

## Parameters

- `a`: The first angle in radians.
- `b`: The second angle in radians.
- `alpha`: The interpolation factor (0.0 - 1.0).

## Return Value

The interpolated angle in radians.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float a = 0.0f; // 0 degrees
    float b = 3.14159f; // 180 degrees
    float alpha = 0.5f;
    float result = Math<float>::LerpAngle(a, b, alpha); // result ≈ 1.5708f (90 degrees)
}
```
