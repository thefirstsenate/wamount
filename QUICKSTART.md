# WAMount Quick Start Guide

Get up and running with WAMount in 5 minutes!

## Step 1: Install Prerequisites

### Install WinFsp

1. Download WinFsp from https://winfsp.dev/rel/
2. Run the installer (e.g., `winfsp-1.12.22301.msi`)
3. Follow installation wizard
4. Restart if prompted

### Verify WinFsp Installation

Open Command Prompt and run:
```cmd
sc query WinFsp.Launcher
```

You should see "RUNNING" in the output.

## Step 2: Get WAMount

### Option A: Download Binary (Recommended)
- Download `wamount.exe` from releases
- Place in a convenient location (e.g., `C:\Tools\`)

### Option B: Build from Source
See [BUILD.md](BUILD.md) for detailed build instructions.

## Step 3: Find Your APFS Disk

### Using Disk Management

1. Press `Win + X` and select "Disk Management"
2. Look for your APFS disk (may show as "Unknown" or "Unallocated")
3. Note the disk number (e.g., "Disk 1")

### Using diskpart

```cmd
diskpart
DISKPART> list disk
DISKPART> exit
```

Note the disk number of your APFS disk.

## Step 4: Mount APFS Volume

### Open Administrator Command Prompt

1. Press `Win + X`
2. Select "Command Prompt (Admin)" or "PowerShell (Admin)"

### Run WAMount

```cmd
# Basic usage (replace 1 with your disk number)
wamount.exe -m X: \\.\PhysicalDrive1

# With debug output
wamount.exe -d -m X: \\.\PhysicalDrive1

# Specify volume index (if multiple volumes)
wamount.exe -m X: -v 0 \\.\PhysicalDrive1
```

### Success!

You should see:
```
WAMount - Windows Apple Mount v1.0
==================================

Opened device: \\.\PhysicalDrive1 (Size: 500000000000 bytes)
Container superblock:
  Block size: 4096
  Block count: 122070312
  Volumes found: 1

Volume info:
  Name: Macintosh HD
  Files: 45678
  Directories: 12345

Volume mounted successfully at X:
Press Ctrl+C to unmount and exit...
```

### Access Your Files

1. Open File Explorer
2. Navigate to the drive letter (e.g., `X:`)
3. Browse your APFS files!

## Step 5: Unmount

Press `Ctrl+C` in the WAMount window to unmount cleanly.

## Common Issues and Solutions

### "Failed to open device"

**Solution:** Run as Administrator
- Right-click Command Prompt
- Select "Run as administrator"

### "Failed to initialize APFS container"

**Possible causes:**
1. Wrong disk number
   - Verify with Disk Management
   - Try other PhysicalDrive numbers

2. Encrypted volume
   - WAMount v1.0 doesn't support encrypted volumes
   - Decrypt the volume on macOS first

3. Not an APFS partition
   - Verify it's actually APFS, not HFS+ or NTFS

### "WinFsp not found" or "winfsp-x64.dll missing"

**Solution:** Install/reinstall WinFsp
- Download from https://winfsp.dev/
- Make sure to install, not just extract

### "Drive letter already in use"

**Solution:** Choose different drive letter
```cmd
wamount.exe -m Y: \\.\PhysicalDrive1
```

### Volume mounts but shows empty

**Possible causes:**
1. Wrong volume index
   - Try different volume index: `-v 0`, `-v 1`, etc.

2. Corrupted filesystem
   - Check disk with macOS Disk Utility first

## Usage Examples

### Mount first volume as X:
```cmd
wamount.exe -m X: \\.\PhysicalDrive1
```

### Mount second volume as Y:
```cmd
wamount.exe -m Y: -v 1 \\.\PhysicalDrive1
```

### Mount with debug logging:
```cmd
wamount.exe -d -m X: \\.\PhysicalDrive1
```

### Mount disk image file:
```cmd
# If you have a disk image (e.g., from dd)
wamount.exe -m X: C:\path\to\image.img
```

## Tips and Best Practices

### Always Unmount Properly
- Use Ctrl+C to unmount
- Don't just close the window
- Wait for "Volume unmounted" message

### Read-Only Access
- WAMount v1.0 is read-only
- Safe for your data
- Cannot modify files

### Performance
- First access may be slow (caching)
- Subsequent access is faster
- Large files may take time

### Safety
- Always backup important data
- Test with non-critical volumes first
- Use on known-good APFS volumes

## What You Can Do

✅ Read files and folders
✅ Copy files from APFS to Windows
✅ View file properties
✅ Search files
✅ Open files with Windows applications

## What You Cannot Do (Yet)

❌ Write or modify files
❌ Delete files
❌ Create new files or folders
❌ Access encrypted volumes
❌ Decompress compressed files

## Next Steps

- Read [README.md](README.md) for detailed information
- See [BUILD.md](BUILD.md) if you want to build from source
- Check [CHANGELOG.md](CHANGELOG.md) for version history

## Getting Help

If you encounter issues:

1. Enable debug mode: `-d` flag
2. Check error messages carefully
3. Verify prerequisites are installed
4. Try different disk/volume numbers
5. Check GitHub issues for similar problems
6. Create new issue with:
   - Windows version
   - WinFsp version
   - Exact error message
   - Debug output

## Safety Notice

⚠️ **Important Safety Information**

- WAMount is experimental software
- Always backup important data before use
- Test with non-critical data first
- Read-only mode is safe but test carefully
- Encrypted volumes are not supported

Happy mounting! 🎉
