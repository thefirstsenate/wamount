# Creating a Release

This guide explains how to create a new release of WAMount using GitHub Actions.

## Automatic Release Workflow

WAMount uses GitHub Actions to automatically build and publish releases. The process is fully automated.

## Creating a Release

### Method 1: Git Tag (Recommended)

1. **Update version numbers**
   ```bash
   # Update CHANGELOG.md with new version
   # Update version in CMakeLists.txt if needed
   ```

2. **Commit changes**
   ```bash
   git add CHANGELOG.md CMakeLists.txt
   git commit -m "Prepare release v1.0.0"
   git push
   ```

3. **Create and push tag**
   ```bash
   git tag -a v1.0.0 -m "Release v1.0.0"
   git push origin v1.0.0
   ```

4. **Wait for automation**
   - GitHub Actions will automatically:
     - Build WAMount on Windows
     - Create release package
     - Generate release notes
     - Create GitHub release
     - Upload binaries

5. **Check the release**
   - Go to: https://github.com/thefirstsenate/wamount/releases
   - Verify the new release is published
   - Download and test the binary

### Method 2: Manual Trigger

1. **Go to Actions tab**
   - Navigate to: https://github.com/thefirstsenate/wamount/actions

2. **Select "Release WAMount" workflow**

3. **Click "Run workflow"**

4. **Enter version**
   - Format: `v1.0.0`
   - Click "Run workflow"

5. **Wait for completion**
   - Monitor the workflow run
   - Check the Releases page when done

## Release Checklist

Before creating a release:

- [ ] Update CHANGELOG.md with new version and changes
- [ ] Update version in CMakeLists.txt (if changed)
- [ ] Test build locally
- [ ] Review and merge all PRs for this release
- [ ] Ensure CI builds are passing
- [ ] Update documentation if needed
- [ ] Write release notes highlights

## Version Numbering

WAMount follows [Semantic Versioning](https://semver.org/):

- **MAJOR.MINOR.PATCH** (e.g., 1.0.0)
  - **MAJOR**: Incompatible API changes
  - **MINOR**: New functionality (backward compatible)
  - **PATCH**: Bug fixes (backward compatible)

Examples:
- `v1.0.0` - Initial release
- `v1.0.1` - Bug fix release
- `v1.1.0` - New features
- `v2.0.0` - Breaking changes

## Pre-releases

For testing or preview releases:

```bash
git tag -a v1.1.0-beta.1 -m "Beta release v1.1.0-beta.1"
git push origin v1.1.0-beta.1
```

Pre-release versions:
- `v1.0.0-alpha.1` - Early testing
- `v1.0.0-beta.1` - Feature complete, testing
- `v1.0.0-rc.1` - Release candidate

## Release Contents

Each release includes:

1. **wamount-vX.X.X-windows-x64.zip** containing:
   - `wamount.exe` - Main executable
   - `README.md` - Documentation
   - `README.txt` - Quick start (text format)
   - `QUICKSTART.md` - Getting started guide
   - `BUILD.md` - Build instructions
   - `LICENSE` - License file
   - `CHANGELOG.md` - Version history

2. **Release notes** with:
   - Version highlights
   - Installation instructions
   - Known issues
   - Changelog
   - Download links

## Workflow Details

### Build Process

1. **Checkout code** from tag
2. **Install WinFsp** SDK
3. **Configure** with CMake
4. **Build** with Visual Studio
5. **Package** binaries and docs
6. **Create** GitHub release
7. **Upload** release assets

### Build Configuration

- **Platform**: Windows Server 2022 (GitHub hosted)
- **Compiler**: Visual Studio 2022
- **Architecture**: x64 (64-bit)
- **Configuration**: Release (optimized)
- **WinFsp**: 2.0.23075

### Artifacts

Build artifacts are available:
- **Release package**: 90 days retention
- **Build logs**: 30 days retention

## Troubleshooting

### Build Fails

1. Check Actions tab for error logs
2. Test build locally first
3. Verify WinFsp is available
4. Check CMakeLists.txt syntax

### Release Not Created

1. Verify tag format: `vX.X.X`
2. Check workflow permissions
3. Review workflow logs
4. Ensure GITHUB_TOKEN has release permissions

### Missing Files

1. Check file paths in workflow
2. Verify files exist in repository
3. Review package creation step

## Manual Release (Emergency)

If automation fails, create release manually:

1. **Build locally**
   ```cmd
   mkdir build
   cd build
   cmake -G "Visual Studio 17 2022" -A x64 ..
   cmake --build . --config Release
   ```

2. **Create package**
   - Copy `build\Release\wamount.exe`
   - Add README, LICENSE, etc.
   - Create ZIP file

3. **Create GitHub release**
   - Go to Releases → Draft new release
   - Choose tag
   - Write release notes
   - Upload ZIP file
   - Publish release

## Post-Release

After releasing:

1. **Test the release**
   - Download from GitHub
   - Test on clean Windows VM
   - Verify installation

2. **Announce**
   - Update README if needed
   - Create discussion post
   - Update documentation site (if any)

3. **Monitor**
   - Watch for issues
   - Respond to feedback
   - Plan next release

## Release Schedule

Suggested schedule:
- **Patch releases**: As needed for critical bugs
- **Minor releases**: Every 1-3 months
- **Major releases**: When breaking changes accumulated

## Questions?

For release issues:
- Check workflow logs in Actions tab
- Open an issue on GitHub
- Review this documentation

---

Last updated: 2025-01-11
