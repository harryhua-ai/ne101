#ifndef __MQTT_H__
#define __MQTT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "system.h"
#include "camera.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MQTT_CERT_PATH      "/littlefs/mqtts_cert.pem"
#define MQTT_KEY_PATH       "/littlefs/mqtts_key.pem"
#define MQTT_CA_PATH        "/littlefs/mqtts_ca.pem"

// Send buffer size for JSON payload construction (shared with push.c)
#define PUSH_SEND_BUFFER_SIZE  (1536000)

void mqtt_open(void);
void mqtt_close(void);
void mqtt_start(void);
void mqtt_stop(void);
void mqtt_restart(void);
bool mqtt_is_connected(void);
esp_err_t mqtt_publish_node(queueNode_t *node);
char *push_build_json_payload(queueNode_t *node);

#ifdef __cplusplus
}
#endif

#endif /* __MQTT_H__ */
