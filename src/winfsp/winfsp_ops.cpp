// Suppress codecvt deprecation warning in C++17
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "winfsp_ops.h"
#include "../apfs/volume.h"
#include "../utils/logging.h"
#include <string>
#include <codecvt>
#include <locale>

WinFspAPFS::WinFspAPFS(std::shared_ptr<APFSVolume> volume)
    : volume_(volume), file_system_(nullptr) {
}

WinFspAPFS::~WinFspAPFS() {
    unmount();
}

bool WinFspAPFS::mount(const std::wstring& mount_point) {
    if (file_system_ != nullptr) {
        Logger::error("Already mounted");
        return false;
    }

    mount_point_ = mount_point;

    // Create WinFsp file system
    FSP_FSCTL_VOLUME_PARAMS VolumeParams = {0};
    VolumeParams.Version = sizeof(FSP_FSCTL_VOLUME_PARAMS);
    VolumeParams.SectorSize = 4096;
    VolumeParams.SectorsPerAllocationUnit = 1;
    VolumeParams.VolumeCreationTime = 0;
    VolumeParams.VolumeSerialNumber = 0;
    VolumeParams.FileInfoTimeout = 1000;
    VolumeParams.CaseSensitiveSearch = 1;
    VolumeParams.CasePreservedNames = 1;
    VolumeParams.UnicodeOnDisk = 1;
    VolumeParams.PersistentAcls = 0;
    VolumeParams.ReparsePoints = 0;
    VolumeParams.ReparsePointsAccessCheck = 0;
    VolumeParams.NamedStreams = 0;
    VolumeParams.ReadOnlyVolume = 1;  // Read-only for now

    std::string vol_name = volume_->getVolumeName();
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring vol_name_wide = converter.from_bytes(vol_name);

    wcscpy_s(VolumeParams.FileSystemName, L"APFS");
    wcsncpy_s(VolumeParams.Prefix, vol_name_wide.c_str(), sizeof(VolumeParams.Prefix) / sizeof(WCHAR) - 1);

    // Create file system
    FSP_FILE_SYSTEM_INTERFACE Interface = {0};
    Interface.GetVolumeInfo = GetVolumeInfo;
    Interface.GetSecurityByName = GetSecurityByName;
    Interface.Open = Open;
    Interface.Close = Close;
    Interface.Read = Read;
    Interface.ReadDirectory = ReadDirectory;
    Interface.GetFileInfo = GetFileInfo;

    NTSTATUS Result = FspFileSystemCreate(
        L"",
        &VolumeParams,
        &Interface,
        &file_system_);

    if (!NT_SUCCESS(Result)) {
        Logger::error("Failed to create file system: 0x" + std::to_string(Result));
        return false;
    }

    file_system_->UserContext = this;

    // Set mount point
    Result = FspFileSystemSetMountPoint(file_system_, mount_point.c_str());
    if (!NT_SUCCESS(Result)) {
        Logger::error("Failed to set mount point: 0x" + std::to_string(Result));
        FspFileSystemDelete(file_system_);
        file_system_ = nullptr;
        return false;
    }

    // Start dispatcher
    Result = FspFileSystemStartDispatcher(file_system_, 0);
    if (!NT_SUCCESS(Result)) {
        Logger::error("Failed to start dispatcher: 0x" + std::to_string(Result));
        FspFileSystemDelete(file_system_);
        file_system_ = nullptr;
        return false;
    }

    Logger::info("Volume mounted at: " + std::string(mount_point.begin(), mount_point.end()));
    return true;
}

void WinFspAPFS::unmount() {
    if (file_system_ != nullptr) {
        FspFileSystemStopDispatcher(file_system_);
        FspFileSystemDelete(file_system_);
        file_system_ = nullptr;
        Logger::info("Volume unmounted");
    }
}

WinFspAPFS* WinFspAPFS::getThis(FSP_FILE_SYSTEM* FileSystem) {
    return static_cast<WinFspAPFS*>(FileSystem->UserContext);
}

NTSTATUS WinFspAPFS::GetVolumeInfo(
    FSP_FILE_SYSTEM* FileSystem,
    FSP_FSCTL_VOLUME_INFO* VolumeInfo) {

    WinFspAPFS* self = getThis(FileSystem);

    VolumeInfo->TotalSize = self->volume_->getVolumeSize();
    VolumeInfo->FreeSize = 0;  // Read-only, report 0 free
    VolumeInfo->VolumeLabelLength = 0;

    return STATUS_SUCCESS;
}

NTSTATUS WinFspAPFS::GetSecurityByName(
    FSP_FILE_SYSTEM* FileSystem,
    PWSTR FileName,
    PUINT32 PFileAttributes,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    SIZE_T* PSecurityDescriptorSize) {

    WinFspAPFS* self = getThis(FileSystem);

    // Convert wide string to UTF-8
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string path = converter.to_bytes(FileName);

    FileInfo info;
    if (!self->volume_->lookupPath(path, info)) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    if (PFileAttributes != nullptr) {
        *PFileAttributes = FILE_ATTRIBUTE_READONLY;
        if (info.is_directory) {
            *PFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        }
    }

    // Simplified security descriptor (everyone read-only)
    if (PSecurityDescriptorSize != nullptr) {
        *PSecurityDescriptorSize = 0;
    }

    return STATUS_SUCCESS;
}

NTSTATUS WinFspAPFS::Open(
    FSP_FILE_SYSTEM* FileSystem,
    PWSTR FileName,
    UINT32 CreateOptions,
    UINT32 GrantedAccess,
    PVOID* PFileContext,
    FSP_FSCTL_FILE_INFO* FileInfo) {

    WinFspAPFS* self = getThis(FileSystem);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string path = converter.to_bytes(FileName);

    struct FileInfo apfs_info;
    if (!self->volume_->lookupPath(path, apfs_info)) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    FileContext* context = new FileContext();
    context->inode = apfs_info.inode;
    context->is_directory = apfs_info.is_directory;
    context->size = apfs_info.size;

    *PFileContext = context;
    self->fillFileInfo(apfs_info, FileInfo);

    return STATUS_SUCCESS;
}

NTSTATUS WinFspAPFS::Close(
    FSP_FILE_SYSTEM* FileSystem,
    PVOID FileContext) {

    FileContext* context = static_cast<FileContext*>(FileContext);
    delete context;

    return STATUS_SUCCESS;
}

NTSTATUS WinFspAPFS::Read(
    FSP_FILE_SYSTEM* FileSystem,
    PVOID FileContext,
    PVOID Buffer,
    UINT64 Offset,
    ULONG Length,
    PULONG PBytesTransferred) {

    WinFspAPFS* self = getThis(FileSystem);
    FileContext* context = static_cast<FileContext*>(FileContext);

    if (context->is_directory) {
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    size_t bytes_read = 0;
    if (!self->volume_->readFile(context->inode, Offset, Buffer, Length, bytes_read)) {
        return STATUS_UNSUCCESSFUL;
    }

    *PBytesTransferred = static_cast<ULONG>(bytes_read);
    return STATUS_SUCCESS;
}

NTSTATUS WinFspAPFS::ReadDirectory(
    FSP_FILE_SYSTEM* FileSystem,
    PVOID FileContext,
    PWSTR Pattern,
    PWSTR Marker,
    PVOID Buffer,
    ULONG Length,
    PULONG PBytesTransferred) {

    WinFspAPFS* self = getThis(FileSystem);
    FileContext* context = static_cast<FileContext*>(FileContext);

    if (!context->is_directory) {
        return STATUS_NOT_A_DIRECTORY;
    }

    std::vector<struct FileInfo> entries;
    if (!self->volume_->readDirectory(context->inode, entries)) {
        return STATUS_UNSUCCESSFUL;
    }

    // Convert entries to WinFsp format
    NTSTATUS Result = STATUS_SUCCESS;
    ULONG BytesTransferred = 0;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    for (const auto& entry : entries) {
        std::wstring name = converter.from_bytes(entry.name);

        FSP_FSCTL_DIR_INFO DirInfo = {0};
        self->fillFileInfo(entry, &DirInfo.FileInfo);

        BOOLEAN Added = FspFileSystemAddDirInfo(
            &DirInfo,
            Buffer,
            Length,
            &BytesTransferred);

        if (!Added) {
            Result = STATUS_SUCCESS;  // Buffer full, but not an error
            break;
        }
    }

    *PBytesTransferred = BytesTransferred;
    return Result;
}

NTSTATUS WinFspAPFS::GetFileInfo(
    FSP_FILE_SYSTEM* FileSystem,
    PVOID FileContext,
    FSP_FSCTL_FILE_INFO* FileInfo) {

    WinFspAPFS* self = getThis(FileSystem);
    FileContext* context = static_cast<FileContext*>(FileContext);

    struct FileInfo apfs_info;
    if (!self->volume_->getFileInfo(context->inode, apfs_info)) {
        return STATUS_UNSUCCESSFUL;
    }

    self->fillFileInfo(apfs_info, FileInfo);
    return STATUS_SUCCESS;
}

void WinFspAPFS::fillFileInfo(const struct FileInfo& apfs_info, FSP_FSCTL_FILE_INFO* fsp_info) {
    memset(fsp_info, 0, sizeof(FSP_FSCTL_FILE_INFO));

    fsp_info->FileAttributes = FILE_ATTRIBUTE_READONLY;
    if (apfs_info.is_directory) {
        fsp_info->FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
    }

    fsp_info->FileSize = apfs_info.size;
    fsp_info->AllocationSize = (apfs_info.size + 4095) & ~4095ULL;  // Round up to block size

    // Convert APFS timestamps (nanoseconds since epoch) to Windows FILETIME
    // APFS uses Unix epoch (1970), Windows uses 1601
    const uint64_t WINDOWS_TICK = 10000000;
    const uint64_t SEC_TO_UNIX_EPOCH = 11644473600LL;

    fsp_info->CreationTime = (apfs_info.create_time / 1000000000) * WINDOWS_TICK +
                              SEC_TO_UNIX_EPOCH * WINDOWS_TICK;
    fsp_info->LastAccessTime = fsp_info->CreationTime;
    fsp_info->LastWriteTime = (apfs_info.mod_time / 1000000000) * WINDOWS_TICK +
                               SEC_TO_UNIX_EPOCH * WINDOWS_TICK;
    fsp_info->ChangeTime = fsp_info->LastWriteTime;

    fsp_info->IndexNumber = apfs_info.inode;
}
