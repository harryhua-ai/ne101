#ifndef __MORSE_DPP_CLI_H__
#define __MORSE_DPP_CLI_H__

#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Default DPP push-button wait time (ms). */
#define MORSE_DPP_PB_DEFAULT_TIMEOUT_MS  (200000U)

/**
 * Delay before mmwlan_dpp_stop() after PB success so the enrollee can send
 * DPP Configuration Result to the configurator (hostapd waits ~2 s).
 */
#define MORSE_DPP_PB_CONF_RESULT_DELAY_MS  (3500U)

void morse_dpp_cli_init(void);

/**
 * Run HaLow DPP push-button provisioning (blocking).
 * Saves credentials to NVS and reconnects on success.
 */
esp_err_t morse_dpp_pb_run(uint32_t timeout_ms);

/** Stop an in-progress DPP session. */
void morse_dpp_pb_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __MORSE_DPP_CLI_H__ */
