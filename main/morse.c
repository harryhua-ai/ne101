#include <stdio.h>
#include <string.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "config.h"
#include "system.h"
#include "utils.h"
#include "mmregdb.h"
#include "mmosal.h"
#include "mmwlan.h"
#include "mmutils.h"
#include "mmhal_os.h"
#include "mmhal_wlan.h"
#include "mmhal_wlan_esp32.h"
#include "mmhalow.h"
#include "morse.h"

#define TAG "-->MORSE"

struct mm_netif_driver {
    esp_netif_driver_base_t base;
    void *handle;
};

typedef struct mm_netif_driver *mm_netif_driver_t;

struct mm_scan_args {
    struct mm_scan_result *result;
    struct mmosal_semb *semaphore;
};

#define MM_WIFI_DOMAIN_CODE_MAXLEN 12

struct mm_wifi_config {
    char ssid[MMWLAN_SSID_MAXLEN];
    char password[MMWLAN_PASSPHRASE_MAXLEN];
    char country_code[MM_WIFI_DOMAIN_CODE_MAXLEN];
    esp_netif_t *netif;
};

struct mm_wifi_config g_mm_wifi_config = {
    .ssid = "morse",
    .password = "12345678",
    .country_code = "EU"
};

static esp_timer_handle_t s_connect_timer;
static bool s_connect_pending;

/* Forward declarations (used by mmwlan init helper). */
static void wifi_rx_cb(struct mmpkt *rxpkt, void *arg);
static void wifi_link_state_cb(enum mmwlan_link_state link_state, void *arg);

static void mm_wifi_iso_country_code(const char *domain_code, char iso_code[3])
{
    if (domain_code == NULL || domain_code[0] == '\0') {
        iso_code[0] = '\0';
        return;
    }

    if (strncmp(domain_code, "AU-", 3) == 0) {
        strncpy(iso_code, "AU", 3);
        return;
    }

    iso_code[0] = domain_code[0];
    iso_code[1] = domain_code[1];
    iso_code[2] = '\0';
}

static bool mm_wifi_is_valid_domain_code(const char *domain_code)
{
    return domain_code != NULL && mmregdb_lookup_domain(domain_code) != NULL;
}

static void mm_wifi_select_bcf_by_country(const char *domain_code)
{
    char iso_code[3] = { 0 };

    /* Select embedded BCF BEFORE mmwlan_init()/mmwlan_boot(). */
    extern void mmhal_wlan_select_bcf_by_country(const char *country_code);
    mm_wifi_iso_country_code(domain_code, iso_code);
    mmhal_wlan_select_bcf_by_country(iso_code);
}

static esp_err_t mm_wifi_mmwlan_init_and_boot(esp_netif_t *esp_netif,
                                             const uint8_t *mac_addr,
                                             const char *country_code,
                                             bool print_versions)
{
    enum mmwlan_status status;
    struct mmwlan_boot_args boot_args = MMWLAN_BOOT_ARGS_INIT;

    mm_wifi_select_bcf_by_country(country_code);

    mmwlan_init();
    mm_wifi_set_mac((uint8_t *)mac_addr);

    status = mmwlan_register_rx_pkt_cb(wifi_rx_cb, esp_netif);
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to register %s callback", "rx");
        return ESP_FAIL;
    }

    status = mmwlan_register_link_state_cb(wifi_link_state_cb, esp_netif);
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to register %s callback", "link state");
        return ESP_FAIL;
    }

    const struct mmwlan_s1g_channel_list *channel_list = mmregdb_lookup_domain(country_code);
    if (channel_list == NULL) {
        ESP_LOGE(TAG, "Could not find specified regulatory domain matching country code %s", country_code);
        return ESP_FAIL;
    }

    status = mmwlan_set_channel_list(channel_list);
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to set country code %s", channel_list->country_code);
        return ESP_FAIL;
    }

    status = mmwlan_boot(&boot_args);
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Boot failed with code %d", status);
        return ESP_FAIL;
    }

    mmwlan_set_power_save_mode(MMWLAN_PS_DISABLED);
    if (print_versions) {
        mmhalow_print_version_info();
    }

    return ESP_OK;
}

/*--------------------------------------------wifi interface-----------------------------------------*/
static void wifi_free(void *h, void *buffer)
{
    struct mmpktview *pktview = (struct mmpktview *)buffer;
    struct mmpkt *rxpkt = mmpkt_from_view(pktview);
    mmpkt_close(&pktview);
    mmpkt_release(rxpkt);
}

static esp_err_t wifi_transmit(void *h, void *buffer, size_t len)
{
    struct mmpkt *pkt;
    struct mmpktview *pktview;
    enum mmwlan_status status;
    struct mmwlan_tx_metadata metadata = {
        .tid = 0,
    };

    status = mmwlan_tx_wait_until_ready(1000);
    if (status != MMWLAN_SUCCESS)
    {
        ESP_LOGE(TAG, "Transmit blocked: %d", status);
        return ESP_FAIL;
    }

    pkt = mmwlan_alloc_mmpkt_for_tx(len, metadata.tid);
    if (pkt == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate packet for transmit.");
        return ESP_ERR_NO_MEM;
    }
    pktview = mmpkt_open(pkt);
    mmpkt_append_data(pktview, (const uint8_t *)buffer, len);
    mmpkt_close(&pktview);

    status = mmwlan_tx_pkt(pkt, &metadata);
    if (status != MMWLAN_SUCCESS)
    {
        ESP_LOGE(TAG, "Packet failed to send - %d", status);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t wifi_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf)
{
    return wifi_transmit(h, buffer, len);
}

static esp_err_t wifi_driver_start(esp_netif_t *esp_netif, void *args)
{
    mm_netif_driver_t driver = args;
    driver->base.netif = esp_netif;
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle =  driver,
        .transmit = wifi_transmit,
        .transmit_wrap = wifi_transmit_wrap,
        .driver_free_rx_buffer = wifi_free
    };

    return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
}

static mm_netif_driver_t wifi_create_if_driver()
{
    mm_netif_driver_t driver = calloc(1, sizeof(struct mm_netif_driver));
    if (driver == NULL) {
        ESP_LOGE(TAG, "No memory to create a wifi interface handle");
        return NULL;
    }
    driver->handle = NULL; // TODO
    driver->base.post_attach = wifi_driver_start;
    return driver;
}

static void wifi_destroy_if_driver(mm_netif_driver_t driver)
{
    if (driver) {
        free(driver);
    }
}

static esp_err_t disconnect_and_destroy(esp_netif_t *esp_netif)
{
    mm_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);
    esp_netif_driver_ifconfig_t driver_ifconfig = { };
    esp_err_t  ret = esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
    wifi_destroy_if_driver(driver);
    return ret;
}

static esp_err_t wifi_clear_default_sta_handlers(void)
{
    return ESP_OK;
}

static esp_err_t wifi_set_default_sta_handlers(void)
{
    return ESP_OK;
}

static void wifi_rx_cb(struct mmpkt *rxpkt, void *arg){
    esp_netif_t *esp_netif = (esp_netif_t *)arg;
    assert(esp_netif);
    struct mmpktview *pktview = mmpkt_open(rxpkt);
    uint32_t data_len = mmpkt_get_data_length(pktview);
    esp_err_t ret = esp_netif_receive(esp_netif, mmpkt_get_data_start(pktview), data_len, pktview);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_netif_receive failed - %d", ret);
        mmpkt_close(&pktview);
        mmpkt_release(rxpkt);
    }
    // ESP_LOGI(TAG, "--------->Received %d bytes from morse", head_size + len);
}

static void wifi_link_state_cb(enum mmwlan_link_state link_state, void *arg)
{
    esp_netif_t *esp_netif = (esp_netif_t *)arg;
    if (link_state == MMWLAN_LINK_DOWN) {
        ESP_LOGI(TAG, "Link down");
        esp_netif_action_disconnected(esp_netif, NULL, 0, NULL);
        ESP_ERROR_CHECK(esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, NULL, 0, portMAX_DELAY));
    } else {
        ESP_LOGI(TAG, "Link up");
        s_connect_pending = false;
        if (s_connect_timer != NULL) {
            esp_timer_stop(s_connect_timer);
        }
        esp_netif_action_connected(esp_netif, NULL, 0, NULL);
        ESP_ERROR_CHECK(esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, NULL, 0, portMAX_DELAY));
    }
}

static void wifi_sta_state_cb(enum mmwlan_sta_state sta_state)
{
    switch (sta_state) {
        case MMWLAN_STA_DISABLED:
            ESP_LOGI(TAG, "Disconnected");
            // esp_netif_action_stop(esp_netif, NULL, 0, NULL);
            ESP_ERROR_CHECK(esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_STOP, NULL, 0, portMAX_DELAY));
            break;
        case MMWLAN_STA_CONNECTING:
            ESP_LOGI(TAG, "Connecting");
            break;
        case MMWLAN_STA_CONNECTED:
            ESP_LOGI(TAG, "Connected");
            ESP_ERROR_CHECK(esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_START, NULL, 0, portMAX_DELAY));
            break;
        default:
            break;
    }
}

static void wifi_scan_rx_cb(const struct mmwlan_scan_result *result, void *arg)
{
    struct mm_scan_args *args = (struct mm_scan_args *)arg;
    struct scan_item *item = NULL;
    struct mm_rsn_information rsn_info = { 0 };

    char bssid_str[18];
    char ssid_str[MMWLAN_SSID_MAXLEN];

    if (args->result->items_count > MAX_SCAN_ITEM_COUNT) {
        ESP_LOGE(TAG, "Too many scan results");
        return;
    }
    // deduplicate
    for (int i = 0; i < args->result->items_count; i++) {
        if (memcmp(result->bssid, args->result->items[i].bssid, MMWLAN_MAC_ADDR_LEN) == 0) {
            return;
        }
    }

    item = &args->result->items[args->result->items_count];
    memcpy(item->bssid, result->bssid, MMWLAN_MAC_ADDR_LEN),
           snprintf(item->ssid, (result->ssid_len + 1), "%s", result->ssid);
    item->rssi = result->rssi;
    snprintf(bssid_str, sizeof(bssid_str), "%02x:%02x:%02x:%02x:%02x:%02x",
             result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3],
             result->bssid[4], result->bssid[5]);
    snprintf(ssid_str, (result->ssid_len + 1), "%s", result->ssid);

    int ret = mm_parse_rsn_information(result->ies, result->ies_len, &rsn_info);
    if (ret < 0) {
        item->authmode = 0;
    } else {
        item->authmode = (rsn_info.num_akm_suites > 0 && rsn_info.akm_suites[0] == MM_AKM_SUITE_SAE) ? 1 : 0;
    }

    const char *sec_primary = (rsn_info.num_akm_suites > 0) ? mm_akm_suite_to_string(rsn_info.akm_suites[0]) : "None";
    const char *sec_secondary = (rsn_info.num_akm_suites > 1) ? mm_akm_suite_to_string(rsn_info.akm_suites[1]) : "";

    ESP_LOGI(TAG, "\n"
        "%.*s" "\n"
        "\t" "Operating BW: %u MHz" "\n"
        "\t" "BSSID: %s" "\n"
        "\t" "RSSI: %3d" "\n"
        "\t" "Beacon Interval (TUs): %u" "\n"
        "\t" "Capability Info: 0x%04x" "\n"
        "\t" "Security: %s %s" "\n",
        result->ssid_len, result->ssid,
        result->op_bw_mhz,
        bssid_str,
        result->rssi,
        result->beacon_interval,
        result->capability_info,
        sec_primary,
        sec_secondary
    );

    args->result->items_count++;
}

static void wifi_scan_completed_cb(enum mmwlan_scan_state state, void *arg)
{
    struct mm_scan_args *args = (struct mm_scan_args *)arg;
    (void)(state);
    ESP_LOGI(TAG, "Scanning completed.");
    mmosal_semb_give(args->semaphore);
}

/*--------------------------------------------netif interface------------------------- ----------------*/

static esp_err_t netif_attach_wifi_station(esp_netif_t *esp_netif)
{
    mm_netif_driver_t driver = wifi_create_if_driver();
    if (driver == NULL) {
        ESP_LOGE(TAG, "Failed to create wifi interface handle");
        return ESP_FAIL;
    }
    return esp_netif_attach(esp_netif, driver);
}

/*--------------------------------------------external interface-----------------------------------------*/

esp_netif_t *mm_netif_create_default_wifi_sta(void)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    ESP_ERROR_CHECK(netif_attach_wifi_station(netif));
    ESP_ERROR_CHECK(wifi_set_default_sta_handlers());
    return netif;
}

void mm_netif_destroy_wifi_sta(esp_netif_t *esp_netif)
{
    if (esp_netif) {
        wifi_clear_default_sta_handlers();
        disconnect_and_destroy(esp_netif);
    }
    esp_netif_destroy(esp_netif);
}

esp_err_t mm_wifi_init(esp_netif_t *esp_netif, uint8_t *mac_addr, const char *country_code)
{
    if (!mm_wifi_is_valid_domain_code(country_code)) {
        ESP_LOGE(TAG, "Invalid country code: %s", country_code);
        return ESP_FAIL;
    }

    mmhal_init();
    if (mm_wifi_mmwlan_init_and_boot(esp_netif, mac_addr, country_code, true) != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_netif_set_mac(esp_netif, mac_addr));
    ESP_LOGI(TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    g_mm_wifi_config.netif = esp_netif;
    strncpy(g_mm_wifi_config.country_code, country_code, sizeof(g_mm_wifi_config.country_code));
    g_mm_wifi_config.country_code[sizeof(g_mm_wifi_config.country_code) - 1] = '\0';
    ESP_LOGI(TAG, "initialized OK");
    esp_netif_action_start(esp_netif, NULL, 0, NULL);
    return ESP_OK;
}

esp_err_t mm_wifi_deinit(void)
{
    (void)mmwlan_shutdown();
    mmwlan_deinit();
    return ESP_OK;
}

void mm_wifi_shutdown()
{
    mm_wifi_deinit();
    mmhal_wlan_shutdown();
}

esp_err_t mm_wifi_set_config(const char *ssid, const char *password)
{
    if (ssid == NULL || password == NULL) {
        ESP_LOGE(TAG, "Invalid parameter");
        return ESP_FAIL;
    }
    if (strlen(ssid)) {
        strncpy(g_mm_wifi_config.ssid, ssid, MMWLAN_SSID_MAXLEN);
    }
    if (strlen(password)) {
        strncpy(g_mm_wifi_config.password, password, MMWLAN_PASSPHRASE_MAXLEN);
    } else {
        memset(g_mm_wifi_config.password, 0, MMWLAN_PASSPHRASE_MAXLEN);
    }

    return ESP_OK;
}

esp_err_t mm_wifi_scan(mm_scan_result_t *result)
{
    static bool scanning = false;
    enum mmwlan_status status;
    struct mmwlan_scan_req scan_req = MMWLAN_SCAN_REQ_INIT;
    struct mm_scan_args scan_args = {0};

    if (scanning) {
        ESP_LOGE(TAG, "Already scanning");
        return ESP_FAIL;
    }
    scanning = true;
    mmwlan_scan_abort(); // abort previous scan

    scan_req.scan_rx_cb = wifi_scan_rx_cb;
    scan_req.scan_complete_cb = wifi_scan_completed_cb;
    scan_req.scan_cb_arg = &scan_args;
    scan_args.result = result;
    scan_args.semaphore = mmosal_semb_create("scan");
    result->items_count = 0;

    status = mmwlan_scan_request(&scan_req);
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to scan: %d", status);
        mmwlan_scan_abort();
        mmosal_semb_delete(scan_args.semaphore);
        scanning = false;
        return ESP_FAIL;
    }

    mmosal_semb_wait(scan_args.semaphore, 30000); // 30s
    mmosal_semb_delete(scan_args.semaphore);
    scanning = false;
    return ESP_OK;
}

esp_err_t mm_wifi_get_mac(uint8_t *mac)
{
    mmhal_read_mac_addr(mac);
    return ESP_OK;
}

esp_err_t mm_wifi_set_mac(uint8_t *mac)
{
    mmhal_write_mac_addr(mac);
    return ESP_OK;
}

esp_err_t mm_wifi_set_country_code(const char *country_code)
{
    if (!mm_wifi_is_valid_domain_code(country_code)) {
        ESP_LOGE(TAG, "Invalid country code: %s", country_code);
        return ESP_FAIL;
    }

    if (strcmp(country_code, g_mm_wifi_config.country_code) == 0) {
        ESP_LOGI(TAG, "Country code already set to %s", country_code);
        return ESP_OK;
    }

    /*
     * Country code affects regdb selection and (when dual-BCF is enabled) which BCF is presented
     * to morselib. BCF is read during mmwlan_init()/boot, but esp_netif must NOT be re-added.
     * So we restart only the mmwlan stack (shutdown/deinit/init/boot), not mmhal_init/netif.
     */
    const struct mmwlan_s1g_channel_list *channel_list = mmregdb_lookup_domain(country_code);
    if (channel_list == NULL) {
        ESP_LOGE(TAG, "Could not find specified regulatory domain matching country code %s",
                 country_code);
        return ESP_FAIL;
    }

    /* Ensure STA is stopped before restarting the chip. */
    (void)mm_wifi_disconnect();

    if (mmwlan_shutdown() != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to shutdown");
        return ESP_FAIL;
    }
    mmwlan_deinit();

    esp_netif_t *netif = g_mm_wifi_config.netif;
    if (netif == NULL) {
        ESP_LOGE(TAG, "Cannot change country code: netif not initialized");
        return ESP_FAIL;
    }

    uint8_t mac_addr[6] = { 0 };
    if (esp_netif_get_mac(netif, mac_addr) != ESP_OK) {
        ESP_LOGW(TAG, "Failed to get netif MAC; falling back to mmhal MAC");
        mm_wifi_get_mac(mac_addr);
    }

    /*
     * Re-init mmwlan stack with new BCF selection.
     * Do not print versions here to avoid UI spam.
     */
    if (mm_wifi_mmwlan_init_and_boot(netif, mac_addr, country_code, true) != ESP_OK) {
        return ESP_FAIL;
    }

    strncpy(g_mm_wifi_config.country_code, country_code, sizeof(g_mm_wifi_config.country_code));
    g_mm_wifi_config.country_code[sizeof(g_mm_wifi_config.country_code) - 1] = '\0';
    ESP_LOGI(TAG, "Set country code to %s", country_code);

    return ESP_OK;
}

esp_err_t mm_wifi_get_country_code(char *country_code)
{
    strncpy(country_code, g_mm_wifi_config.country_code, MM_WIFI_DOMAIN_CODE_MAXLEN);
    country_code[MM_WIFI_DOMAIN_CODE_MAXLEN - 1] = '\0';
    return ESP_OK;
}

static void mm_wifi_connect_timer_stop(void)
{
    s_connect_pending = false;
    if (s_connect_timer != NULL) {
        esp_timer_stop(s_connect_timer);
    }
}

static void mm_wifi_connect_timeout_cb(void *arg)
{
    (void)arg;
    if (!s_connect_pending) {
        return;
    }
    s_connect_pending = false;

    ESP_LOGE(TAG, "Connect timeout after %d ms", MM_WIFI_CONNECT_TIMEOUT_MS);
    mmwlan_sta_disable();

    esp_netif_t *netif = g_mm_wifi_config.netif;
    if (netif != NULL) {
        esp_netif_action_disconnected(netif, NULL, 0, NULL);
    }
    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, NULL, 0, portMAX_DELAY);
}

static esp_err_t mm_wifi_connect_timer_start(void)
{
    if (s_connect_timer == NULL) {
        const esp_timer_create_args_t timer_args = {
            .callback = mm_wifi_connect_timeout_cb,
            .name = "mm_connect",
        };
        esp_err_t err = esp_timer_create(&timer_args, &s_connect_timer);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create connect timer: %s", esp_err_to_name(err));
            return err;
        }
    } else {
        esp_timer_stop(s_connect_timer);
    }

    s_connect_pending = true;
    return esp_timer_start_once(s_connect_timer, (uint64_t)MM_WIFI_CONNECT_TIMEOUT_MS * 1000);
}

esp_err_t mm_wifi_connect()
{
    struct mmwlan_sta_args sta_args = MMWLAN_STA_ARGS_INIT;
    enum mmwlan_status status;

    char *ssid = g_mm_wifi_config.ssid;
    char *password = g_mm_wifi_config.password;

    mm_wifi_connect_timer_stop();

    sta_args.ssid_len = strlen(ssid);
    memcpy(sta_args.ssid, ssid, sta_args.ssid_len);
    ESP_LOGI(TAG, "Connecting to %s", ssid);

    sta_args.passphrase_len = strlen(password);
    if (sta_args.passphrase_len > 0) {
        memcpy(sta_args.passphrase, password, sta_args.passphrase_len);
        sta_args.security_type = MMWLAN_SAE;
    } else {
        sta_args.security_type = MMWLAN_OPEN;
    }

    status = mmwlan_sta_enable(&sta_args, wifi_sta_state_cb);
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to enable station mode: %d", status);
        return ESP_FAIL;
    }

    esp_err_t err = mm_wifi_connect_timer_start();
    if (err != ESP_OK) {
        mmwlan_sta_disable();
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mm_wifi_disconnect()
{
    mm_wifi_connect_timer_stop();
    enum mmwlan_status status = mmwlan_sta_disable();
    if (status != MMWLAN_SUCCESS) {
        ESP_LOGE(TAG, "Failed to disable station mode: %d", status);
        return ESP_FAIL;
    }
    return ESP_OK;
}
