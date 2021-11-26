/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <optional>
#include <sstream>

#include "BootControl.h"
#include <sysexits.h>

using android::hardware::hidl_string;
using android::hardware::Return;

enum BootCtlVersion { BOOTCTL_V1_0, BOOTCTL_V1_1, BOOTCTL_V1_2 };

static void usage(FILE* where, BootCtlVersion /* bootVersion */, int /* argc */, char* argv[]) {
    fprintf(where,
            "%s - command-line wrapper for the boot HAL.\n"
            "\n"
            "Usage:\n"
            "  %s COMMAND\n"
            "\n"
            "Commands:\n"
            "  get-number-slots               - Prints number of slots.\n"
            "  get-current-slot               - Prints currently running SLOT.\n"
            "  mark-boot-successful           - Mark current slot as GOOD.\n"
            "  get-active-boot-slot           - Prints the SLOT to load on next boot.\n"
            "  set-active-boot-slot SLOT      - On next boot, load and execute SLOT.\n"
            "  set-slot-as-unbootable SLOT    - Mark SLOT as invalid.\n"
            "  is-slot-bootable SLOT          - Returns 0 only if SLOT is bootable.\n"
            "  is-slot-marked-successful SLOT - Returns 0 only if SLOT is marked GOOD.\n"
            "  get-suffix SLOT                - Prints suffix for SLOT.\n"
            "\n"
            "SLOT parameter is the zero-based slot-number.\n",
            argv[0], argv[0]);
}

static int do_get_number_slots(BootControl module) {
    uint32_t numSlots = module.getNumberSlots();
    fprintf(stdout, "%u\n", numSlots);
    return EX_OK;
}

static int do_get_current_slot(BootControl module) {
    Slot curSlot = module.getCurrentSlot();
    fprintf(stdout, "%u\n", curSlot);
    return EX_OK;
}

static std::function<void(CommandResult)> generate_callback(CommandResult* crp) {
    return [=](CommandResult cr) { *crp = cr; };
}

static int handle_return(const Return<void>& ret, CommandResult cr, const char* errStr) {
    if (!ret.isOk()) {
        fprintf(stderr, errStr, ret.description().c_str());
        return EX_SOFTWARE;
    } else if (!cr.success) {
        fprintf(stderr, errStr, cr.errMsg.c_str());
        return EX_SOFTWARE;
    }
    return EX_OK;
}

static int do_mark_boot_successful(BootControl module) {
    CommandResult cr;
    Return<void> ret = module.markBootSuccessful(generate_callback(&cr));
    return handle_return(ret, cr, "Error marking as having booted successfully: %s\n");
}

static int do_get_active_boot_slot(BootControl module) {
    uint32_t slot = module.getActiveBootSlot();
    fprintf(stdout, "%u\n", slot);
    return EX_OK;
}

static int do_set_active_boot_slot(BootControl module, Slot slot_number) {
    CommandResult cr;
    Return<void> ret = module.setActiveBootSlot(slot_number, generate_callback(&cr));
    return handle_return(ret, cr, "Error setting active boot slot: %s\n");
}

static int do_set_slot_as_unbootable(BootControl module, Slot slot_number) {
    CommandResult cr;
    Return<void> ret = module.setSlotAsUnbootable(slot_number, generate_callback(&cr));
    return handle_return(ret, cr, "Error setting slot as unbootable: %s\n");
}

static int handle_return(const Return<BoolResult>& ret, const char* errStr) {
    if (!ret.isOk()) {
        fprintf(stderr, errStr, ret.description().c_str());
        return EX_SOFTWARE;
    } else if (ret == BoolResult::INVALID_SLOT) {
        fprintf(stderr, errStr, "Invalid slot");
        return EX_SOFTWARE;
    } else if (ret == BoolResult::TRUE) {
        return EX_OK;
    }
    return EX_SOFTWARE;
}

static int do_is_slot_bootable(BootControl module, Slot slot_number) {
    Return<BoolResult> ret = module.isSlotBootable(slot_number);
    return handle_return(ret, "Error calling isSlotBootable(): %s\n");
}

static int do_is_slot_marked_successful(BootControl module, Slot slot_number) {
    Return<BoolResult> ret = module.isSlotMarkedSuccessful(slot_number);
    return handle_return(ret, "Error calling isSlotMarkedSuccessful(): %s\n");
}

static int do_get_suffix(BootControl module, Slot slot_number) {
    std::function<void(hidl_string)> cb = [](hidl_string suffix) {
        fprintf(stdout, "%s\n", suffix.c_str());
    };
    Return<void> ret = module.getSuffix(slot_number, cb);
    if (!ret.isOk()) {
        fprintf(stderr, "Error calling getSuffix(): %s\n", ret.description().c_str());
        return EX_SOFTWARE;
    }
    return EX_OK;
}

static uint32_t parse_slot(BootCtlVersion bootVersion, int pos, int argc, char* argv[]) {
    if (pos > argc - 1) {
        usage(stderr, bootVersion, argc, argv);
        exit(EX_USAGE);
        return -1;
    }
    errno = 0;
    uint64_t ret = strtoul(argv[pos], NULL, 10);
    if (errno != 0 || ret > UINT_MAX) {
        usage(stderr, bootVersion, argc, argv);
        exit(EX_USAGE);
        return -1;
    }
    return (uint32_t)ret;
}

int main(int argc, char* argv[]) {
    BootControl v1_0_module;
    BootControl v1_1_module;
    BootControl v1_2_module;
    BootCtlVersion bootVersion = BOOTCTL_V1_0;

    v1_0_module = BootControl();
    v1_1_module = v1_0_module;
    v1_2_module = v1_0_module;
    bootVersion = BOOTCTL_V1_2;

    if (argc < 2) {
        usage(stderr, bootVersion, argc, argv);
        return EX_USAGE;
    }

    // Functions present from version 1.0
    if (strcmp(argv[1], "get-number-slots") == 0) {
        return do_get_number_slots(v1_0_module);
    } else if (strcmp(argv[1], "get-current-slot") == 0) {
        return do_get_current_slot(v1_0_module);
    } else if (strcmp(argv[1], "mark-boot-successful") == 0) {
        return do_mark_boot_successful(v1_0_module);
    } else if (strcmp(argv[1], "set-active-boot-slot") == 0) {
        return do_set_active_boot_slot(v1_0_module, parse_slot(bootVersion, 2, argc, argv));
    } else if (strcmp(argv[1], "set-slot-as-unbootable") == 0) {
        return do_set_slot_as_unbootable(v1_0_module, parse_slot(bootVersion, 2, argc, argv));
    } else if (strcmp(argv[1], "is-slot-bootable") == 0) {
        return do_is_slot_bootable(v1_0_module, parse_slot(bootVersion, 2, argc, argv));
    } else if (strcmp(argv[1], "is-slot-marked-successful") == 0) {
        return do_is_slot_marked_successful(v1_0_module, parse_slot(bootVersion, 2, argc, argv));
    } else if (strcmp(argv[1], "get-suffix") == 0) {
        return do_get_suffix(v1_0_module, parse_slot(bootVersion, 2, argc, argv));
    } else if (strcmp(argv[1], "get-active-boot-slot") == 0) {
        return do_get_active_boot_slot(v1_2_module);
    }

    // Parameter not matched, print usage
    usage(stderr, bootVersion, argc, argv);
    return EX_USAGE;
}
