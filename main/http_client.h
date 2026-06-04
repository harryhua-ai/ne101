#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct http_header_s {
    char *key;
    char *value;
} http_header_t;

typedef struct http_s {
    char *url;
    char *method;
    char *body;
    int timeout;
    http_header_t *headers;
    int header_cnt;
    char **resp;
} http_t;

typedef struct OTApackage {
    char fwTitle[32];
    char fwChecksum[32];
    char cfTitle[32];
    char cfChecksum[32];
} OTApackage_t;

void http_client_check_update();
esp_err_t http_client_sync_server_time();
int8_t http_client_send_req(http_t *http);
int8_t http_client_download_file(const char *url, const char *filename, int timeout, int filesize, const char *md5,
                                 const char *crc32);
int8_t http_client_upload_file(const char *url, const char *filename, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* __HTTP_CLIENT_H__ */
