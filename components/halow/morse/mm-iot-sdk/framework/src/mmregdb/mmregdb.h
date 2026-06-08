/*
 * Copyright 2022-2024 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

/**
 * @ingroup MMWLAN_REGDB
 * @defgroup MMWLAN_REGDB_HEADER Template S1G regulatory database header
 *
 * \@{
 *
 * @section MMWLAN_REGDB_OVERRIDE Overriding Regulatory Database
 * The data in the database is defined in @ref mmregdb.c, and can be overwritten as required.
 */

#pragma once

#include "mmwlan.h"

/**
 * Get a pointer to regulatory_db.
 * Stores channel information for each supported domain.
 * For more info, @ref see mmregdb.c.
 *
 * @return Reference to the regulatory database
 */
const struct mmwlan_regulatory_db *get_regulatory_db(void);

/**
 * Look up a regulatory domain by its full domain code.
 *
 * Standard ISO domains use two-letter codes (e.g. "US", "EU"). Australia exposes
 * three channelisation variants: "AU-2020", "AU-2024", and "AU-revmf".
 *
 * @param domain_code  Domain code to look up.
 *
 * @returns the matching channel list if found, else NULL.
 */
const struct mmwlan_s1g_channel_list *mmregdb_lookup_domain(const char *domain_code);

/** \@} */
