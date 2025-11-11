#ifndef APFS_TYPES_H
#define APFS_TYPES_H

#include <cstdint>

// APFS basic type definitions
typedef uint64_t oid_t;          // Object identifier
typedef uint64_t xid_t;          // Transaction identifier
typedef int64_t paddr_t;         // Physical address
typedef uint32_t apfs_hash_t;   // Hash value

// APFS Object Types
#define OBJECT_TYPE_NX_SUPERBLOCK          0x00000001
#define OBJECT_TYPE_BTREE                  0x00000002
#define OBJECT_TYPE_BTREE_NODE             0x00000003
#define OBJECT_TYPE_SPACEMAN               0x00000005
#define OBJECT_TYPE_SPACEMAN_CAB           0x00000006
#define OBJECT_TYPE_SPACEMAN_CIB           0x00000007
#define OBJECT_TYPE_SPACEMAN_BITMAP        0x00000008
#define OBJECT_TYPE_SPACEMAN_FREE_QUEUE    0x00000009
#define OBJECT_TYPE_EXTENT_LIST_TREE       0x0000000a
#define OBJECT_TYPE_OMAP                   0x0000000b
#define OBJECT_TYPE_CHECKPOINT_MAP         0x0000000c
#define OBJECT_TYPE_FS                     0x0000000d
#define OBJECT_TYPE_FSTREE                 0x0000000e
#define OBJECT_TYPE_BLOCKREFTREE           0x0000000f
#define OBJECT_TYPE_SNAPMETATREE           0x00000010

// APFS Object Flags
#define OBJ_VIRTUAL                        0x00000000
#define OBJ_EPHEMERAL                      0x80000000
#define OBJ_PHYSICAL                       0x40000000
#define OBJ_NOHEADER                       0x20000000
#define OBJ_ENCRYPTED                      0x10000000
#define OBJ_NONPERSISTENT                  0x08000000

// APFS constants
#define APFS_BLOCK_SIZE                    4096
#define NX_MAGIC                           0x4253584e  // 'NXSB'
#define APFS_MAGIC                         0x42535041  // 'APSB'
#define NX_MAX_FILE_SYSTEMS                100

// APFS Volume Flags
#define APFS_FS_UNENCRYPTED                0x00000001
#define APFS_FS_EFFACEABLE                 0x00000002
#define APFS_FS_RESERVED_4                 0x00000004
#define APFS_FS_ONEKEY                     0x00000008
#define APFS_FS_SPILLEDOVER                0x00000010
#define APFS_FS_RUN_SPILLOVER_CLEANER      0x00000020
#define APFS_FS_ALWAYS_CHECK_EXTENTREF     0x00000040

// APFS Inode Flags
#define INODE_IS_APFS_PRIVATE              0x00000001
#define INODE_MAINTAIN_DIR_STATS           0x00000002
#define INODE_DIR_STATS_ORIGIN             0x00000004
#define INODE_PROT_CLASS_EXPLICIT          0x00000008
#define INODE_WAS_CLONED                   0x00000010
#define INODE_HAS_SECURITY_EA              0x00000800
#define INODE_BEING_TRUNCATED              0x00001000
#define INODE_HAS_FINDER_INFO              0x00002000
#define INODE_IS_SPARSE                    0x00004000
#define INODE_WAS_EVER_CLONED              0x00008000
#define INODE_ACTIVE_FILE_TRIMMED          0x00010000
#define INODE_PINNED_TO_MAIN               0x00020000
#define INODE_PINNED_TO_TIER2              0x00040000
#define INODE_HAS_RSRC_FORK                0x00080000
#define INODE_NO_RSRC_FORK                 0x00100000
#define INODE_ALLOCATION_SPILLEDOVER       0x00200000

// APFS J-Tree Types
#define APFS_TYPE_ANY                      0
#define APFS_TYPE_SNAP_METADATA            1
#define APFS_TYPE_EXTENT                   2
#define APFS_TYPE_INODE                    3
#define APFS_TYPE_XATTR                    4
#define APFS_TYPE_SIBLING_LINK             5
#define APFS_TYPE_DSTREAM_ID               6
#define APFS_TYPE_CRYPTO_STATE             7
#define APFS_TYPE_FILE_EXTENT              8
#define APFS_TYPE_DIR_REC                  9
#define APFS_TYPE_DIR_STATS                10
#define APFS_TYPE_SNAP_NAME                11
#define APFS_TYPE_SIBLING_MAP              12

// File modes
#define S_IFMT                             0170000
#define S_IFIFO                            0010000
#define S_IFCHR                            0020000
#define S_IFDIR                            0040000
#define S_IFBLK                            0060000
#define S_IFREG                            0100000
#define S_IFLNK                            0120000
#define S_IFSOCK                           0140000
#define S_IFWHT                            0160000

// Basic object header (all APFS objects start with this)
struct obj_phys_t {
    uint8_t o_cksum[8];      // Fletcher checksum
    oid_t o_oid;             // Object identifier
    xid_t o_xid;             // Transaction identifier
    uint32_t o_type;         // Object type
    uint32_t o_subtype;      // Object subtype
} __attribute__((packed));

// UUID
struct uuid_t {
    uint8_t uuid[16];
} __attribute__((packed));

#endif // APFS_TYPES_H
