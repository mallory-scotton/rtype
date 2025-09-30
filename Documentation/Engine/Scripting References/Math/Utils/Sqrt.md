# Sqrt

Returns the square root of a value.

## Syntax

```cpp
template <typename T>
static T Sqrt(const T value)
```

## Parameters

- `value`: The value.

## Return Value

The square root of the value. For non-floating-point types, returns the value unchanged.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float val = 9.0f;
    float sqrtVal = Math<float>::Sqrt(val); // sqrtVal = 3.0f

    int ival = 9;
    int isqrt = Math<int>::Sqrt(ival); // isqrt = 9 (unchanged)
}
```
