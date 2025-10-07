# Lerp

Linearly interpolates between two values.

## Syntax

```cpp
template <typename T>
static T Lerp(const T a, const T b, const Float32 alpha)
```

## Parameters

- `a`: The first value.
- `b`: The second value.
- `alpha`: The interpolation factor (0.0 - x.0).

## Return Value

The interpolated value.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float a = 0.0f;
    float b = 10.0f;
    float alpha = 0.5f;
    float result = Math<float>::Lerp(a, b, alpha); // result = 5.0f
}
```