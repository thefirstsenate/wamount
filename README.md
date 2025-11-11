# WAMount - Windows Apple Mount

![Build Status](https://github.com/thefirstsenate/wamount/actions/workflows/build.yml/badge.svg)
![Release](https://img.shields.io/github/v/release/thefirstsenate/wamount?include_prereleases)
![License](https://img.shields.io/github/license/thefirstsenate/wamount)
![Platform](https://img.shields.io/badge/platform-Windows%2010%2F11-blue)

WAMount (Windows Apple Mount) is a user-mode filesystem driver that enables mounting and reading APFS (Apple File System) partitions on Windows 10/11.

## 📥 Download

**Pre-built binaries** are available from [GitHub Releases](https://github.com/thefirstsenate/wamount/releases/latest).

Download the latest `wamount-vX.X.X-windows-x64.zip`, extract it, and follow the [Quick Start Guide](QUICKSTART.md).

**Building from source?** See [BUILD.md](BUILD.md) for detailed instructions.

## Architecture

WAMount uses **WinFsp** (Windows File System Proxy) to implement a user-mode filesystem driver, avoiding the complexity of kernel-mode drivers. This approach provides:

- **Easy Installation**: No driver signing required
- **Safe Operation**: Runs in user space, crashes won't affect system
- **Simple Usage**: Standard application that runs while partition is mounted
- **Read-Only Support**: Initially supports read-only access to APFS volumes

## Technical Overview

### APFS Structure
APFS (Apple File System) is a copy-on-write filesystem with the following key components:

1. **Container**: Outermost structure containing multiple volumes
2. **Superblock**: Located at block 0, contains container information
3. **Checkpoint Descriptor Area**: Manages filesystem checkpoints
4. **Object Map**: Maps virtual addresses to physical addresses
5. **B-Trees**: Hierarchical structures for files, directories, and metadata
6. **Volumes**: Individual filesystems within a container

### Implementation Components

```
WAMount/
├── src/
│   ├── apfs/
│   │   ├── container.cpp/h       - APFS container parsing
│   │   ├── superblock.cpp/h      - Superblock structures
│   │   ├── object_map.cpp/h      - Object mapping
│   │   ├── btree.cpp/h           - B-tree implementation
│   │   ├── volume.cpp/h          - Volume management
│   │   ├── inode.cpp/h           - File/directory nodes
│   │   └── crypto.cpp/h          - Decryption (if needed)
│   ├── winfsp/
│   │   └── winfsp_ops.cpp/h      - WinFsp interface implementation
│   ├── utils/
│   │   ├── disk_reader.cpp/h     - Low-level disk access
│   │   └── logging.cpp/h         - Debug logging
│   └── main.cpp                   - CLI interface
├── include/                       - Public headers
├── CMakeLists.txt                 - Build configuration
└── README.md                      - This file
```

## Requirements

- **Windows 10/11** (64-bit)
- **WinFsp** 1.12 or later ([Download](https://winfsp.dev/))
- **Visual Studio 2019+** or **MinGW-w64** (for building)
- **CMake 3.15+** (for building)

## Building

### Using CMake with Visual Studio

```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```

### Using CMake with MinGW

```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Usage

### Basic Usage

```cmd
# Mount APFS partition
wamount.exe -m X: \\.\PhysicalDrive1

# Mount with volume selection (if container has multiple volumes)
wamount.exe -m X: \\.\PhysicalDrive1 -v 0

# Unmount (Ctrl+C or close window)
```

### Advanced Options

```cmd
wamount.exe [options] -m <mountpoint> <device>

Options:
  -m <drive>      Mount point (e.g., X:)
  -v <index>      Volume index (default: 0)
  -d              Enable debug output
  -r              Read-only mode (default)
  -h              Show help
```

### Finding Your APFS Partition

Use Windows Disk Management or `diskpart` to identify your physical drive:

```cmd
diskpart
DISKPART> list disk
DISKPART> exit
```

Then mount using `\\.\PhysicalDrive#` where # is your disk number.

## Current Status

- [x] Project structure
- [x] APFS container parsing
- [x] Superblock reading
- [x] Object map implementation
- [x] B-tree parsing
- [x] File/directory reading
- [x] WinFsp integration
- [x] Basic CLI
- [ ] Encryption support
- [ ] Write support (future)
- [ ] Compression support

## Limitations

- **Read-only**: Currently supports read-only access
- **No Encryption**: Encrypted volumes not yet supported
- **No Compression**: Compressed files read as-is
- **Windows Only**: Designed for Windows 10/11

## Technical References

- [Apple File System Reference](https://developer.apple.com/support/downloads/Apple-File-System-Reference.pdf)
- [WinFsp Documentation](https://winfsp.dev/doc/)
- APFS filesystem specifications and reverse engineering documentation

## License

This project is provided as-is for educational and personal use.

## Contributing

Contributions welcome! Please ensure code follows the existing style and includes appropriate error handling.

## Disclaimer

This software is provided "as-is" without warranty. Use at your own risk. Always backup important data before mounting partitions with third-party tools.
