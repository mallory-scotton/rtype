# Pow

Returns the power of a value.

## Syntax

```cpp
template <typename T>
static T Pow(const T base, const T exponent)
```

## Parameters

- `base`: The base value.
- `exponent`: The exponent value.

## Return Value

The base raised to the power of the exponent.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float base = 2.0f;
    float exp = 3.0f;
    float result = Math<float>::Pow(base, exp); // result = 8.0f
}
```
