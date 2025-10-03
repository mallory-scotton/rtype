# Normalize

Normalizes a value from a given range to [0, 1].

## Syntax

```cpp
template <typename T>
static T Normalize(const T value, const T min, const T max)
```

## Parameters

- `value`: The value to normalize.
- `min`: The minimum of the input range.
- `max`: The maximum of the input range.

## Return Value

The normalized value in the range [0, 1].

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float value = 5.0f;
    float min = 0.0f;
    float max = 10.0f;
    float normalized = Math<float>::Normalize(value, min, max); // normalized = 0.5f
}
```
