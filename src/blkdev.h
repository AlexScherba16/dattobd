// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (C) 2022 Datto Inc.
 */

#ifndef BLKDEV_H_
#define BLKDEV_H_

#include "includes.h"

struct block_device;

#ifdef HAVE_BLKDEV_PUT_1
//#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
#define dattobd_blkdev_put(bdev) blkdev_put(bdev);
#elif defined HAVE_BLKDEV_PUT_2
#define dattobd_blkdev_put(bdev) blkdev_put(bdev,NULL);
#elif defined HAVE_BDEV_THAW
#define dattobd_blkdev_put(b) ({\
    struct bdev_handle* handle;\
    handle=bdev_open_by_dev(b->bd_dev, FMODE_READ, &b, NULL);\
    if(!handle){\
    LOG_DEBUG("Error could not open a handle");\
    handle=kmalloc(sizeof(struct bdev_handle),GFP_KERNEL);
    handle->bdev=b;
    handle->holder=b;
    handle->mode=FMODE_READ;
    }\
    bdev_release(handle);\
    })
#else
#define dattobd_blkdev_put(bdev) blkdev_put(bdev, FMODE_READ);
#endif

#ifndef bdev_whole
//#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
#define bdev_whole(bdev) ((bdev)->bd_contains)
#endif

#ifndef HAVE_HD_STRUCT
//#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
#define dattobd_bdev_size(bdev) (bdev_nr_sectors(bdev))
#elif !defined HAVE_PART_NR_SECTS_READ
//#if LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0)
#define dattobd_bdev_size(bdev) ((bdev)->bd_part->nr_sects)
#else
#define dattobd_bdev_size(bdev) part_nr_sects_read((bdev)->bd_part)
#endif

#if !defined HAVE_BLKDEV_GET_BY_PATH_1 && !defined HAVE_BDEV_OPEN_BY_PATH
#define dattobd_blkdev_get_by_path(path, mode, holder) blkdev_get_by_path(path,mode,holder)
#elif defined HAVE_BDEV_OPEN_BY_PATH && !defined HAVE_BLKDEV_GET_BY_PATH
#define dattobd_blkdev_get_by_path(path, mode, holder) bdev_open_by_path(path,mode,holder,NULL)->bdev
#else
#define dattobd_blkdev_get_by_path(path, mode, holder) blkdev_get_by_path(path,mode,holder,NULL)
#endif 

#if !defined HAVE_BLKDEV_GET_BY_PATH && !defined HAVE_BLKDEV_GET_BY_PATH_1 && !defined HAVE_BDEV_OPEN_BY_PATH
//#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,38)
struct block_device *blkdev_get_by_path(const char *path, fmode_t mode,
                                        void *holder);
#endif

#ifdef HAVE_BD_SUPER
#define dattobd_get_super(bdev) (bdev)->bd_super
#define dattobd_drop_super(sb)
#elif defined HAVE_GET_SUPER
#define dattobd_get_super(bdev) get_super(bdev)
#define dattobd_drop_super(sb) drop_super(sb)
#elif defined HAVE_BD_HAS_SUBMIT_BIO && !defined HAVE_BDEV_THAW
struct super_block* dattobd_get_active_super(struct block_device*);
#define dattobd_get_super(bdev) dattobd_get_active_super(bdev)
#define dattobd_drop_super(sb)
#else
#define dattobd_get_super(bdev) (struct super_block*)(bdev)->bd_holder
#define dattobd_drop_super(sb)
#endif
#endif /* BLKDEV_H_ */
