# GitHub Actions Workflows

WAMount uses GitHub Actions for automated building, testing, and releasing. This document explains each workflow.

## Active Workflows

### 1. Build WAMount (`build.yml`)

**Purpose**: Continuous integration - builds WAMount on every push and PR.

**Triggers**:
- Push to `main` branch
- Push to `claude/**` branches
- Pull requests to `main`
- Manual dispatch

**What it does**:
1. Builds both Release and Debug configurations
2. Installs WinFsp automatically
3. Compiles with Visual Studio 2022
4. Creates downloadable artifacts
5. Uploads binaries for 30 days (Release) or 7 days (Debug)

**Artifacts**:
- `wamount-windows-x64` - Release build + docs
- `wamount-windows-x64-debug` - Debug build + symbols

**Status**: ![Build Status](https://github.com/thefirstsenate/wamount/actions/workflows/build.yml/badge.svg)

**View runs**: [Build workflow runs](https://github.com/thefirstsenate/wamount/actions/workflows/build.yml)

---

### 2. Release WAMount (`release.yml`)

**Purpose**: Create GitHub releases with packaged binaries.

**Triggers**:
- Push tag matching `v*.*.*` (e.g., `v1.0.0`)
- Manual dispatch with version input

**What it does**:
1. Builds release configuration
2. Creates release package with:
   - Executable
   - Documentation files
   - Quick start guide
   - LICENSE
3. Generates release notes
4. Creates GitHub release
5. Uploads ZIP package as release asset

**Package contents**:
```
wamount-vX.X.X-windows-x64.zip
├── wamount.exe
├── README.md
├── README.txt
├── QUICKSTART.md
├── BUILD.md
├── LICENSE
└── CHANGELOG.md
```

**View runs**: [Release workflow runs](https://github.com/thefirstsenate/wamount/actions/workflows/release.yml)

---

### 3. PR Checks (`pr-check.yml`)

**Purpose**: Quality checks for pull requests.

**Triggers**:
- Pull requests to `main`
- Manual dispatch

**What it does**:

**Code Quality**:
- ✅ Verifies required files exist
- ✅ Checks file permissions
- ✅ Warns about large files
- ✅ Checks line endings
- ✅ Validates documentation

**Build Verification**:
- ✅ Confirms code compiles
- ✅ Verifies executable is created
- ✅ Reports file size

**View runs**: [PR check runs](https://github.com/thefirstsenate/wamount/actions/workflows/pr-check.yml)

---

## Workflow Configuration

### Build Environment

All workflows use:
- **OS**: Windows Server 2022 (latest)
- **Compiler**: Visual Studio 2022
- **Architecture**: x64 (64-bit)
- **CMake**: Latest available
- **WinFsp**: 2.0.23075

### Caching

Currently, no caching is configured. Future optimization could include:
- WinFsp installer caching
- CMake build cache
- Dependency caching

### Secrets

No secrets required for public repository. For private repositories:
- `GITHUB_TOKEN` - Automatically provided for releases

## Using Workflow Artifacts

### Download Build Artifacts

1. Go to [Actions tab](https://github.com/thefirstsenate/wamount/actions)
2. Click on a successful workflow run
3. Scroll to "Artifacts" section
4. Download the artifact ZIP

### Testing Development Builds

```cmd
# Download artifact from Actions
# Extract wamount.exe
# Test with:
wamount.exe -h
```

## Manual Workflow Dispatch

### Running Build Manually

1. Go to: [Build workflow](https://github.com/thefirstsenate/wamount/actions/workflows/build.yml)
2. Click "Run workflow"
3. Select branch
4. Click "Run workflow"

### Creating Release Manually

1. Go to: [Release workflow](https://github.com/thefirstsenate/wamount/actions/workflows/release.yml)
2. Click "Run workflow"
3. Enter version (e.g., `v1.0.0`)
4. Click "Run workflow"

## Workflow Permissions

Required permissions for workflows:

- **Contents**: Write (for creating releases)
- **Actions**: Read (for downloading artifacts)
- **Pull Requests**: Write (for PR comments)

## Monitoring Workflows

### Status Badges

Add to README or other docs:

```markdown
![Build](https://github.com/thefirstsenate/wamount/actions/workflows/build.yml/badge.svg)
![Release](https://github.com/thefirstsenate/wamount/actions/workflows/release.yml/badge.svg)
```

### Email Notifications

GitHub sends notifications for:
- Failed workflows (if you have permissions)
- Workflow completions (if enabled)

Configure in: Settings → Notifications

## Troubleshooting Workflows

### Build Failures

**Common issues**:
1. **WinFsp installation fails**
   - Check download URL
   - Verify installer version

2. **CMake configuration fails**
   - Check CMakeLists.txt syntax
   - Verify WinFsp path

3. **Compilation errors**
   - Check for syntax errors
   - Verify all includes are correct

**Solutions**:
- Review workflow logs
- Test locally first
- Check recent changes

### Release Failures

**Common issues**:
1. **Tag format incorrect**
   - Must be `vX.X.X`
   - Use semantic versioning

2. **Duplicate release**
   - Tag already exists
   - Delete old release first

3. **Missing permissions**
   - Check repository settings
   - Verify GITHUB_TOKEN

**Solutions**:
- Follow [RELEASE.md](RELEASE.md) guide
- Check workflow permissions
- Review release logs

## Workflow Customization

### Modifying Workflows

1. Edit `.github/workflows/*.yml`
2. Test changes in a branch
3. Create PR to verify
4. Merge when passing

### Adding New Workflows

1. Create new `.yml` file in `.github/workflows/`
2. Define triggers and jobs
3. Test thoroughly
4. Document in this file

### Best Practices

- ✅ Always test workflow changes in branches
- ✅ Use descriptive job names
- ✅ Add comments for complex steps
- ✅ Keep workflows DRY (Don't Repeat Yourself)
- ✅ Use GitHub Actions marketplace for common tasks
- ✅ Set appropriate retention periods

## Performance

### Current Metrics

Typical workflow times:
- **Build**: ~5-10 minutes
- **Release**: ~8-12 minutes
- **PR Checks**: ~3-5 minutes

### Optimization Ideas

Future improvements:
1. **Caching**
   - Cache WinFsp installer
   - Cache CMake builds
   - Cache dependencies

2. **Parallelization**
   - Run checks in parallel
   - Build multiple configs simultaneously

3. **Conditional Jobs**
   - Skip docs-only changes
   - Fast-path for minor changes

## Resources

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions)
- [WinFsp on GitHub](https://github.com/winfsp/winfsp)

## Questions?

For workflow issues:
1. Check [Actions tab](https://github.com/thefirstsenate/wamount/actions)
2. Review workflow logs
3. Consult this documentation
4. Open an issue if needed

---

Last updated: 2025-01-11
