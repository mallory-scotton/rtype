# Mod

Modulo function for all arithmetic types.

## Syntax

```cpp
template <typename T>
static T Mod(const T value, const T modulus)
```

## Parameters

- `value`: The value.
- `modulus`: The modulus.

## Return Value

The modulo result. For integral types, ensures the result has the same sign as the modulus. For floating-point types, uses fmod with adjustments.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int val = 7;
    int mod = 3;
    int result = Math<int>::Mod(val, mod); // result = 1

    float fval = 7.5f;
    float fmod = 3.0f;
    float fresult = Math<float>::Mod(fval, fmod); // fresult = 1.5f
}
```
