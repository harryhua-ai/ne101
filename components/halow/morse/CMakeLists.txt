# Copyright 2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(MM_IOT_SDK ${CMAKE_CURRENT_SOURCE_DIR}/mm-iot-sdk)

add_subdirectory(morseFirmware)
add_subdirectory(shims)
add_subdirectory(mmpktmem)
add_subdirectory(mmutils)
add_subdirectory(morselib)
add_subdirectory(regdb)
add_subdirectory(hostap)

set(MORSE_FW_OBJ ${MORSE_FW_OBJ} PARENT_SCOPE)
set(MORSE_BCF_OBJ ${MORSE_BCF_OBJ} PARENT_SCOPE)
