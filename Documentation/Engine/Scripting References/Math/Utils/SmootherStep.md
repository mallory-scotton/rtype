# SmootherStep

Smoother step interpolation.

## Syntax

```cpp
template <typename T>
static T SmootherStep(const T edge0, const T edge1, const T x)
```

## Parameters

- `edge0`: The lower edge of the transition.
- `edge1`: The upper edge of the transition.
- `x`: The value to interpolate.

## Return Value

The interpolated value using a smoother step function.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float edge0 = 0.0f;
    float edge1 = 1.0f;
    float x = 0.5f;
    float result = Math<float>::SmootherStep(edge0, edge1, x); // result ≈ 0.5f (smoothed)
}
```
