# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(ROOT_DIR ${MM_IOT_SDK}/framework/morselib)

if(CONFIG_MM_MORSELIB_USE_PREBUILT)
    # Use a prebuilt libmorse.a and skip compiling morselib sources and mangling.
    set(_prebuilt "${CONFIG_MM_MORSELIB_PREBUILT_PATH}")
    if(NOT IS_ABSOLUTE "${_prebuilt}")
        get_filename_component(_prebuilt "${_prebuilt}" ABSOLUTE BASE_DIR "${CMAKE_SOURCE_DIR}")
    endif()

    if(NOT EXISTS "${_prebuilt}")
        message(FATAL_ERROR
            "CONFIG_MM_MORSELIB_USE_PREBUILT is enabled but the prebuilt morselib archive was not found:\n"
            "  CONFIG_MM_MORSELIB_PREBUILT_PATH='${CONFIG_MM_MORSELIB_PREBUILT_PATH}'\n"
            "  resolved='${_prebuilt}'\n"
            "Provide a valid libmorse.a path or disable CONFIG_MM_MORSELIB_USE_PREBUILT."
        )
    endif()

    message(STATUS "Using prebuilt morselib: ${_prebuilt}")

    set(INC
        ${ROOT_DIR}/include
        ${ROOT_DIR}/src
        ${ROOT_DIR}/src/internal
        ${ROOT_DIR}/src/umac/rc/mmrc_osal
        ${ROOT_DIR}/mmrc/src/core
    )

    add_library(morselib STATIC IMPORTED GLOBAL)
    set_target_properties(morselib PROPERTIES IMPORTED_LOCATION "${_prebuilt}")

    set_property(TARGET morselib PROPERTY INTERFACE_LINK_LIBRARIES mmpktmem)
    target_include_directories(morselib INTERFACE ${INC})
    target_link_libraries(morselib INTERFACE shims mmpktmem)

    return()
endif()

set(INC
    ${ROOT_DIR}/include
    ${ROOT_DIR}/src
    ${ROOT_DIR}/src/internal
    ${ROOT_DIR}/src/umac/rc/mmrc_osal
    ${ROOT_DIR}/mmrc/src/core
)
set(SRCS
    ${ROOT_DIR}/mmrc/src/core/mmrc.c
    ${ROOT_DIR}/src/common/mac_address.c
    ${ROOT_DIR}/src/common/mmpkt.c
    ${ROOT_DIR}/src/common/mmpkt_list.c
    ${ROOT_DIR}/src/dot11/dot11_utils.c
    ${ROOT_DIR}/src/umac/ap/umac_ap.c
    ${ROOT_DIR}/src/umac/ate/umac_ate.c
    ${ROOT_DIR}/src/umac/ba/umac_ba.c
    ${ROOT_DIR}/src/umac/connection/umac_connection.c
    ${ROOT_DIR}/src/umac/config/umac_config.c
    ${ROOT_DIR}/src/umac/core/umac_core.c
    ${ROOT_DIR}/src/umac/core/umac_evtloop.c
    ${ROOT_DIR}/src/umac/core/umac_evtq.c
    ${ROOT_DIR}/src/umac/core/umac_timeout.c
    ${ROOT_DIR}/src/umac/data/umac_data.c
    ${ROOT_DIR}/src/umac/data/umac_sta_data.c
    ${ROOT_DIR}/src/umac/datapath/datapath_defrag.c
    ${ROOT_DIR}/src/umac/datapath/umac_datapath.c
    ${ROOT_DIR}/src/umac/datapath/umac_datapath_ap.c
    ${ROOT_DIR}/src/umac/frames/action.c
    ${ROOT_DIR}/src/umac/frames/association.c
    ${ROOT_DIR}/src/umac/frames/authentication.c
    ${ROOT_DIR}/src/umac/frames/deauthentication.c
    ${ROOT_DIR}/src/umac/frames/disassociation.c
    ${ROOT_DIR}/src/umac/frames/frame_constructor.c
    ${ROOT_DIR}/src/umac/frames/probe_request.c
    ${ROOT_DIR}/src/umac/frames/probe_response.c
    ${ROOT_DIR}/src/umac/frames/utils.c
    ${ROOT_DIR}/src/umac/ies/aid_request.c
    ${ROOT_DIR}/src/umac/ies/twt_ie.c
    ${ROOT_DIR}/src/umac/ies/s1g_capabilities.c
    ${ROOT_DIR}/src/umac/ies/s1g_operation.c
    ${ROOT_DIR}/src/umac/ies/s1g_tim.c
    ${ROOT_DIR}/src/umac/ies/ies_common.c
    ${ROOT_DIR}/src/umac/ies/wmm.c
    ${ROOT_DIR}/src/umac/ies/morse_ie.c
    ${ROOT_DIR}/src/umac/interface/umac_interface.c
    ${ROOT_DIR}/src/umac/keys/connection_keys.c
    ${ROOT_DIR}/src/umac/keys/umac_keys.c
    ${ROOT_DIR}/src/umac/ps/umac_ps.c
    ${ROOT_DIR}/src/umac/rc/mmrc_osal/mmrc_osal.c
    ${ROOT_DIR}/src/umac/rc/umac_rc.c
    ${ROOT_DIR}/src/umac/regdb/umac_regdb.c
    ${ROOT_DIR}/src/umac/scan/umac_scan.c
    ${ROOT_DIR}/src/umac/scan/hw_scan.c
    ${ROOT_DIR}/src/umac/stats/umac_stats.c
    ${ROOT_DIR}/src/umac/supplicant_shim/bip.c
    ${ROOT_DIR}/src/umac/supplicant_shim/ccmp.c
    ${ROOT_DIR}/src/umac/supplicant_shim/config.c
    ${ROOT_DIR}/src/umac/supplicant_shim/driver.c
    ${ROOT_DIR}/src/umac/supplicant_shim/driver_ap.c
    ${ROOT_DIR}/src/umac/supplicant_shim/eloop.c
    ${ROOT_DIR}/src/umac/supplicant_shim/morse_dpp_event.c
    ${ROOT_DIR}/src/umac/supplicant_shim/random.c
    ${ROOT_DIR}/src/umac/supplicant_shim/supplicant_core.c
    ${ROOT_DIR}/src/umac/supplicant_shim/supplicant_core_ap.c
    ${ROOT_DIR}/src/umac/twt/umac_twt.c
    ${ROOT_DIR}/src/umac/umac.c
    ${ROOT_DIR}/src/umac/umac_mmdrv_shim.c
    ${ROOT_DIR}/src/umac/wnm_sleep/umac_wnm_sleep.c
    ${ROOT_DIR}/src/umac/offload/umac_offload.c
    ${ROOT_DIR}/src/driver/morse_driver/mm6108/mm6108.c
    ${ROOT_DIR}/src/driver/morse_driver/mm6108/pager_if_hw.c
    ${ROOT_DIR}/src/driver/morse_driver/mm6108/pager_if.c
    ${ROOT_DIR}/src/driver/morse_driver/mm6108/pageset.c
    ${ROOT_DIR}/src/driver/morse_driver/mm8108/mm8108.c
    ${ROOT_DIR}/src/driver/morse_driver/mm8108/yaps-hw.c
    ${ROOT_DIR}/src/driver/morse_driver/mm8108/yaps.c
    ${ROOT_DIR}/src/driver/beacon/beacon.c
    ${ROOT_DIR}/src/driver/driver.c
    ${ROOT_DIR}/src/driver/driver_task.c
    ${ROOT_DIR}/src/driver/health/driver_health.c
    ${ROOT_DIR}/src/driver/puff/puff.c
    ${ROOT_DIR}/src/driver/morse_crc/morse_crc.c
    ${ROOT_DIR}/src/driver/morse_driver/command.c
    ${ROOT_DIR}/src/driver/morse_driver/event.c
    ${ROOT_DIR}/src/driver/morse_driver/ext_host_table.c
    ${ROOT_DIR}/src/driver/morse_driver/firmware.c
    ${ROOT_DIR}/src/driver/morse_driver/firmware_mbin.c
    ${ROOT_DIR}/src/driver/morse_driver/hw.c
    ${ROOT_DIR}/src/driver/morse_driver/ps.c
    ${ROOT_DIR}/src/driver/morse_driver/skbq.c
    ${ROOT_DIR}/src/driver/transport/sdio.c
    ${ROOT_DIR}/src/driver/transport/sdio_spi.c
)

add_library(libmorse STATIC ${SRCS} ${INC})
target_include_directories(libmorse PUBLIC ${INC})
add_dependencies(libmorse mmhostap)
target_link_libraries(libmorse PRIVATE mmutils mmhostap)

set(PROTECTED_SYMS_FILE "${MM_IOT_SDK}/framework/tools/metadata/protected_syms.txt")
file(STRINGS "${PROTECTED_SYMS_FILE}" _raw_syms
     REGEX "^[^#].*")
list(TRANSFORM _raw_syms STRIP)
list(FILTER _raw_syms EXCLUDE REGEX "^$")

set(protected_syms)
foreach(sym IN LISTS _raw_syms)
    list(APPEND protected_syms -p "${sym}")
endforeach()

# Use toolchain executables CMake already resolved (portable on Linux and Windows).
set(AR "${CMAKE_AR}")
set(RANLIB "${CMAKE_RANLIB}")
# librarymangler.py appends "objcopy"/"nm" to the toolchain prefix (not a full objcopy path).
string(REGEX REPLACE "objcopy(\\.exe)?$" "" TOOLCHAIN_PREFIX "${CMAKE_OBJCOPY}")
# On Windows, cmd.exe treats "E:/..." as drive "E:"; normalize paths for custom commands.
if(CMAKE_HOST_WIN32)
    file(TO_NATIVE_PATH "${AR}" AR)
    file(TO_NATIVE_PATH "${RANLIB}" RANLIB)
    file(TO_NATIVE_PATH "${TOOLCHAIN_PREFIX}" TOOLCHAIN_PREFIX)
endif()
# Prefer ESP-IDF's PYTHON; fall back to find_package for this subdir.
if(DEFINED PYTHON)
    set(PYTHON_EXE "${PYTHON}")
else()
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(PYTHON_EXE "${Python3_EXECUTABLE}")
endif()
if(CMAKE_HOST_WIN32)
    file(TO_NATIVE_PATH "${PYTHON_EXE}" PYTHON_EXE)
endif()
set(LIBMORSE  "$<TARGET_FILE:libmorse>")
set(LIBHOSTAP "$<TARGET_FILE:mmhostap>")
set(MANGLER "${MM_IOT_SDK}/framework/tools/buildsystem/librarymangler.py")
set(MERGESCRIPT "${CMAKE_CURRENT_BINARY_DIR}/libmorse.ar")
set(LIBMORSEMANG "${CMAKE_CURRENT_BINARY_DIR}/libmorse.a")

# Export the generated archive into a stable prebuilt location inside the repo.
# This keeps a ready-to-use prebuilt lib in sync when building from source.
get_filename_component(_cc_name "${CMAKE_C_COMPILER}" NAME)
string(REGEX REPLACE "-gcc(\\.exe)?$" "" _toolchain_id "${_cc_name}")
if(_toolchain_id STREQUAL "${_cc_name}")
    # Fallback: try CXX compiler name if CC doesn't match expected pattern.
    get_filename_component(_cxx_name "${CMAKE_CXX_COMPILER}" NAME)
    string(REGEX REPLACE "-g\\+\\+(\\.exe)?$" "" _toolchain_id "${_cxx_name}")
endif()
if(_toolchain_id STREQUAL "")
    set(_toolchain_id "unknown-toolchain")
endif()
set(_export_prebuilt_dir "${CMAKE_CURRENT_LIST_DIR}/prebuilt/${IDF_TARGET}/${_toolchain_id}")
set(_export_prebuilt_a "${_export_prebuilt_dir}/libmorse.a")

file(GENERATE
    OUTPUT "${MERGESCRIPT}"
    CONTENT
    "CREATE ${LIBMORSEMANG}
    ADDLIB ${LIBMORSE}
    ADDLIB ${LIBHOSTAP}
    SAVE
    END")

add_custom_command(
    OUTPUT ${LIBMORSEMANG}
    COMMAND ${CMAKE_COMMAND} -E echo "Creating mangled libmorse"
    COMMAND ${CMAKE_COMMAND} -E cat ${MERGESCRIPT} | ${AR} -M
    COMMAND ${RANLIB} ${LIBMORSEMANG}
    COMMAND ${PYTHON_EXE} ${MANGLER} -t ${TOOLCHAIN_PREFIX} -m ${CMAKE_CURRENT_BINARY_DIR} ${protected_syms} ${LIBMORSEMANG}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS libmorse mmhostap
)

add_custom_command(
    OUTPUT "${_export_prebuilt_a}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${_export_prebuilt_dir}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBMORSEMANG}" "${_export_prebuilt_a}"
    DEPENDS ${LIBMORSEMANG}
    COMMENT "Exporting prebuilt morselib to ${_export_prebuilt_a}"
    VERBATIM
)

add_custom_target(morselib_build ALL DEPENDS ${LIBMORSEMANG} "${_export_prebuilt_a}")
add_library(morselib STATIC IMPORTED GLOBAL)
set_target_properties(morselib PROPERTIES IMPORTED_LOCATION "${LIBMORSEMANG}")

set_property(TARGET morselib PROPERTY INTERFACE_LINK_LIBRARIES mmpktmem)

target_include_directories(morselib INTERFACE ${INC})
target_link_libraries(morselib INTERFACE shims
    mmpktmem)
