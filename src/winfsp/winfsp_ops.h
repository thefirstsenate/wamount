#ifndef WINFSP_OPS_H
#define WINFSP_OPS_H

#include <winfsp/winfsp.h>
#include <memory>
#include <string>
#include <map>

class APFSVolume;

class WinFspAPFS {
public:
    WinFspAPFS(std::shared_ptr<APFSVolume> volume);
    ~WinFspAPFS();

    // Mount/unmount operations
    bool mount(const std::wstring& mount_point);
    void unmount();

    // WinFsp callbacks
    static NTSTATUS GetVolumeInfo(
        FSP_FILE_SYSTEM* FileSystem,
        FSP_FSCTL_VOLUME_INFO* VolumeInfo);

    static NTSTATUS GetSecurityByName(
        FSP_FILE_SYSTEM* FileSystem,
        PWSTR FileName,
        PUINT32 PFileAttributes,
        PSECURITY_DESCRIPTOR SecurityDescriptor,
        SIZE_T* PSecurityDescriptorSize);

    static NTSTATUS Open(
        FSP_FILE_SYSTEM* FileSystem,
        PWSTR FileName,
        UINT32 CreateOptions,
        UINT32 GrantedAccess,
        PVOID* PFileContext,
        FSP_FSCTL_FILE_INFO* FileInfo);

    static NTSTATUS Close(
        FSP_FILE_SYSTEM* FileSystem,
        PVOID FileContext);

    static NTSTATUS Read(
        FSP_FILE_SYSTEM* FileSystem,
        PVOID FileContext,
        PVOID Buffer,
        UINT64 Offset,
        ULONG Length,
        PULONG PBytesTransferred);

    static NTSTATUS ReadDirectory(
        FSP_FILE_SYSTEM* FileSystem,
        PVOID FileContext,
        PWSTR Pattern,
        PWSTR Marker,
        PVOID Buffer,
        ULONG Length,
        PULONG PBytesTransferred);

    static NTSTATUS GetFileInfo(
        FSP_FILE_SYSTEM* FileSystem,
        PVOID FileContext,
        FSP_FSCTL_FILE_INFO* FileInfo);

private:
    struct FileContext {
        uint64_t inode;
        bool is_directory;
        uint64_t size;
    };

    void fillFileInfo(const struct FileInfo& apfs_info, FSP_FSCTL_FILE_INFO* fsp_info);
    static WinFspAPFS* getThis(FSP_FILE_SYSTEM* FileSystem);

    std::shared_ptr<APFSVolume> volume_;
    FSP_FILE_SYSTEM* file_system_;
    std::wstring mount_point_;
};

#endif // WINFSP_OPS_H
