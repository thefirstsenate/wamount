#ifndef APFS_BTREE_H
#define APFS_BTREE_H

#include "../../include/apfs_structures.h"
#include <memory>
#include <vector>
#include <functional>

class APFSVolume;

struct BTreeEntry {
    std::vector<uint8_t> key;
    std::vector<uint8_t> value;
};

class BTree {
public:
    BTree(std::shared_ptr<APFSVolume> volume, oid_t root_oid);
    ~BTree();

    // Initialize tree
    bool init();

    // Search operations
    bool lookup(const void* key, size_t key_size, std::vector<uint8_t>& value);
    bool lookupRange(const void* key_min, size_t key_min_size,
                     const void* key_max, size_t key_max_size,
                     std::vector<BTreeEntry>& entries);

    // Enumeration
    bool enumerate(std::function<bool(const BTreeEntry&)> callback);

private:
    bool readNode(oid_t node_oid, std::vector<uint8_t>& node_data);
    bool searchNode(const std::vector<uint8_t>& node_data, const void* key, size_t key_size,
                    std::vector<uint8_t>& value, bool& found);
    int compareKeys(const void* key1, size_t size1, const void* key2, size_t size2);
    bool getKeyValue(const btree_node_phys_t* node, uint32_t index,
                     const uint8_t*& key, uint16_t& key_len,
                     const uint8_t*& val, uint16_t& val_len);

    std::weak_ptr<APFSVolume> volume_;
    oid_t root_oid_;
    btree_info_t tree_info_;
};

#endif // APFS_BTREE_H
