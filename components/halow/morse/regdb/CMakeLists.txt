# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(ROOT_DIR ${MM_IOT_SDK}/framework/src/mmregdb)

set(inc
    ${ROOT_DIR})
set(src
    ${ROOT_DIR}/mmregdb.c
)

add_library(regdb STATIC ${src})

target_include_directories(regdb PUBLIC ${inc})
add_dependencies(regdb shims morselib)
target_link_libraries(regdb PUBLIC shims morselib)
