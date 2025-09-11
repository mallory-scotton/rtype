# 🤖 R-Type

```bash
# Install dependencies
conan install . --output-folder=Build --build=missing --settings=build_type=Release --settings=compiler.cppstd=20 -c tools.system.package_manager:mode=install

# Configure CMake
cmake -B Build -S . -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build Build --config Release
```
