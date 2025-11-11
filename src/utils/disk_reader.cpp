#include "disk_reader.h"
#include <iostream>

DiskReader::DiskReader()
    : handle_(INVALID_HANDLE_VALUE), disk_size_(0) {
}

DiskReader::~DiskReader() {
    close();
}

bool DiskReader::open(const std::string& device_path) {
    if (handle_ != INVALID_HANDLE_VALUE) {
        close();
    }

    device_path_ = device_path;

    // Open the device with read access
    handle_ = CreateFileA(
        device_path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (handle_ == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open device: " << device_path
                  << " (Error: " << GetLastError() << ")" << std::endl;
        return false;
    }

    // Get disk size
    DISK_GEOMETRY_EX geometry;
    DWORD bytes_returned;

    if (DeviceIoControl(
            handle_,
            IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
            nullptr,
            0,
            &geometry,
            sizeof(geometry),
            &bytes_returned,
            nullptr)) {
        disk_size_ = geometry.DiskSize.QuadPart;
    } else {
        // Try to get file size instead
        LARGE_INTEGER size;
        if (GetFileSizeEx(handle_, &size)) {
            disk_size_ = size.QuadPart;
        } else {
            std::cerr << "Failed to get disk size" << std::endl;
            close();
            return false;
        }
    }

    std::cout << "Opened device: " << device_path
              << " (Size: " << disk_size_ << " bytes)" << std::endl;

    return true;
}

void DiskReader::close() {
    if (handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
        disk_size_ = 0;
    }
}

bool DiskReader::readBlock(uint64_t block_num, void* buffer, size_t block_size) {
    return readBlocks(block_num, 1, buffer, block_size);
}

bool DiskReader::readBlocks(uint64_t start_block, uint64_t count, void* buffer, size_t block_size) {
    uint64_t offset = start_block * block_size;
    size_t total_size = count * block_size;
    return readBytes(offset, buffer, total_size);
}

bool DiskReader::readBytes(uint64_t offset, void* buffer, size_t size) {
    if (handle_ == INVALID_HANDLE_VALUE) {
        std::cerr << "Disk not open" << std::endl;
        return false;
    }

    if (offset + size > disk_size_) {
        std::cerr << "Read beyond disk size" << std::endl;
        return false;
    }

    // Set file pointer
    LARGE_INTEGER li_offset;
    li_offset.QuadPart = offset;

    if (SetFilePointerEx(handle_, li_offset, nullptr, FILE_BEGIN) == 0) {
        std::cerr << "Failed to seek to offset " << offset << std::endl;
        return false;
    }

    // Read data
    DWORD bytes_read = 0;
    if (ReadFile(handle_, buffer, static_cast<DWORD>(size), &bytes_read, nullptr) == 0) {
        std::cerr << "Failed to read data (Error: " << GetLastError() << ")" << std::endl;
        return false;
    }

    if (bytes_read != size) {
        std::cerr << "Incomplete read: " << bytes_read << " of " << size << " bytes" << std::endl;
        return false;
    }

    return true;
}
