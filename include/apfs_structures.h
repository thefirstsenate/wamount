#ifndef APFS_STRUCTURES_H
#define APFS_STRUCTURES_H

#include "apfs_types.h"

// NX Superblock (Container Superblock)
struct nx_superblock_t {
    obj_phys_t nx_o;
    uint32_t nx_magic;                          // 'NXSB'
    uint32_t nx_block_size;                     // Block size
    uint64_t nx_block_count;                    // Total blocks in container

    uint64_t nx_features;                       // Feature flags
    uint64_t nx_readonly_compatible_features;
    uint64_t nx_incompatible_features;

    uuid_t nx_uuid;                             // Container UUID

    oid_t nx_next_oid;                          // Next object ID to use
    xid_t nx_next_xid;                          // Next transaction ID

    uint32_t nx_xp_desc_blocks;                 // Checkpoint descriptor blocks
    uint32_t nx_xp_data_blocks;                 // Checkpoint data blocks
    paddr_t nx_xp_desc_base;                    // Base address of checkpoint descriptors
    paddr_t nx_xp_data_base;                    // Base address of checkpoint data
    uint32_t nx_xp_desc_next;                   // Next checkpoint descriptor index
    uint32_t nx_xp_data_next;                   // Next checkpoint data index
    uint32_t nx_xp_desc_index;                  // Current checkpoint descriptor index
    uint32_t nx_xp_desc_len;                    // Checkpoint descriptor length
    uint32_t nx_xp_data_index;                  // Current checkpoint data index
    uint32_t nx_xp_data_len;                    // Checkpoint data length

    oid_t nx_spaceman_oid;                      // Space manager object ID
    oid_t nx_omap_oid;                          // Object map object ID
    oid_t nx_reaper_oid;                        // Reaper object ID

    uint32_t nx_test_type;                      // Test type (for testing only)

    uint32_t nx_max_file_systems;               // Maximum number of file systems
    oid_t nx_fs_oid[NX_MAX_FILE_SYSTEMS];       // File system object IDs
    uint64_t nx_counters[32];                   // Statistics counters
    prange_t nx_blocked_out_prange;             // Blocked out physical range
    oid_t nx_evict_mapping_tree_oid;            // Evict mapping tree object ID
    uint64_t nx_flags;                          // Container flags
    paddr_t nx_efi_jumpstart;                   // EFI jumpstart location
    uuid_t nx_fusion_uuid;                      // Fusion drive UUID
    prange_t nx_keylocker;                      // Key locker range
    uint64_t nx_ephemeral_info[4];              // Ephemeral information

    oid_t nx_test_oid;                          // Test object ID

    oid_t nx_fusion_mt_oid;                     // Fusion middle tree object ID
    oid_t nx_fusion_wbc_oid;                    // Fusion write-back cache object ID
    prange_t nx_fusion_wbc;                     // Fusion write-back cache range

    uint64_t nx_newest_mounted_version;         // Newest mounted version

    prange_t nx_mkb_locker;                     // Media key bag locker
} __attribute__((packed));

// Physical range
struct prange_t {
    paddr_t pr_start_paddr;                     // Start physical address
    uint64_t pr_block_count;                    // Number of blocks
} __attribute__((packed));

// Object Map (OMAP)
struct omap_phys_t {
    obj_phys_t om_o;
    uint32_t om_flags;                          // Flags
    uint32_t om_snap_count;                     // Snapshot count
    uint32_t om_tree_type;                      // Tree type
    uint32_t om_snapshot_tree_type;             // Snapshot tree type
    oid_t om_tree_oid;                          // Tree object ID
    oid_t om_snapshot_tree_oid;                 // Snapshot tree object ID
    xid_t om_most_recent_snap;                  // Most recent snapshot transaction
    xid_t om_pending_revert_min;                // Minimum pending revert transaction
    xid_t om_pending_revert_max;                // Maximum pending revert transaction
} __attribute__((packed));

// OMAP key
struct omap_key_t {
    oid_t ok_oid;                               // Object ID
    xid_t ok_xid;                               // Transaction ID
} __attribute__((packed));

// OMAP value
struct omap_val_t {
    uint32_t ov_flags;                          // Flags
    uint32_t ov_size;                           // Size
    paddr_t ov_paddr;                           // Physical address
} __attribute__((packed));

// B-tree node descriptor
struct btree_node_phys_t {
    obj_phys_t btn_o;
    uint16_t btn_flags;                         // Node flags
    uint16_t btn_level;                         // Level in tree
    uint32_t btn_nkeys;                         // Number of keys
    nloc_t btn_table_space;                     // Table space location
    nloc_t btn_free_space;                      // Free space location
    nloc_t btn_key_free_list;                   // Key free list location
    nloc_t btn_val_free_list;                   // Value free list location
    uint64_t btn_data[];                        // Variable length data
} __attribute__((packed));

// Node location
struct nloc_t {
    uint16_t off;                               // Offset
    uint16_t len;                               // Length
} __attribute__((packed));

// B-tree info
struct btree_info_t {
    uint32_t bt_flags;                          // Flags
    uint32_t bt_node_size;                      // Node size
    uint32_t bt_key_size;                       // Key size
    uint32_t bt_val_size;                       // Value size
} __attribute__((packed));

// APFS Volume Superblock
struct apfs_superblock_t {
    obj_phys_t apfs_o;
    uint32_t apfs_magic;                        // 'APSB'
    uint32_t apfs_fs_index;                     // Index in container

    uint64_t apfs_features;                     // Feature flags
    uint64_t apfs_readonly_compatible_features;
    uint64_t apfs_incompatible_features;

    uint64_t apfs_unmount_time;                 // Last unmount time

    uint64_t apfs_fs_reserve_block_count;       // Reserved blocks
    uint64_t apfs_fs_quota_block_count;         // Quota blocks
    uint64_t apfs_fs_alloc_count;               // Allocated blocks

    wrapped_meta_crypto_state_t apfs_meta_crypto; // Crypto state

    uint32_t apfs_root_tree_type;               // Root tree type
    uint32_t apfs_extentref_tree_type;          // Extent reference tree type
    uint32_t apfs_snap_meta_tree_type;          // Snapshot metadata tree type

    oid_t apfs_omap_oid;                        // Object map OID
    oid_t apfs_root_tree_oid;                   // Root tree OID
    oid_t apfs_extentref_tree_oid;              // Extent reference tree OID
    oid_t apfs_snap_meta_tree_oid;              // Snapshot metadata tree OID

    xid_t apfs_revert_to_xid;                   // Revert to transaction ID
    oid_t apfs_revert_to_sblock_oid;            // Revert to superblock OID

    uint64_t apfs_next_obj_id;                  // Next object ID

    uint64_t apfs_num_files;                    // Number of files
    uint64_t apfs_num_directories;              // Number of directories
    uint64_t apfs_num_symlinks;                 // Number of symlinks
    uint64_t apfs_num_other_fsobjects;          // Number of other objects
    uint64_t apfs_num_snapshots;                // Number of snapshots

    uint64_t apfs_total_blocks_alloced;         // Total allocated blocks
    uint64_t apfs_total_blocks_freed;           // Total freed blocks

    uuid_t apfs_vol_uuid;                       // Volume UUID
    uint64_t apfs_last_mod_time;                // Last modification time

    uint64_t apfs_fs_flags;                     // Filesystem flags

    apfs_modified_by_t apfs_formatted_by;       // Formatted by info
    apfs_modified_by_t apfs_modified_by[8];     // Modified by info (max 8 entries)

    uint8_t apfs_volname[256];                  // Volume name
    uint32_t apfs_next_doc_id;                  // Next document ID

    uint16_t apfs_role;                         // Volume role
    uint16_t apfs_reserved;

    xid_t apfs_root_to_xid;                     // Root to transaction ID
    oid_t apfs_er_state_oid;                    // ER state OID

    uint64_t apfs_cloneinfo_id_epoch;           // Clone info ID epoch
    uint64_t apfs_cloneinfo_xid;                // Clone info transaction ID

    oid_t apfs_snap_meta_ext_oid;               // Snapshot metadata extent OID

    uuid_t apfs_volume_group_id;                // Volume group ID

    oid_t apfs_integrity_meta_oid;              // Integrity metadata OID
    oid_t apfs_fext_tree_oid;                   // File extent tree OID
    uint32_t apfs_fext_tree_type;               // File extent tree type
    uint32_t apfs_reserved_type;
    oid_t apfs_reserved_oid;
} __attribute__((packed));

// Wrapped crypto state
struct wrapped_meta_crypto_state_t {
    uint16_t major_version;
    uint16_t minor_version;
    uint32_t cpflags;
    uint32_t persistent_class;
    uint32_t key_os_version;
    uint16_t key_revision;
    uint16_t unused;
} __attribute__((packed));

// Modified by info
struct apfs_modified_by_t {
    uint8_t id[32];                             // Identifier
    uint64_t timestamp;                         // Timestamp
    xid_t last_xid;                             // Last transaction ID
} __attribute__((packed));

// J-tree key (used in APFS filesystem B-tree)
struct j_key_t {
    uint64_t obj_id_and_type;                   // Object ID and type combined
} __attribute__((packed));

// Helper macros to extract obj_id and type
#define OBJ_ID_MASK                            0x0fffffffffffffffULL
#define OBJ_TYPE_MASK                          0xf000000000000000ULL
#define OBJ_TYPE_SHIFT                         60

inline uint64_t j_key_obj_id(const j_key_t* key) {
    return key->obj_id_and_type & OBJ_ID_MASK;
}

inline uint8_t j_key_obj_type(const j_key_t* key) {
    return (key->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT;
}

// Inode value
struct j_inode_val_t {
    uint64_t parent_id;                         // Parent directory ID
    uint64_t private_id;                        // Private ID
    uint64_t create_time;                       // Creation time
    uint64_t mod_time;                          // Modification time
    uint64_t change_time;                       // Change time
    uint64_t access_time;                       // Access time
    uint64_t internal_flags;                    // Internal flags
    union {
        int32_t nchildren;                      // Number of children (for dir)
        int32_t nlink;                          // Number of links (for file)
    };
    uint32_t default_protection_class;          // Default protection class
    uint32_t write_generation_counter;          // Write generation counter
    uint32_t bsd_flags;                         // BSD flags
    uint32_t owner;                             // Owner UID
    uint32_t group;                             // Group GID
    uint16_t mode;                              // File mode
    uint16_t pad1;
    uint64_t uncompressed_size;                 // Uncompressed size
    uint8_t xfields[];                          // Extended fields
} __attribute__((packed));

// Directory record key
struct j_drec_key_t {
    j_key_t hdr;
    uint16_t name_len;                          // Name length
    uint8_t name[];                             // Name (variable length)
} __attribute__((packed));

// Directory record value
struct j_drec_val_t {
    uint64_t file_id;                           // File inode number
    uint64_t date_added;                        // Date added
    uint16_t flags;                             // Flags
    uint8_t xfields[];                          // Extended fields
} __attribute__((packed));

// File extent key
struct j_file_extent_key_t {
    j_key_t hdr;
    uint64_t logical_addr;                      // Logical address in file
} __attribute__((packed));

// File extent value
struct j_file_extent_val_t {
    uint64_t len_and_flags;                     // Length and flags
    uint64_t phys_block_num;                    // Physical block number
    uint64_t crypto_id;                         // Crypto ID
} __attribute__((packed));

// Extended field header
struct x_field_t {
    uint8_t x_type;                             // Field type
    uint8_t x_flags;                            // Flags
    uint16_t x_size;                            // Size
} __attribute__((packed));

// Extended field types
#define INO_EXT_TYPE_SNAP_XID                  1
#define INO_EXT_TYPE_DELTA_TREE_OID            2
#define INO_EXT_TYPE_DOCUMENT_ID               3
#define INO_EXT_TYPE_NAME                      4
#define INO_EXT_TYPE_PREV_FSIZE                5
#define INO_EXT_TYPE_RESERVED_6                6
#define INO_EXT_TYPE_FINDER_INFO               7
#define INO_EXT_TYPE_DSTREAM                   8
#define INO_EXT_TYPE_RESERVED_9                9
#define INO_EXT_TYPE_DIR_STATS_KEY             10
#define INO_EXT_TYPE_FS_UUID                   11
#define INO_EXT_TYPE_RESERVED_12               12
#define INO_EXT_TYPE_SPARSE_BYTES              13
#define INO_EXT_TYPE_RDEV                      14
#define INO_EXT_TYPE_PURGEABLE_FLAGS           15
#define INO_EXT_TYPE_ORIG_SYNC_ROOT_ID         16

// Data stream
struct j_dstream_t {
    uint64_t size;                              // Size
    uint64_t alloced_size;                      // Allocated size
    uint64_t default_crypto_id;                 // Default crypto ID
    uint64_t total_bytes_written;               // Total bytes written
    uint64_t total_bytes_read;                  // Total bytes read
} __attribute__((packed));

#endif // APFS_STRUCTURES_H
