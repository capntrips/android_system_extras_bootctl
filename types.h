/*
 * Copyright 2016 The Android Open Source Project
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

#include <string>
#include <cstdint>

/**
 * A command result encapsulating whether the command succeeded and
 * an error string.
 */
struct CommandResult {
    bool success;
    std::string errMsg;
};

/**
 * An identifier for a slot number.
 */
typedef uint32_t Slot;

/**
 * A result encapsulating whether a function returned true, false or
 * failed due to an invalid slot number
 */
enum BoolResult : int32_t {
    FALSE = 0,
    TRUE = 1,
    INVALID_SLOT = -1
};
