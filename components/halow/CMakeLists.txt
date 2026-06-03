# Copyright 2025 Robert Carey
# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(src "mmhalow.c")

set(inc ".")

idf_component_register(
	INCLUDE_DIRS
	${inc}
	SRCS
	${src}
	REQUIRES
	esp_netif
	esp_wifi
)

add_subdirectory(morse)
add_dependencies(${COMPONENT_TARGET} morsefirmware mmpktmem shims mmutils morselib regdb)
target_link_libraries(${COMPONENT_TARGET} INTERFACE "${MORSE_FW_OBJ}" "${MORSE_BCF_OBJ}")
target_link_libraries(${COMPONENT_TARGET} PUBLIC morsefirmware_objs mmpktmem shims mmutils morselib regdb)
