# Fract

Returns the fractional part of a value.

## Syntax

```cpp
template <typename T>
static T Fract(const T value)
```

## Parameters

- `value`: The input value.

## Return Value

The fractional part of the input value (the part after the decimal point).

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float val = 3.14f;
    float fract = Math<float>::Fract(val); // fract = 0.14f

    float val2 = -2.7f;
    float fract2 = Math<float>::Fract(val2); // fract2 = 0.3f
}
```
