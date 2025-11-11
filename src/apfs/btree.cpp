#include "btree.h"
#include "volume.h"
#include "container.h"
#include "../utils/logging.h"
#include <cstring>
#include <string>
#include <algorithm>

BTree::BTree(std::shared_ptr<APFSVolume> volume, oid_t root_oid)
    : volume_(volume), root_oid_(root_oid) {
    memset(&tree_info_, 0, sizeof(tree_info_));
}

BTree::~BTree() {
}

bool BTree::init() {
    if (root_oid_ == 0) {
        Logger::error("B-tree root OID is 0");
        return false;
    }

    Logger::debug("Initialized B-tree with root OID: " + std::to_string(root_oid_));
    return true;
}

bool BTree::readNode(oid_t node_oid, std::vector<uint8_t>& node_data) {
    auto vol = volume_.lock();
    if (!vol) {
        Logger::error("Volume is no longer valid");
        return false;
    }

    auto container = vol->getContainer();
    if (!container) {
        Logger::error("Container is no longer valid");
        return false;
    }

    uint32_t block_size = container->getBlockSize();
    node_data.resize(block_size);

    // Use OID as physical address (simplified)
    if (!container->readBlock(node_oid, node_data.data(), block_size)) {
        Logger::error("Failed to read B-tree node at OID: " + std::to_string(node_oid));
        return false;
    }

    return true;
}

bool BTree::lookup(const void* key, size_t key_size, std::vector<uint8_t>& value) {
    std::vector<uint8_t> node_data;
    oid_t current_oid = root_oid_;
    bool found = false;

    // Traverse tree from root to leaf
    while (current_oid != 0 && !found) {
        if (!readNode(current_oid, node_data)) {
            return false;
        }

        if (!searchNode(node_data, key, key_size, value, found)) {
            return false;
        }

        // If not found and we have more levels, continue traversal
        // This is simplified - full implementation would follow child pointers
        break;
    }

    return found;
}

bool BTree::searchNode(const std::vector<uint8_t>& node_data, const void* key, size_t key_size,
                       std::vector<uint8_t>& value, bool& found) {
    const btree_node_phys_t* node = reinterpret_cast<const btree_node_phys_t*>(node_data.data());

    found = false;

    // Validate node
    uint32_t obj_type = node->btn_o.o_type & 0xffff;
    if (obj_type != OBJECT_TYPE_BTREE_NODE && obj_type != OBJECT_TYPE_BTREE) {
        Logger::debug("Not a B-tree node, type: 0x" + std::to_string(obj_type));
        return true;  // Not an error, just not found
    }

    uint32_t nkeys = node->btn_nkeys;
    Logger::debug("Searching node with " + std::to_string(nkeys) + " keys at level " +
                  std::to_string(node->btn_level));

    // Binary search through keys
    for (uint32_t i = 0; i < nkeys; i++) {
        const uint8_t* node_key;
        const uint8_t* node_val;
        uint16_t key_len, val_len;

        if (!getKeyValue(node, i, node_key, key_len, node_val, val_len)) {
            continue;
        }

        int cmp = compareKeys(key, key_size, node_key, key_len);

        if (cmp == 0) {
            // Found the key
            value.assign(node_val, node_val + val_len);
            found = true;
            return true;
        }
    }

    return true;
}

bool BTree::getKeyValue(const btree_node_phys_t* node, uint32_t index,
                        const uint8_t*& key, uint16_t& key_len,
                        const uint8_t*& val, uint16_t& val_len) {
    if (index >= node->btn_nkeys) {
        return false;
    }

    // Get table of contents - keys and values are stored at offsets
    // This is a simplified implementation
    const uint8_t* node_base = reinterpret_cast<const uint8_t*>(node);
    const nloc_t* toc = reinterpret_cast<const nloc_t*>(node->btn_data);

    // Key location
    nloc_t key_loc = toc[index];
    key = node_base + key_loc.off;
    key_len = key_loc.len;

    // Value location (comes after keys in TOC)
    nloc_t val_loc = toc[node->btn_nkeys + index];
    val = node_base + val_loc.off;
    val_len = val_loc.len;

    return true;
}

int BTree::compareKeys(const void* key1, size_t size1, const void* key2, size_t size2) {
    size_t min_size = std::min(size1, size2);
    int cmp = memcmp(key1, key2, min_size);

    if (cmp != 0) {
        return cmp;
    }

    if (size1 < size2) return -1;
    if (size1 > size2) return 1;
    return 0;
}

bool BTree::lookupRange(const void* key_min, size_t key_min_size,
                        const void* key_max, size_t key_max_size,
                        std::vector<BTreeEntry>& entries) {
    // Simplified implementation - would need to traverse tree and collect all matching entries
    entries.clear();
    return true;
}

bool BTree::enumerate(std::function<bool(const BTreeEntry&)> callback) {
    // Simplified implementation - would traverse entire tree
    return true;
}
