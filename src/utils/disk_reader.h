#ifndef DISK_READER_H
#define DISK_READER_H

#include <windows.h>
#include <string>
#include <vector>
#include <memory>

class DiskReader {
public:
    DiskReader();
    ~DiskReader();

    // Open a disk or partition
    bool open(const std::string& device_path);

    // Close the disk
    void close();

    // Read block(s) from disk
    bool readBlock(uint64_t block_num, void* buffer, size_t block_size = 4096);
    bool readBlocks(uint64_t start_block, uint64_t count, void* buffer, size_t block_size = 4096);

    // Read arbitrary bytes
    bool readBytes(uint64_t offset, void* buffer, size_t size);

    // Get disk size
    uint64_t getDiskSize() const { return disk_size_; }

    // Check if disk is open
    bool isOpen() const { return handle_ != INVALID_HANDLE_VALUE; }

private:
    HANDLE handle_;
    uint64_t disk_size_;
    std::string device_path_;
};

#endif // DISK_READER_H
