# Ceil

Returns the smallest integer value not less than the given value.

## Syntax

```cpp
template <typename T>
static T Ceil(const T value)
```

## Parameters

- `value`: The input value of arithmetic type.

## Return Value

The smallest integer value not less than the input value.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float val1 = 3.14f;
    float ceil1 = Math<float>::Ceil(val1); // ceil1 = 4.0f

    float val2 = -2.7f;
    float ceil2 = Math<float>::Ceil(val2); // ceil2 = -2.0f
}
```
