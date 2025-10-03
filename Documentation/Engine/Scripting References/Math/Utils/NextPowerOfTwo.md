# NextPowerOfTwo

Calculates the next power of two.

## Syntax

```cpp
template <typename T>
static T NextPowerOfTwo(T value)
```

## Parameters

- `value`: The integer to check.

## Return Value

The next power of two greater than or equal to the input. For non-integral types, returns the value unchanged.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int val1 = 5;
    int next1 = Math<int>::NextPowerOfTwo(val1); // next1 = 8

    int val2 = 8;
    int next2 = Math<int>::NextPowerOfTwo(val2); // next2 = 8

    float fval = 5.0f;
    float fnext = Math<float>::NextPowerOfTwo(fval); // fnext = 5.0f
}
```
