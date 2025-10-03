# Round

Rounds a value to the nearest integer.

## Syntax

```cpp
template <typename T>
static T Round(const T value)
```

## Parameters

- `value`: The value to round.

## Return Value

The rounded value to the nearest integer.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float val1 = 3.6f;
    float round1 = Math<float>::Round(val1); // round1 = 4.0f

    float val2 = 3.4f;
    float round2 = Math<float>::Round(val2); // round2 = 3.0f
}
```
