#include "volume.h"
#include "container.h"
#include "btree.h"
#include "../utils/logging.h"
#include <cstring>
#include <sstream>
#include <string>
#include <algorithm>

APFSVolume::APFSVolume(std::shared_ptr<APFSContainer> container, oid_t volume_oid)
    : container_(container), volume_oid_(volume_oid) {
    memset(&superblock_, 0, sizeof(superblock_));
    memset(&volume_omap_, 0, sizeof(volume_omap_));
}

APFSVolume::~APFSVolume() {
}

bool APFSVolume::init() {
    Logger::info("Initializing APFS volume...");

    if (!readSuperblock()) {
        Logger::error("Failed to read volume superblock");
        return false;
    }

    if (!readObjectMap()) {
        Logger::error("Failed to read volume object map");
        return false;
    }

    if (!initRootTree()) {
        Logger::error("Failed to initialize root tree");
        return false;
    }

    Logger::info("APFS volume '" + getVolumeName() + "' initialized successfully");
    return true;
}

bool APFSVolume::readSuperblock() {
    auto container = container_.lock();
    if (!container) {
        Logger::error("Container is no longer valid");
        return false;
    }

    std::vector<uint8_t> buffer(container->getBlockSize());

    // Read volume superblock (OID is physical address)
    if (!container->readBlock(volume_oid_, buffer.data(), container->getBlockSize())) {
        Logger::error("Failed to read volume superblock");
        return false;
    }

    memcpy(&superblock_, buffer.data(), sizeof(superblock_));

    // Validate magic number
    if (superblock_.apfs_magic != APFS_MAGIC) {
        Logger::error("Invalid APFS volume magic: 0x" +
                      std::to_string(superblock_.apfs_magic));
        return false;
    }

    Logger::info("Volume superblock:");
    Logger::info("  Name: " + getVolumeName());
    Logger::info("  Files: " + std::to_string(superblock_.apfs_num_files));
    Logger::info("  Directories: " + std::to_string(superblock_.apfs_num_directories));

    return true;
}

bool APFSVolume::readObjectMap() {
    if (superblock_.apfs_omap_oid == 0) {
        Logger::error("Volume object map OID is 0");
        return false;
    }

    auto container = container_.lock();
    if (!container) {
        return false;
    }

    std::vector<uint8_t> buffer(container->getBlockSize());

    if (!container->readBlock(superblock_.apfs_omap_oid, buffer.data(), container->getBlockSize())) {
        Logger::error("Failed to read volume object map");
        return false;
    }

    memcpy(&volume_omap_, buffer.data(), sizeof(omap_phys_t));

    Logger::info("Volume object map:");
    Logger::info("  Tree OID: " + std::to_string(volume_omap_.om_tree_oid));

    return true;
}

bool APFSVolume::initRootTree() {
    if (superblock_.apfs_root_tree_oid == 0) {
        Logger::error("Root tree OID is 0");
        return false;
    }

    root_tree_ = std::make_shared<BTree>(shared_from_this(), superblock_.apfs_root_tree_oid);
    if (!root_tree_->init()) {
        Logger::error("Failed to initialize root tree");
        return false;
    }

    return true;
}

std::string APFSVolume::getVolumeName() const {
    std::string name(reinterpret_cast<const char*>(superblock_.apfs_volname));
    if (name.empty()) {
        name = "Untitled";
    }
    return name;
}

uint64_t APFSVolume::getVolumeSize() const {
    return superblock_.apfs_total_blocks_alloced *
           container_.lock()->getBlockSize();
}

bool APFSVolume::getRootInode(uint64_t& inode) {
    // Root directory is always inode 2 in APFS
    inode = 2;
    return true;
}

bool APFSVolume::getFileInfo(uint64_t inode, FileInfo& info) {
    // Build the key for inode lookup
    j_key_t key;
    key.obj_id_and_type = (inode & OBJ_ID_MASK) |
                          (static_cast<uint64_t>(APFS_TYPE_INODE) << OBJ_TYPE_SHIFT);

    std::vector<uint8_t> value;
    if (!root_tree_->lookup(&key, sizeof(key), value)) {
        Logger::debug("Inode " + std::to_string(inode) + " not found");
        return false;
    }

    if (value.size() < sizeof(j_inode_val_t)) {
        Logger::error("Inode value too small");
        return false;
    }

    const j_inode_val_t* inode_val = reinterpret_cast<const j_inode_val_t*>(value.data());

    info.inode = inode;
    info.parent_id = inode_val->parent_id;
    info.create_time = inode_val->create_time;
    info.mod_time = inode_val->mod_time;
    info.mode = inode_val->mode;
    info.is_directory = (inode_val->mode & S_IFMT) == S_IFDIR;
    info.size = 0;

    // Parse extended fields to get size
    const uint8_t* xfield_ptr = inode_val->xfields;
    const uint8_t* xfield_end = value.data() + value.size();

    while (xfield_ptr + sizeof(x_field_t) <= xfield_end) {
        const x_field_t* xfield = reinterpret_cast<const x_field_t*>(xfield_ptr);

        if (xfield->x_type == INO_EXT_TYPE_DSTREAM) {
            if (xfield_ptr + sizeof(x_field_t) + sizeof(j_dstream_t) <= xfield_end) {
                const j_dstream_t* dstream = reinterpret_cast<const j_dstream_t*>(xfield_ptr + sizeof(x_field_t));
                info.size = dstream->size;
            }
            break;
        }

        xfield_ptr += sizeof(x_field_t) + xfield->x_size;
    }

    return true;
}

bool APFSVolume::readDirectory(uint64_t inode, std::vector<FileInfo>& entries) {
    entries.clear();

    // Build key range for directory entries
    j_key_t key_min, key_max;
    key_min.obj_id_and_type = (inode & OBJ_ID_MASK) |
                              (static_cast<uint64_t>(APFS_TYPE_DIR_REC) << OBJ_TYPE_SHIFT);
    key_max.obj_id_and_type = ((inode + 1) & OBJ_ID_MASK) |
                              (static_cast<uint64_t>(APFS_TYPE_DIR_REC) << OBJ_TYPE_SHIFT);

    std::vector<BTreeEntry> btree_entries;
    if (!root_tree_->lookupRange(&key_min, sizeof(key_min), &key_max, sizeof(key_max), btree_entries)) {
        Logger::debug("Failed to enumerate directory entries");
        return false;
    }

    for (const auto& entry : btree_entries) {
        if (entry.key.size() < sizeof(j_drec_key_t)) {
            continue;
        }

        const j_drec_key_t* drec_key = reinterpret_cast<const j_drec_key_t*>(entry.key.data());

        if (entry.value.size() < sizeof(j_drec_val_t)) {
            continue;
        }

        const j_drec_val_t* drec_val = reinterpret_cast<const j_drec_val_t*>(entry.value.data());

        FileInfo child_info;
        if (getFileInfo(drec_val->file_id, child_info)) {
            child_info.name = std::string(reinterpret_cast<const char*>(drec_key->name), drec_key->name_len);
            entries.push_back(child_info);
        }
    }

    return true;
}

bool APFSVolume::lookupPath(const std::string& path, FileInfo& info) {
    // Start from root
    uint64_t current_inode;
    if (!getRootInode(current_inode)) {
        return false;
    }

    // Handle root path
    if (path == "/" || path.empty()) {
        return getFileInfo(current_inode, info);
    }

    // Split path into components
    std::vector<std::string> components;
    std::stringstream ss(path);
    std::string component;

    while (std::getline(ss, component, '/')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }

    // Traverse path
    for (const auto& comp : components) {
        std::vector<FileInfo> entries;
        if (!readDirectory(current_inode, entries)) {
            return false;
        }

        bool found = false;
        for (const auto& entry : entries) {
            if (entry.name == comp) {
                current_inode = entry.inode;
                info = entry;
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

bool APFSVolume::readFile(uint64_t inode, uint64_t offset, void* buffer, size_t size, size_t& bytes_read) {
    bytes_read = 0;

    // Get file info to validate
    FileInfo info;
    if (!getFileInfo(inode, info)) {
        return false;
    }

    if (info.is_directory) {
        Logger::error("Cannot read from directory");
        return false;
    }

    if (offset >= info.size) {
        return true;  // EOF
    }

    // Adjust size if reading beyond EOF
    size_t read_size = std::min(size, static_cast<size_t>(info.size - offset));

    // Build key for file extent lookup
    j_file_extent_key_t ext_key;
    ext_key.hdr.obj_id_and_type = (inode & OBJ_ID_MASK) |
                                   (static_cast<uint64_t>(APFS_TYPE_FILE_EXTENT) << OBJ_TYPE_SHIFT);
    ext_key.logical_addr = offset;

    std::vector<uint8_t> value;
    if (!root_tree_->lookup(&ext_key, sizeof(ext_key), value)) {
        Logger::debug("File extent not found for offset " + std::to_string(offset));
        return false;
    }

    if (value.size() < sizeof(j_file_extent_val_t)) {
        Logger::error("File extent value too small");
        return false;
    }

    const j_file_extent_val_t* ext_val = reinterpret_cast<const j_file_extent_val_t*>(value.data());

    auto container = container_.lock();
    if (!container) {
        return false;
    }

    // Read from physical blocks
    uint64_t phys_block = ext_val->phys_block_num;
    uint64_t block_offset = offset % container->getBlockSize();
    uint64_t blocks_to_read = (read_size + block_offset + container->getBlockSize() - 1) / container->getBlockSize();

    std::vector<uint8_t> temp_buffer(blocks_to_read * container->getBlockSize());

    if (!container->readBlock(phys_block, temp_buffer.data(), temp_buffer.size())) {
        Logger::error("Failed to read file data blocks");
        return false;
    }

    memcpy(buffer, temp_buffer.data() + block_offset, read_size);
    bytes_read = read_size;

    return true;
}
