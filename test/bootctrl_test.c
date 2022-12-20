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
#include <string.h>
#include <bootctrl.h>

void printUsage(const char** argv) {
    fprintf(stderr, "usage)\n");
    fprintf(stderr, " - ./%s get\n", argv[0]);
    fprintf(stderr, " - ./%s set {active_part_id}\n", argv[0]);
}

int main(int argc, const char** argv) {
    int val;
    const char* mode;

    if (argc < 2) {
        printUsage(argv);
        return -1;
    }

    mode = argv[1];

    if (strcmp(mode, "get") == 0) {
        // get
        val = getCurrentSlot();
        printf("Active slot: %d\n", val);
        val = getAvailableSlot();
        printf("Available slot: %d\n", val);
    } else if (strcmp(mode, "set") == 0) {
        // set
        val = atoi(argv[2]);
        if (setActiveBootSlot(val)) {
            printf("Active slot changed: %d\n", val);
        }
    } else {
        printUsage(argv);
    }

    return 0;
}
