# Contributing to WAMount

Thank you for your interest in contributing to WAMount! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Areas for Contribution](#areas-for-contribution)

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help create a welcoming environment
- Report unacceptable behavior

## Getting Started

1. **Fork the repository**
   - Click "Fork" on GitHub
   - Clone your fork locally

2. **Set up development environment**
   - Follow [BUILD.md](BUILD.md) instructions
   - Install all prerequisites

3. **Create a branch**
   ```cmd
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Prerequisites

- Windows 10/11 development machine
- Visual Studio 2019+ or MinGW-w64
- CMake 3.15+
- WinFsp SDK
- Git

### Building for Development

```cmd
# Debug build with symbols
mkdir build-debug
cd build-debug
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Debug
```

### Running Tests

```cmd
# After building
cd build-debug
ctest -C Debug --verbose
```

## Making Changes

### Before You Start

1. **Check existing issues**
   - Look for related issues or PRs
   - Comment on issues you'd like to work on

2. **Discuss major changes**
   - Open an issue first
   - Discuss approach and design
   - Get feedback before coding

### Development Workflow

1. **Write code**
   - Follow coding standards
   - Add comments for complex logic
   - Keep changes focused

2. **Test thoroughly**
   - Test on different APFS volumes
   - Test error conditions
   - Verify no regressions

3. **Document changes**
   - Update relevant documentation
   - Add code comments
   - Update CHANGELOG.md

## Coding Standards

### C++ Style Guide

**General Rules:**
- C++17 standard
- Follow existing code style
- Use meaningful variable names
- Keep functions focused and small

**Naming Conventions:**
```cpp
// Classes: PascalCase
class APFSContainer { };

// Functions: camelCase
bool readSuperblock();

// Variables: snake_case for members
uint32_t block_size_;

// Constants: UPPER_CASE
#define MAX_VOLUMES 100
const int BLOCK_SIZE = 4096;

// Namespaces: lowercase
namespace apfs { }
```

**Code Formatting:**
```cpp
// Braces on same line for functions
bool init() {
    if (condition) {
        // Do something
    }
    return true;
}

// Pointer/reference alignment
void* buffer;
Type& reference;

// Include order
#include <system_headers>
#include <third_party>
#include "local_headers.h"
```

**Error Handling:**
```cpp
// Return false on error, log appropriately
if (!readBlock(0, buffer)) {
    Logger::error("Failed to read block");
    return false;
}

// Use RAII for resources
auto resource = std::make_unique<Resource>();
```

### Documentation

**File Headers:**
```cpp
// Brief description of file purpose
// Additional details if needed
```

**Function Comments:**
```cpp
// Brief description of what function does
//
// @param input Description of parameter
// @return Description of return value
bool functionName(int input);
```

**Inline Comments:**
```cpp
// Explain WHY, not WHAT
// Comment complex algorithms
// Document assumptions
```

## Testing

### Manual Testing

1. **Basic functionality**
   - Mount/unmount
   - Read files
   - List directories

2. **Edge cases**
   - Empty directories
   - Large files
   - Special characters in names

3. **Error conditions**
   - Invalid disks
   - Corrupted structures
   - Permission issues

### Test Cases to Consider

- Single and multiple volumes
- Different APFS versions
- Various file sizes
- Deep directory structures
- Long filenames
- Unicode characters

## Submitting Changes

### Before Submitting

✅ Code builds without warnings
✅ Existing functionality still works
✅ New code is tested
✅ Documentation is updated
✅ Commit messages are clear

### Pull Request Process

1. **Update your branch**
   ```cmd
   git checkout main
   git pull upstream main
   git checkout your-branch
   git rebase main
   ```

2. **Push to your fork**
   ```cmd
   git push origin your-branch
   ```

3. **Create Pull Request**
   - Use descriptive title
   - Explain what and why
   - Reference related issues
   - Include test results

4. **Respond to feedback**
   - Address review comments
   - Update code as needed
   - Be patient and respectful

### Commit Messages

**Format:**
```
Short summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain what and why, not how.

- Bullet points for multiple changes
- Reference issues: Fixes #123
```

**Examples:**
```
Add support for compressed files

Implement LZFSE and zlib decompression for APFS compressed
files. This allows reading compressed data streams.

Fixes #42
```

## Areas for Contribution

### High Priority

- 🔴 **Encryption support**
  - Implement crypto decryption
  - Support for different key types

- 🔴 **B-tree optimization**
  - Improve search performance
  - Cache frequently accessed nodes

- 🔴 **Error handling**
  - Better error messages
  - Recovery from errors
  - Validation checks

### Medium Priority

- 🟡 **Compression support**
  - LZFSE decompression
  - zlib decompression

- 🟡 **Extended attributes**
  - Read extended attributes
  - Resource forks

- 🟡 **Performance**
  - Caching layer
  - Async I/O
  - Memory optimization

### Low Priority

- 🟢 **GUI application**
  - Windows GUI for mounting
  - Volume browser

- 🟢 **Additional features**
  - Snapshot viewing
  - Volume information tool
  - Benchmark utilities

### Documentation

- Improve existing docs
- Add code examples
- Create tutorials
- API documentation

### Testing

- Unit tests
- Integration tests
- Test automation
- Test coverage

## Getting Help

### Resources

- **APFS Specification**
  - Apple File System Reference
  - Reverse engineering docs

- **WinFsp Documentation**
  - https://winfsp.dev/doc/

- **Project Documentation**
  - README.md
  - BUILD.md
  - Source code comments

### Communication

- **GitHub Issues** - Bug reports and features
- **Pull Requests** - Code review and discussion
- **Discussions** - General questions

## Development Tips

### Debugging

**Enable debug logging:**
```cpp
Logger::setLevel(LogLevel::DEBUG);
```

**Use debugger:**
- Set breakpoints in WinFsp callbacks
- Inspect APFS structures
- Step through B-tree traversal

**Common issues:**
- Check OID/XID mappings
- Verify block alignments
- Validate structure sizes

### Understanding APFS

**Key concepts:**
1. Container vs Volume
2. Physical vs Virtual objects
3. B-tree structure
4. Object mapping
5. Checkpointing

**Useful tools:**
- Hex editor (HxD, 010 Editor)
- Disk viewer
- APFS analyzers

### WinFsp Integration

**Important callbacks:**
- GetVolumeInfo - Volume metadata
- Open/Close - File handles
- Read - File reading
- ReadDirectory - Directory listing

**Best practices:**
- Return proper NTSTATUS codes
- Fill all required fields
- Handle Unicode correctly
- Clean up resources

## Recognition

Contributors will be:
- Listed in CONTRIBUTORS.md
- Credited in release notes
- Mentioned in significant features

## Questions?

Don't hesitate to:
- Open an issue for questions
- Ask for clarification
- Request help with setup
- Suggest improvements to this guide

Thank you for contributing to WAMount! 🚀
