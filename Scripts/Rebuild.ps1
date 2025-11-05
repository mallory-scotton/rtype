# Define paths to add
$paths = @(
    "C:\Program Files\CMake\bin"
    "C:\Program Files\Python312\Scripts"
    "C:\Program Files\Python312"
    "C:\Program Files\Python311\Scripts"
    "C:\Program Files\Python311"
    "$env:USERPROFILE\AppData\Local\Programs\Python\Python312\Scripts"
    "$env:USERPROFILE\AppData\Local\Programs\Python\Python312"
    "$env:USERPROFILE\AppData\Local\Programs\Python\Python311\Scripts"
    "$env:USERPROFILE\AppData\Local\Programs\Python\Python311"
    "$env:USERPROFILE\.local\bin"
    "$env:USERPROFILE\AppData\Roaming\Python\Python312\Scripts"
    "$env:USERPROFILE\AppData\Roaming\Python\Python311\Scripts"
)

# Load existing user PATH
$existing = [System.Environment]::GetEnvironmentVariable("Path", "User")

foreach ($p in $paths) {
    if ($existing -notlike "*$p*") {
        $existing = "$existing;$p"
    }
}

# Update PATH for current session
$env:Path = $existing

# Persist PATH to user environment variables
[System.Environment]::SetEnvironmentVariable("Path", $existing, "User")

Write-Host "PATH updated successfully."

rm Build
mkdir Build
conan install . --output-folder=Build --build=missing --settings=build_type=Release --settings=compiler.cppstd=20 -c tools.system.package_manager:mode=install
cmake -B Build -S . -A x64 -T host=x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_FILE="Build\build\generators\conan_toolchain.cmake"
cmake --build Build --config Release -- /m:1 /maxcpucount:1
