# LerpClamped

Linearly interpolates between two values with clamping.

## Syntax

```cpp
template <typename T>
static T LerpClamped(const T a, const T b, Float32 alpha)
```

## Parameters

- `a`: The first value.
- `b`: The second value.
- `alpha`: The interpolation factor (0.0 - 1.0).

## Return Value

The interpolated value, with alpha clamped to [0.0, 1.0].

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float a = 0.0f;
    float b = 10.0f;
    float alpha = 1.5f; // Will be clamped to 1.0f
    float result = Math<float>::LerpClamped(a, b, alpha); // result = 10.0f
}
```
