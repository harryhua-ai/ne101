#include <stdio.h>
#include <string.h>

#include "esp_console.h"
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "mmwlan.h"

#include "debug.h"
#include "config.h"
#include "morse.h"
#include "net_module.h"
#include "wifi.h"

#include "morse_dpp_cli.h"

static const char *TAG = "dpp_pb";

static SemaphoreHandle_t s_dpp_sem;
static bool s_dpp_running;

static struct
{
    bool done;
    enum mmwlan_dpp_pb_result result;
    char ssid[MMWLAN_SSID_MAXLEN];
    char pass[MMWLAN_PASSPHRASE_MAXLEN];
} s_dpp_ctx;

static void dpp_event_cb(const struct mmwlan_dpp_cb_args *evt, void *arg)
{
    (void) arg;
    if (!evt || evt->event != MMWLAN_DPP_EVT_PB_RESULT)
    {
        return;
    }

    s_dpp_ctx.done = true;
    s_dpp_ctx.result = evt->args.pb_result.result;
    s_dpp_ctx.ssid[0] = '\0';
    s_dpp_ctx.pass[0] = '\0';

    if (evt->args.pb_result.result == MMWLAN_DPP_PB_RESULT_SUCCESS &&
        evt->args.pb_result.ssid != NULL &&
        evt->args.pb_result.passphrase != NULL &&
        evt->args.pb_result.ssid_len > 0 &&
        evt->args.pb_result.ssid_len < MMWLAN_SSID_MAXLEN)
    {
        memcpy(s_dpp_ctx.ssid, evt->args.pb_result.ssid, evt->args.pb_result.ssid_len);
        s_dpp_ctx.ssid[evt->args.pb_result.ssid_len] = '\0';
        strncpy(s_dpp_ctx.pass, evt->args.pb_result.passphrase, sizeof(s_dpp_ctx.pass));
        s_dpp_ctx.pass[sizeof(s_dpp_ctx.pass) - 1] = '\0';
    }

    if (s_dpp_sem)
    {
        xSemaphoreGive(s_dpp_sem);
    }
}

void morse_dpp_pb_stop(void)
{
    (void) mmwlan_dpp_stop();
    if (s_dpp_sem)
    {
        xSemaphoreGive(s_dpp_sem);
    }
    s_dpp_running = false;
}

esp_err_t morse_dpp_pb_run(uint32_t timeout_ms)
{
    if (!netModule_is_mmwifi())
    {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (s_dpp_running)
    {
        return ESP_ERR_INVALID_STATE;
    }

    s_dpp_running = true;
    s_dpp_ctx.done = false;
    s_dpp_ctx.result = MMWLAN_DPP_PB_RESULT_ERROR;
    s_dpp_ctx.ssid[0] = '\0';
    s_dpp_ctx.pass[0] = '\0';

    if (!s_dpp_sem)
    {
        s_dpp_sem = xSemaphoreCreateBinary();
    }
    if (!s_dpp_sem)
    {
        s_dpp_running = false;
        return ESP_ERR_NO_MEM;
    }
    (void) xSemaphoreTake(s_dpp_sem, 0);

    /* Free connection mode for DPP to avoid MMWLAN_UNAVAILABLE */
    (void) mm_wifi_disconnect();

    struct mmwlan_dpp_args dpp_args = {
        .dpp_event_cb = dpp_event_cb,
        .dpp_event_cb_arg = NULL,
    };

    enum mmwlan_status st = mmwlan_dpp_start(&dpp_args);
    if (st != MMWLAN_SUCCESS)
    {
        ESP_LOGE(TAG, "mmwlan_dpp_start failed (%d)", st);
        s_dpp_running = false;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "DPP PB started, waiting up to %lu ms", (unsigned long) timeout_ms);
    BaseType_t ok = xSemaphoreTake(s_dpp_sem, pdMS_TO_TICKS(timeout_ms));

    (void) mmwlan_dpp_stop();
    /* Let UMAC/DPP tear down before STA reconnect (reduces pageset -32768 storms). */
    vTaskDelay(pdMS_TO_TICKS(1500));

    if (!ok)
    {
        ESP_LOGW(TAG, "DPP PB timed out");
        s_dpp_running = false;
        return ESP_ERR_TIMEOUT;
    }

    ESP_LOGI(TAG, "DPP PB result=%d", (int) s_dpp_ctx.result);
    if (s_dpp_ctx.result != MMWLAN_DPP_PB_RESULT_SUCCESS ||
        s_dpp_ctx.ssid[0] == '\0' ||
        s_dpp_ctx.pass[0] == '\0')
    {
        s_dpp_running = false;
        return ESP_FAIL;
    }

    wifiAttr_t wifi;
    if (cfg_get_wifi_attr(&wifi) != ESP_OK)
    {
        memset(&wifi, 0, sizeof(wifi));
    }

    strncpy(wifi.ssid, s_dpp_ctx.ssid, sizeof(wifi.ssid));
    wifi.ssid[sizeof(wifi.ssid) - 1] = '\0';
    strncpy(wifi.password, s_dpp_ctx.pass, sizeof(wifi.password));
    wifi.password[sizeof(wifi.password) - 1] = '\0';

    (void) cfg_set_wifi_attr(&wifi);

    ESP_LOGI(TAG, "saved ssid='%s', reconnecting", wifi.ssid);
    esp_err_t rc = wifi_sta_reconnect(wifi.ssid, wifi.password);
    if (rc != ESP_OK)
    {
        ESP_LOGE(TAG, "reconnect failed (0x%x)", (unsigned) rc);
    }

    s_dpp_running = false;
    return rc;
}

static uint32_t parse_timeout_ms(int argc, char **argv)
{
    const uint32_t default_timeout = MORSE_DPP_PB_DEFAULT_TIMEOUT_MS;
    for (int i = 2; i + 1 < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0)
        {
            long v = strtol(argv[i + 1], NULL, 10);
            if (v > 0)
            {
                return (uint32_t) v;
            }
        }
    }
    return default_timeout;
}

static int do_dpp_pb(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage:\n");
        printf("  dpp_pb start [-t timeout_ms]\n");
        printf("  dpp_pb stop\n");
        return ESP_ERR_INVALID_ARG;
    }

    if (strcmp(argv[1], "stop") == 0)
    {
        morse_dpp_pb_stop();
        printf("dpp_pb: stopped\n");
        return ESP_OK;
    }

    if (strcmp(argv[1], "start") != 0)
    {
        printf("dpp_pb: unknown subcommand '%s'\n", argv[1]);
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t timeout_ms = parse_timeout_ms(argc, argv);
    esp_err_t rc = morse_dpp_pb_run(timeout_ms);

    if (rc == ESP_ERR_NOT_SUPPORTED)
    {
        printf("dpp_pb: not supported (not HaLow module)\n");
    }
    else if (rc == ESP_ERR_INVALID_STATE)
    {
        printf("dpp_pb: already running\n");
    }
    else if (rc == ESP_ERR_TIMEOUT)
    {
        printf("dpp_pb: timed out\n");
    }
    else if (rc != ESP_OK)
    {
        printf("dpp_pb: failed (0x%x)\n", (unsigned) rc);
    }
    else
    {
        printf("dpp_pb: success\n");
    }

    return rc;
}

static esp_console_cmd_t g_cmd[] = {
    ESP_CONSOLE_CMD_INIT("dpp_pb", "DPP push-button provisioning (HaLow)", NULL, do_dpp_pb, NULL),
};

void morse_dpp_cli_init(void)
{
    debug_cmd_add(g_cmd, sizeof(g_cmd) / sizeof(g_cmd[0]));
}
