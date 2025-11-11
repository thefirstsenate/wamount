# Changelog

All notable changes to WAMount will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-11

### Added
- Initial release of WAMount
- APFS container and volume superblock parsing
- Read-only APFS volume mounting on Windows 10/11
- WinFsp integration for user-mode filesystem
- B-tree traversal and object map support
- File and directory reading operations
- Command-line interface with mount/unmount capabilities
- Support for multiple volumes within APFS container
- Debug logging mode
- Comprehensive build system with CMake
- Documentation (README, BUILD, LICENSE)

### Limitations
- Read-only access only (no write support)
- No encryption support (unencrypted volumes only)
- No compression support
- Simplified B-tree implementation
- Basic object map (no full traversal)

### Known Issues
- Large files may have performance issues
- Some APFS features not yet implemented
- Limited error recovery

## [Unreleased]

### Planned Features
- Encrypted volume support
- Compression support (zlib, lzfse)
- Improved B-tree performance
- Full object map traversal
- Extended attributes support
- Hard link support
- Snapshot viewing
- Better error handling and recovery
- Progress indicators for large operations
- Caching layer for improved performance

### Future Enhancements
- Write support (experimental)
- GUI application
- Volume information tool
- Integrity verification
- Benchmark utilities

---

## Version History

- **1.0.0** - Initial public release with basic read-only support
