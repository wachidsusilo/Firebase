#ifndef FB_SSL_H
#define FB_SSL_H

#include "Arduino.h"
#include "Log.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ssl.h"

struct fb_ssl_context {
    int socket;
    unsigned long handshake_timeout;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_ctr_drbg_context drbg_ctx;
    mbedtls_entropy_context entropy_ctx;
    fb_ssl_context() {
        ssl_ctx = mbedtls_ssl_context();
        ssl_conf = mbedtls_ssl_config();
        drbg_ctx = mbedtls_ctr_drbg_context();
        entropy_ctx = mbedtls_entropy_context();
    }
};

void fb_ssl_init(fb_ssl_context *ssl_client);
int fb_start_ssl_client(fb_ssl_context *ssl_client, const char *host, uint32_t port, int timeout);
void fb_stop_ssl_socket(fb_ssl_context *ssl_client);
int fb_data_to_read(fb_ssl_context *ssl_client);
int fb_send_ssl_data(fb_ssl_context *ssl_client, const uint8_t *data, uint16_t len);
int fb_get_ssl_receive(fb_ssl_context *ssl_client, uint8_t *data, int length);

#endif
