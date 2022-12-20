// Copyright (c) 2022 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>

#include <libuboot.h>
#include <bootctrl.h>

#define KEY_ACTIVE_PART "active_part_id"
#define KEY_ON_UPGRADE "now_on_upgrade"
#define KEY_TRYCOUNT "boot_try"
#define A_PARTITION_INDEX 2

/**
 * Utility functions
 */
struct uboot_ctx* initializeContext();
void finalizeContext(struct uboot_ctx* ctx);

/**
 * get current slot
 * return)
 *   0: active_part_id == 2
 *   1: active_part_id == 3
 */
int getCurrentSlot() {
    int curr;
    const char *value;
    struct uboot_ctx *ctx;

    ctx = initializeContext();
    if (ctx == NULL) {
        return -1;
    }

    value = libuboot_get_env(ctx, KEY_ACTIVE_PART);
    curr = atoi(value) - A_PARTITION_INDEX;

    finalizeContext(ctx);
    return curr;
}

/**
 * get available slot
 * return)
 *   0: active_part_id == 3
 *   1: active_part_id == 2
 */
int getAvailableSlot() {
    int current = getCurrentSlot();
    if (current == 0) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * change active slot (to upgrade) and set upgrade flag / try count (for rollback)
 * args)
 *   slot: slot number
 * return)
 *   true if success, false if failed
 */
bool setActiveBootSlot(int slot) {
    int currSlot, ret;
    char buf[10];
    const char *value;
    struct uboot_ctx *ctx;

    ctx = initializeContext();
    if (ctx == NULL) {
        return false;
    }

    // get current slot and compare
    value = libuboot_get_env(ctx, KEY_ACTIVE_PART);
    currSlot = atoi(value) - A_PARTITION_INDEX;
    if (currSlot == slot) {
        fprintf(stdout, "[libbootctrl] INFO: slot[%d] already active state.\n", slot);
        return false;
    }

    // set active partition id
    sprintf(buf, "%d", slot + A_PARTITION_INDEX);
    ret = libuboot_set_env(ctx, KEY_ACTIVE_PART, buf);
    if (ret < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: setenv error for '%s': ret_code = %d\n", KEY_ACTIVE_PART, ret);
        return false;
    }

    // set upgrade flag
    ret = libuboot_set_env(ctx, KEY_ON_UPGRADE, "1");
    if (ret < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: setenv error for '%s': ret_code = %d\n", KEY_ON_UPGRADE, ret);
        return false;
    }

    // set try count as 0
    ret = libuboot_set_env(ctx, KEY_TRYCOUNT, "0");
    if (ret < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: setenv error for '%s': ret_code = %d\n", KEY_TRYCOUNT, ret);
        return false;
    }

    // save into file
    ret = libuboot_env_store(ctx);
    if (ret < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: failed to store: ret_code = %d\n", ret);
        return false;
    }

    finalizeContext(ctx);
    return true;
}

/*********************************************************************************/
/*********************************************************************************/

struct uboot_ctx* initializeContext() {
    int ret;
    struct uboot_ctx *ctx;

    if (libuboot_initialize(&ctx, NULL) < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: Cannot initialize environment\n");
        return NULL;
    }

    if ((ret = libuboot_read_config(ctx, "/etc/fw_env.config")) < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: Configuration file wrong or corrupted\n");
        libuboot_exit(ctx);
        return NULL;
    }

    if ((ret = libuboot_open(ctx)) < 0) {
        fprintf(stderr, "[libbootctrl] ERROR: Cannot read environment, failed!\n");
        libuboot_close(ctx);
        return NULL;
    }

    fprintf(stdout, "[libbootctrl] INFO: uboot open successed: %p\n", ctx);
    return ctx;
}

void finalizeContext(struct uboot_ctx* ctx) {
    fprintf(stdout, "[libbootctrl] INFO: uboot close: %p\n", ctx);
    libuboot_close(ctx);
    libuboot_exit(ctx);
}


