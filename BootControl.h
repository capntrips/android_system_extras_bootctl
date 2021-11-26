/*
 * Copyright (C) 2020 The Android Open Source Project
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

#pragma once

#include "types.h"
#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

struct BootControl {
    using markBootSuccessful_cb = std::function<void(const CommandResult& error)>;
    using setActiveBootSlot_cb = std::function<void(const CommandResult& error)>;
    using setSlotAsUnbootable_cb = std::function<void(const CommandResult& error)>;
    using getSuffix_cb = std::function<void(const ::android::hardware::hidl_string& slotSuffix)>;

    // Methods from ::android::hardware::boot::V1_0::IBootControl follow.
    Return<uint32_t> getNumberSlots();
    Return<uint32_t> getCurrentSlot();
    Return<void> markBootSuccessful(markBootSuccessful_cb _hidl_cb);
    Return<void> setActiveBootSlot(uint32_t slot, setActiveBootSlot_cb _hidl_cb);
    Return<void> setSlotAsUnbootable(uint32_t slot, setSlotAsUnbootable_cb _hidl_cb);
    Return<BoolResult> isSlotBootable(uint32_t slot);
    Return<BoolResult> isSlotMarkedSuccessful(
            uint32_t slot);
    Return<void> getSuffix(uint32_t slot, getSuffix_cb _hidl_cb);

    // Methods from ::android::hardware::boot::V1_2::IBootControl follow.
    Return<uint32_t> getActiveBootSlot();

    // Methods from ::android::hidl::base::V1_0::IBase follow.
};
