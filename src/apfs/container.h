#ifndef APFS_CONTAINER_H
#define APFS_CONTAINER_H

#include "../../include/apfs_structures.h"
#include "../utils/disk_reader.h"
#include <memory>
#include <vector>

class APFSVolume;

class APFSContainer : public std::enable_shared_from_this<APFSContainer> {
public:
    APFSContainer(std::shared_ptr<DiskReader> disk);
    ~APFSContainer();

    // Initialize container by reading superblock
    bool init();

    // Get container information
    const nx_superblock_t* getSuperblock() const { return &superblock_; }
    uint32_t getBlockSize() const { return block_size_; }
    uint64_t getBlockCount() const { return block_count_; }

    // Volume management
    size_t getVolumeCount() const;
    std::shared_ptr<APFSVolume> getVolume(size_t index);

    // Read physical block
    bool readBlock(uint64_t block_num, void* buffer, size_t size);

    // Object map operations
    bool getObjectPhysicalAddress(oid_t oid, xid_t xid, paddr_t& paddr);

private:
    bool readSuperblock();
    bool readObjectMap();

    std::shared_ptr<DiskReader> disk_;
    nx_superblock_t superblock_;
    uint32_t block_size_;
    uint64_t block_count_;
    omap_phys_t container_omap_;
    std::vector<std::shared_ptr<APFSVolume>> volumes_;
};

#endif // APFS_CONTAINER_H
