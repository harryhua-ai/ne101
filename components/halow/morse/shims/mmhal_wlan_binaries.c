/*
 * Copyright 2021-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mmhal_wlan.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/*
 * ---------------------------------------------------------------------------------------------
 *                                      BCF Retrieval
 * ---------------------------------------------------------------------------------------------
 */

/*
 * The following implementation reads the BCF File from the config store.
 */

typedef struct
{
    const uint8_t *start;
    const uint8_t *end;
} mmhal_bcf_span_t;

/**
 * Current selected BCF span. If NULL, falls back to legacy single-BCF symbols.
 */
static mmhal_bcf_span_t s_selected_bcf = { 0 };

static void mmhal_wlan_set_bcf_span(const uint8_t *start, const uint8_t *end)
{
    s_selected_bcf.start = start;
    s_selected_bcf.end = end;
}

/**
 * Select embedded BCF based on ISO 3166-1 alpha-2 country code.
 *
 * Must be called before mmwlan_init()/mmwlan_boot() so morselib reads the intended BCF.
 *
 * Mapping policy:
 * - "US" and "CA" use FCC BCF
 * - everything else uses CE BCF
 *
 * If dual-BCF is not enabled at build time, this function is a no-op and the
 * legacy single embedded BCF is used.
 */
void mmhal_wlan_select_bcf_by_country(const char *country_code)
{
#if defined(CONFIG_MM_BCF_MULTI)
    const uint8_t *start = NULL;
    const uint8_t *end = NULL;

    /* Implemented by auto-generated mmhal_bcf_db.c (compiled into shims). */
    const char *mmhal_bcf_map_country_to_id(const char *cc);
    bool mmhal_bcf_lookup_span_by_id(const char *id, const uint8_t **start, const uint8_t **end);

    const char *id = mmhal_bcf_map_country_to_id(country_code);
    if (id != NULL && mmhal_bcf_lookup_span_by_id(id, &start, &end))
    {
        mmhal_wlan_set_bcf_span(start, end);
        printf("BCF select: %s (%c%c) span=%p..%p len=%u\n",
               id,
               country_code ? country_code[0] : '?',
               country_code ? country_code[1] : '?',
               (const void *)start,
               (const void *)end,
               (unsigned int)(end - start));
    }
    else
    {
        printf("BCF select: failed (%c%c)\n",
               country_code ? country_code[0] : '?',
               country_code ? country_code[1] : '?');
    }
#else
    (void)country_code;
#endif
}

void mmhal_wlan_read_bcf_file(uint32_t offset, uint32_t requested_len, struct mmhal_robuf *robuf)
{
    const uint8_t *bcf_start = NULL;
    const uint8_t *bcf_end = NULL;

#ifdef CONFIG_MM_BCF_MULTI
    if (s_selected_bcf.start != NULL && s_selected_bcf.end != NULL)
    {
        bcf_start = s_selected_bcf.start;
        bcf_end = s_selected_bcf.end;
    }
#endif

    /* Legacy single-BCF fallback (keeps existing behaviour when multi/dual is disabled). */
    if (bcf_start == NULL || bcf_end == NULL)
    {
#if !defined(CONFIG_MM_BCF_MULTI)
        /** Points to the start of the BCF binary image. Defined as part of the Makefile */
        extern uint8_t bcf_binary_start;
        /** Points to the end of the BCF binary image. Defined as part of the Makefile */
        extern uint8_t bcf_binary_end;
        bcf_start = &bcf_binary_start;
        bcf_end = &bcf_binary_end;
#else
        printf("BCF select: no valid BCF span configured\n");
        return;
#endif
    }

    size_t bcf_len = (size_t)(bcf_end - bcf_start);

    /* Initialise robuf */
    robuf->buf = NULL;
    robuf->len = 0;
    robuf->free_arg = NULL;
    robuf->free_cb = NULL;

    /* Sanity check */
    if (bcf_len < offset)
    {
        printf("Detected an attempt to start reading off the end of the bcf file.\n");
        return;
    }

    robuf->buf = (uint8_t *)bcf_start + offset;
    robuf->len = bcf_len - offset;
    robuf->len = (robuf->len < requested_len) ? robuf->len : requested_len;
}

/*
 * ---------------------------------------------------------------------------------------------
 *                                    Firmware Retrieval
 * ---------------------------------------------------------------------------------------------
 */
/** Points to the start of the firmware binary image. Defined as part of the Makefile */
extern uint8_t firmware_binary_start;
/** Points to the end of the firmware binary image. Defined as part of the Makefile */
extern uint8_t firmware_binary_end;

void mmhal_wlan_read_fw_file(uint32_t offset, uint32_t requested_len, struct mmhal_robuf *robuf)
{
    uint32_t firmware_len = &firmware_binary_end - &firmware_binary_start;
    if (offset > firmware_len)
    {
        printf("Detected an attempt to start read off the end of the firmware file.\n");
        robuf->buf = NULL;
        return;
    }

    robuf->buf = (&firmware_binary_start + offset);
    firmware_len -= offset;

    robuf->len = (firmware_len < requested_len) ? firmware_len : requested_len;
}
