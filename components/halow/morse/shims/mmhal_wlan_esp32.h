/*
 * ESP32-specific extensions to mmhal_wlan (SDIO-over-SPI clock control).
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

/** Change host SPI clock after Morse transport init / mmwlan_boot(). */
void mmhal_wlan_spi_set_clock_hz(uint32_t clock_hz);
