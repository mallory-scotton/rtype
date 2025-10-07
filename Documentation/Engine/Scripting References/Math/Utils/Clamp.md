# Clamp

Clamps a value between a minimum and maximum value.

## Syntax

```cpp
template <typename T>
static T Clamp(const T value, const T min, const T max)
```

## Parameters

- `value`: The value to clamp.
- `min`: The minimum value.
- `max`: The maximum value.

## Return Value

The clamped value, which is the minimum if the input is less than the minimum, the maximum if the input is greater than the maximum, or the input value otherwise.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int val = 15;
    int clamped = Math<int>::Clamp(val, 0, 10); // clamped = 10

    float fval = -5.0f;
    float fclamped = Math<float>::Clamp(fval, 0.0f, 1.0f); // fclamped = 0.0f
}
```
