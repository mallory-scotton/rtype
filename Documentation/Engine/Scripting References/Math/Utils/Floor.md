# Floor

Returns the largest integer value not greater than the given value.

## Syntax

```cpp
template <typename T>
static T Floor(const T value)
```

## Parameters

- `value`: The input value of arithmetic type.

## Return Value

The largest integer value not greater than the input value.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float val1 = 3.14f;
    float floor1 = Math<float>::Floor(val1); // floor1 = 3.0f

    float val2 = -2.7f;
    float floor2 = Math<float>::Floor(val2); // floor2 = -3.0f
}
```
