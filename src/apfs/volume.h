#ifndef APFS_VOLUME_H
#define APFS_VOLUME_H

#include "../../include/apfs_structures.h"
#include <memory>
#include <string>
#include <vector>

class APFSContainer;
class BTree;

struct FileInfo {
    uint64_t inode;
    std::string name;
    uint64_t size;
    uint64_t create_time;
    uint64_t mod_time;
    uint16_t mode;
    bool is_directory;
    uint64_t parent_id;
};

class APFSVolume : public std::enable_shared_from_this<APFSVolume> {
public:
    APFSVolume(std::shared_ptr<APFSContainer> container, oid_t volume_oid);
    ~APFSVolume();

    // Initialize volume
    bool init();

    // Get volume information
    const apfs_superblock_t* getSuperblock() const { return &superblock_; }
    std::string getVolumeName() const;
    uint64_t getVolumeSize() const;

    // File operations
    bool getFileInfo(uint64_t inode, FileInfo& info);
    bool getRootInode(uint64_t& inode);
    bool lookupPath(const std::string& path, FileInfo& info);
    bool readDirectory(uint64_t inode, std::vector<FileInfo>& entries);
    bool readFile(uint64_t inode, uint64_t offset, void* buffer, size_t size, size_t& bytes_read);

    // Container access
    std::shared_ptr<APFSContainer> getContainer() { return container_.lock(); }

private:
    bool readSuperblock();
    bool readObjectMap();
    bool initRootTree();

    std::weak_ptr<APFSContainer> container_;
    oid_t volume_oid_;
    apfs_superblock_t superblock_;
    omap_phys_t volume_omap_;
    std::shared_ptr<BTree> root_tree_;
};

#endif // APFS_VOLUME_H
