#include "container.h"
#include "volume.h"
#include "../utils/logging.h"
#include <cstring>

APFSContainer::APFSContainer(std::shared_ptr<DiskReader> disk)
    : disk_(disk), block_size_(0), block_count_(0) {
    memset(&superblock_, 0, sizeof(superblock_));
    memset(&container_omap_, 0, sizeof(container_omap_));
}

APFSContainer::~APFSContainer() {
}

bool APFSContainer::init() {
    Logger::info("Initializing APFS container...");

    if (!readSuperblock()) {
        Logger::error("Failed to read container superblock");
        return false;
    }

    if (!readObjectMap()) {
        Logger::error("Failed to read container object map");
        return false;
    }

    Logger::info("APFS container initialized successfully");
    return true;
}

bool APFSContainer::readSuperblock() {
    // Read block 0 which contains the container superblock
    std::vector<uint8_t> buffer(sizeof(nx_superblock_t));

    if (!disk_->readBlock(0, buffer.data(), APFS_BLOCK_SIZE)) {
        Logger::error("Failed to read superblock from disk");
        return false;
    }

    memcpy(&superblock_, buffer.data(), sizeof(nx_superblock_t));

    // Validate magic number
    if (superblock_.nx_magic != NX_MAGIC) {
        Logger::error("Invalid NX superblock magic: 0x" +
                      std::to_string(superblock_.nx_magic));
        return false;
    }

    block_size_ = superblock_.nx_block_size;
    block_count_ = superblock_.nx_block_count;

    Logger::info("Container superblock:");
    Logger::info("  Block size: " + std::to_string(block_size_));
    Logger::info("  Block count: " + std::to_string(block_count_));
    Logger::info("  Max filesystems: " + std::to_string(superblock_.nx_max_file_systems));

    return true;
}

bool APFSContainer::readObjectMap() {
    if (superblock_.nx_omap_oid == 0) {
        Logger::error("Container object map OID is 0");
        return false;
    }

    std::vector<uint8_t> buffer(block_size_);

    // For physical objects, the OID is the physical address
    if (!readBlock(superblock_.nx_omap_oid, buffer.data(), block_size_)) {
        Logger::error("Failed to read container object map");
        return false;
    }

    memcpy(&container_omap_, buffer.data(), sizeof(omap_phys_t));

    // Validate object type
    uint32_t obj_type = container_omap_.om_o.o_type & 0xffff;
    if (obj_type != OBJECT_TYPE_OMAP) {
        Logger::warning("Object map has unexpected type: 0x" +
                        std::to_string(obj_type));
    }

    Logger::info("Container object map:");
    Logger::info("  Tree OID: " + std::to_string(container_omap_.om_tree_oid));
    Logger::info("  Snapshot count: " + std::to_string(container_omap_.om_snap_count));

    return true;
}

bool APFSContainer::readBlock(uint64_t block_num, void* buffer, size_t size) {
    return disk_->readBlock(block_num, buffer, size);
}

size_t APFSContainer::getVolumeCount() const {
    size_t count = 0;
    for (size_t i = 0; i < NX_MAX_FILE_SYSTEMS; i++) {
        if (superblock_.nx_fs_oid[i] != 0) {
            count++;
        }
    }
    return count;
}

std::shared_ptr<APFSVolume> APFSContainer::getVolume(size_t index) {
    if (index >= NX_MAX_FILE_SYSTEMS || superblock_.nx_fs_oid[index] == 0) {
        return nullptr;
    }

    // Check if already loaded
    if (index < volumes_.size() && volumes_[index]) {
        return volumes_[index];
    }

    // Resize vector if needed
    if (volumes_.size() <= index) {
        volumes_.resize(index + 1);
    }

    // Create new volume
    auto volume = std::make_shared<APFSVolume>(shared_from_this(), superblock_.nx_fs_oid[index]);
    if (!volume->init()) {
        Logger::error("Failed to initialize volume " + std::to_string(index));
        return nullptr;
    }

    volumes_[index] = volume;
    return volume;
}

bool APFSContainer::getObjectPhysicalAddress(oid_t oid, xid_t xid, paddr_t& paddr) {
    // This is a simplified implementation
    // In a full implementation, we would traverse the object map B-tree
    // For now, we'll use the OID directly as physical address for physical objects
    paddr = oid;
    return true;
}
