# Abs

Returns the absolute value of a given number.

## Syntax

```cpp
template <typename T>
static T Abs(const T value)
```

## Parameters

- `value`: The input value of arithmetic type.

## Return Value

The absolute value of the input. For signed types, returns the positive equivalent; for unsigned types, returns the value unchanged.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int negative = -10;
    int positive = Math<int>::Abs(negative); // positive = 10

    float negFloat = -3.14f;
    float posFloat = Math<float>::Abs(negFloat); // posFloat = 3.14f

    unsigned int unsignedVal = 42;
    unsigned int absUnsigned = Math<unsigned int>::Abs(unsignedVal); // absUnsigned = 42
}
```
