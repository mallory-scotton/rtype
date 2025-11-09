# 🤝 Contributing to R-Type

Thank you for your interest in contributing to the R-Type project! We welcome contributions from developers of all skill levels. This document provides guidelines for contributing to the project.

---

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Documentation](#documentation)
- [Submitting Changes](#submitting-changes)
- [Review Process](#review-process)
- [Community](#community)

---

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report unacceptable behavior to [mscotton.pro@gmail.com](mailto:mscotton.pro@gmail.com).

---

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **Development Environment**:
   - CMake 3.21+
   - C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
   - Python 3.7+ with pip
   - Git 2.0+

2. **Knowledge**:
   - C++ programming (C++11/14/17/20 features)
   - Basic understanding of game engine architecture
   - Familiarity with CMake build system
   - Git version control

3. **Read the Documentation**:
   - [Project Overview](Documentation/01.%20Project%20Overview.md)
   - [Getting Started Guide](Documentation/02.%20Getting%20Started.md)
   - [Features Documentation](Documentation/03.%20Features.md)
   - [Directory Layout](Documentation/04.%20Directory%20Layout.md)

### Setting Up Your Development Environment

1. **Fork the Repository**:
   ```bash
   # Fork the repository on GitHub, then clone your fork
   git clone https://github.com/YOUR_USERNAME/rtype.git
   cd rtype
   ```

2. **Add Upstream Remote**:
   ```bash
   git remote add upstream https://github.com/mallory-scotton/rtype.git
   git fetch upstream
   ```

3. **Build the Project**:
   ```bash
   # Use automated build script
   ./Scripts/Build.sh  # Linux/macOS
   # or
   Scripts\Build.bat   # Windows
   ```

4. **Verify Installation**:
   ```bash
   cd Build/bin
   ./TKDEngineTests  # Run engine tests
   ```

---

## How to Contribute

### Types of Contributions

We welcome various types of contributions:

#### 🐛 Bug Reports
- Found a bug? Open an issue with:
  - Clear description of the problem
  - Steps to reproduce
  - Expected vs actual behavior
  - Environment details (OS, compiler, version)
  - Error messages or logs

#### ✨ Feature Requests
- Have an idea? Open an issue describing:
  - The feature and its use case
  - Why it would benefit the project
  - Possible implementation approach
  - Examples from other projects (if applicable)

#### 📝 Documentation
- Improve documentation:
  - Fix typos or unclear explanations
  - Add examples or tutorials
  - Translate documentation
  - Write API documentation

#### 💻 Code Contributions
- Submit code changes:
  - Bug fixes
  - New features
  - Performance improvements
  - Refactoring
  - Test coverage

#### 🎨 Assets and Examples
- Contribute game assets or example projects
- Share demos showcasing engine capabilities

---

## Development Workflow

### Branching Strategy

We follow Git Flow branching model:

- **`main`**: Production-ready code (stable releases)
- **`develop`**: Integration branch (next release)
- **`feature/*`**: New features (branch from `develop`)
- **`bugfix/*`**: Bug fixes (branch from `develop`)
- **`hotfix/*`**: Critical fixes (branch from `main`)
- **`release/*`**: Release preparation (branch from `develop`)

### Creating a Feature Branch

```bash
# Update your local develop branch
git checkout develop
git pull upstream develop

# Create a feature branch
git checkout -b feature/my-awesome-feature

# Work on your changes
# ... make changes, commit, etc ...

# Keep your branch updated
git fetch upstream
git rebase upstream/develop
```

### Commit Message Guidelines

Follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, no logic change)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding or updating tests
- `build`: Build system changes
- `ci`: CI/CD changes
- `chore`: Maintenance tasks

**Examples**:

```bash
# Good commit messages
git commit -m "feat(network): add UDP packet fragmentation"
git commit -m "fix(renderer): correct camera projection matrix calculation"
git commit -m "docs(contributing): add commit message guidelines"
git commit -m "perf(collision): optimize spatial partitioning algorithm"

# With body
git commit -m "feat(audio): implement 3D spatial audio positioning

- Add position, velocity, and attenuation support
- Integrate Doppler effect calculations
- Update AudioSource interface with spatial methods

Resolves #123"
```

**Commit Message Rules**:
- Use present tense ("add feature" not "added feature")
- Use imperative mood ("move cursor to..." not "moves cursor to...")
- First line should be 72 characters or less
- Reference issues and pull requests when applicable
- Explain *what* and *why*, not *how* (code shows how)

---

## Coding Standards

### Code Style

We use **Clang-Format v19** for consistent formatting:

```bash
# Format a single file
clang-format-19 -i path/to/file.cpp

# Format all changed files
git diff --name-only | grep -E '\.(cpp|hpp|h|c)$' | xargs clang-format-19 -i

# Use CMake target
cmake --build Build --target format
```

**Pre-commit Hook**: The project includes a pre-commit hook that automatically formats code. Ensure it's installed:

```bash
# The build script installs this automatically
cp .github/hooks/pre-commit .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit
```

### C++ Style Guidelines

#### Naming Conventions

Follow the established naming patterns:

```cpp
// Classes
class AActor;           // Actor classes (A prefix)
class UComponent;       // UObject-derived (U prefix)
struct FTransform;      // Plain structs (F prefix)
enum class ELogLevel;   // Enumerations (E prefix)
interface IRenderer;    // Interfaces (I prefix)
template<typename T>
class TVector;          // Templates (T prefix)

// Variables
int m_health;           // Member variables (m_ prefix)
static int s_count;     // Static variables (s_ prefix)
int localVariable;      // Local variables (camelCase)
const int MAX_PLAYERS;  // Constants (UPPER_SNAKE_CASE)

// Functions
void DoSomething();     // PascalCase for public methods
void doInternalWork();  // camelCase for private methods
```

#### File Organization

```cpp
///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core.hpp>
#include <Engine/Runtime.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Brief description of the class
///
/// Detailed description providing more context about the class purpose,
/// usage, and any important notes.
///
///////////////////////////////////////////////////////////////////////////////
class MyClass
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Members
    ///////////////////////////////////////////////////////////////////////////
    int m_memberVariable;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Brief description of the method
    ///
    /// \param paramName Description of parameter
    ///
    /// \return Description of return value
    ///
    ///////////////////////////////////////////////////////////////////////////
    int GetValue(void) const;
};

}   // namespace tkd
```

#### Best Practices

1. **Header Guards**: Use `#pragma once`
2. **Const Correctness**: Mark methods `const` when appropriate
3. **Smart Pointers**: Prefer `TUniquePtr` and `TSharedPtr` over raw pointers
4. **RAII**: Use Resource Acquisition Is Initialization
5. **Namespaces**: All code in `tkd` namespace
6. **Error Handling**: Use exceptions for exceptional cases
7. **Documentation**: Document all public APIs with Doxygen comments
8. **Modern C++**: Use C++20 features appropriately

#### Code Example

```cpp
// Good: Modern C++ with clear naming
class UHealthComponent : public UActorComponent
{
private:
    Float32 m_health;
    Float32 m_maxHealth;

public:
    UHealthComponent(Float32 maxHealth = 100.0f)
        : m_health(maxHealth)
        , m_maxHealth(maxHealth)
    {}

    void TakeDamage(Float32 amount) {
        m_health = FMath::Max(0.0f, m_health - amount);
        OnHealthChanged.Broadcast(m_health, m_maxHealth);
    }

    [[nodiscard]] Float32 GetHealthPercent() const {
        return m_health / m_maxHealth;
    }

    TDelegate<Float32, Float32> OnHealthChanged;
};
```

---

## Testing Guidelines

### Running Tests

```bash
cd Build/bin
./TKDEngineTests           # Run all tests
./TKDEngineTests "[Core]"  # Run specific test suite
./TKDEngineTests --success # Verbose output
```

### Writing Tests

We use **Catch2** for unit testing:

```cpp
#include <catch2/catch_test_macros.hpp>
#include <Engine/Core/Math/FVector3.hpp>

TEST_CASE("FVector3 operations", "[Core][Math]") {
    SECTION("Addition") {
        FVector3 a(1.0f, 2.0f, 3.0f);
        FVector3 b(4.0f, 5.0f, 6.0f);
        FVector3 result = a + b;
        
        REQUIRE(result.x == 5.0f);
        REQUIRE(result.y == 7.0f);
        REQUIRE(result.z == 9.0f);
    }
    
    SECTION("Normalization") {
        FVector3 v(3.0f, 4.0f, 0.0f);
        FVector3 normalized = v.Normalized();
        
        REQUIRE(normalized.Length() == Approx(1.0f));
    }
}
```

### Test Coverage

- **Unit Tests**: Test individual functions and classes
- **Integration Tests**: Test interactions between components
- **Performance Tests**: Benchmark critical systems
- **Network Tests**: Test client-server communication

**New features must include tests** covering:
- Normal operation
- Edge cases
- Error conditions
- Performance (if critical)

---

## Documentation

### Code Documentation

Use **Doxygen** style comments for all public APIs:

```cpp
///////////////////////////////////////////////////////////////////////////////
/// \brief Calculate the dot product of two vectors
///
/// Computes the scalar dot product between this vector and another.
/// The dot product is useful for:
/// - Calculating angles between vectors
/// - Projecting one vector onto another
/// - Testing perpendicularity (dot product = 0)
///
/// \param other The other vector to compute dot product with
///
/// \return The scalar dot product value
///
/// \code
/// FVector3 a(1.0f, 0.0f, 0.0f);
/// FVector3 b(0.0f, 1.0f, 0.0f);
/// Float32 dot = a.Dot(b);  // Returns 0.0f (perpendicular)
/// \endcode
///
///////////////////////////////////////////////////////////////////////////////
Float32 Dot(const FVector3& other) const;
```

### Documentation Files

When adding new features, update relevant documentation:

1. **Module Documentation**: `Documentation/Engine/Modules/`
2. **API Reference**: `Documentation/Engine/Scripting References/`
3. **README Files**: Update module-specific READMEs
4. **Examples**: Add usage examples

### Documentation Standards

- Write clear, concise explanations
- Include code examples
- Explain *why*, not just *what*
- Keep documentation in sync with code
- Use proper Markdown formatting

---

## Submitting Changes

### Pull Request Process

1. **Prepare Your Changes**:
   ```bash
   # Ensure code is formatted
   cmake --build Build --target format
   
   # Run tests
   cd Build/bin && ./TKDEngineTests
   
   # Commit your changes
   git add .
   git commit -m "feat(module): description"
   ```

2. **Push to Your Fork**:
   ```bash
   git push origin feature/my-awesome-feature
   ```

3. **Create Pull Request**:
   - Go to GitHub and create a PR from your fork to `upstream/develop`
   - Fill out the PR template completely
   - Link related issues

4. **PR Checklist**:
   - [ ] Code follows project style guidelines
   - [ ] All tests pass
   - [ ] New tests added for new features
   - [ ] Documentation updated
   - [ ] Commit messages follow conventions
   - [ ] No merge conflicts with develop
   - [ ] PR description explains changes clearly

### Pull Request Template

When creating a PR, include:

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Related Issues
Fixes #(issue)

## Testing
Describe testing performed:
- Test cases added
- Manual testing steps
- Performance impact

## Screenshots (if applicable)
Add screenshots for visual changes

## Checklist
- [ ] Code formatted with clang-format-19
- [ ] All tests pass
- [ ] Documentation updated
- [ ] Commit messages follow conventions
```

---

## Review Process

### What to Expect

1. **Automated Checks**: CI/CD pipeline runs automatically
   - Build verification (Linux, Windows, macOS)
   - Test execution
   - Code style validation
   - Static analysis

2. **Code Review**: Maintainers will review your PR
   - May request changes or clarifications
   - Address feedback promptly
   - Be open to suggestions

3. **Approval and Merge**: Once approved:
   - PR is merged into `develop` branch
   - Changes included in next release
   - You'll be credited in CHANGELOG

### Review Criteria

Code reviews focus on:

- **Correctness**: Does it work as intended?
- **Design**: Is it well-architected?
- **Readability**: Is it easy to understand?
- **Testing**: Is it adequately tested?
- **Documentation**: Is it properly documented?
- **Performance**: Are there performance implications?
- **Security**: Are there security concerns?

### Addressing Feedback

```bash
# Make requested changes
git add .
git commit -m "fix: address review comments"

# Update your PR
git push origin feature/my-awesome-feature
```

---

## Community

### Getting Help

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and discussions
- **Email**: [mscotton.pro@gmail.com](mailto:mscotton.pro@gmail.com)

### Staying Updated

- Watch the repository for notifications
- Read the [CHANGELOG](CHANGELOG.md) for updates
- Check [TODO.md](TODO.md) for planned features

### Recognition

Contributors are recognized in:
- [AUTHORS.md](AUTHORS.md) file
- CHANGELOG for their contributions
- Git commit history

---

## Development Tips

### Debugging

```bash
# Build in debug mode
cmake -B Build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build Build

# Run with debugger
gdb ./Build/bin/RTypeGameClient
```

### Performance Profiling

```bash
# Build with profiling
cmake -B Build -S . -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build Build

# Profile with tools
perf record ./Build/bin/RTypeGameClient
perf report
```

### Common Issues

1. **Build Errors**: Ensure all dependencies installed via Conan
2. **Test Failures**: Run tests individually to isolate issues
3. **Format Issues**: Run clang-format before committing
4. **Merge Conflicts**: Keep your branch updated with develop

---

## Additional Resources

### Useful Links

- **Repository**: https://github.com/mallory-scotton/rtype
- **CMake Documentation**: https://cmake.org/documentation/
- **Conan Documentation**: https://docs.conan.io/
- **SFML Documentation**: https://www.sfml-dev.org/documentation/
- **Catch2 Documentation**: https://github.com/catchorg/Catch2

### Learning Resources

- [C++20 Features](https://en.cppreference.com/w/cpp/20)
- [Game Engine Architecture](https://www.gameenginebook.com/)
- [Network Game Programming](https://gafferongames.com/)
- [ECS Architecture](https://austinmorlan.com/posts/entity_component_system/)

---

## License

By contributing to R-Type, you agree that your contributions will be licensed under the [MIT License](LICENSE.md).

---

## Questions?

If you have questions about contributing, feel free to:

- Open a GitHub Discussion
- Email the maintainers
- Ask in your pull request

Thank you for contributing to R-Type! 🎮🚀
