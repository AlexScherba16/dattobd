// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (C) 2024 Datto Inc.
 */

#include "includes.h"

MODULE_LICENSE("GPL");

static inline void dummy(void){
	struct bdev_handle *bd = bdev_open_by_path("path", FMODE_READ, NULL, NULL);
	if(bd) bd = NULL;
}
