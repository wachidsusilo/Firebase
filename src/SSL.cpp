#include "SSL.h"

#include "algorithm"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"
#include "mbedtls/net.h"
#include "mbedtls/oid.h"
#include "mbedtls/platform.h"
#include "mbedtls/sha256.h"
#include "string"

void fb_ssl_init(fb_ssl_context *ssl_client) {
    mbedtls_ssl_init(&ssl_client->ssl_ctx);
    mbedtls_ssl_config_init(&ssl_client->ssl_conf);
    mbedtls_ctr_drbg_init(&ssl_client->drbg_ctx);
}

int fb_start_ssl_client(fb_ssl_context *ssl_client, const char *host, uint32_t port, int timeout) {
    char buf[512];
    int ret, flags;
    int enable = 1;

    ssl_client->socket = -1;
    ssl_client->socket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ssl_client->socket < 0) {
        return ssl_client->socket;
    }

    ip_addr_t ip;
    int t_out = 0;
    while (!ip.u_addr.ip4.addr) {
        dns_gethostbyname(host, &ip, NULL, NULL);
        delay(100);
        if (++t_out > 10) return -1;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ip.u_addr.ip4.addr;
    serv_addr.sin_port = htons(port);

    if (lwip_connect(ssl_client->socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
        if (timeout <= 0) {
            timeout = 30000;
        }
        lwip_setsockopt(ssl_client->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        lwip_setsockopt(ssl_client->socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        lwip_setsockopt(ssl_client->socket, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
        lwip_setsockopt(ssl_client->socket, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
    } else {
#if FB_SSL_DEBUG
        fb_log.e(String(F("SSL")), String(F("Connection failed")));
#endif
        return -1;
    }

    const char *pers = "esp32-tls";
    fcntl(ssl_client->socket, F_SETFL, fcntl(ssl_client->socket, F_GETFL, 0) | O_NONBLOCK);
    mbedtls_entropy_init(&ssl_client->entropy_ctx);
    ret = mbedtls_ctr_drbg_seed(&ssl_client->drbg_ctx, mbedtls_entropy_func, &ssl_client->entropy_ctx, (const unsigned char *)pers, strlen(pers));
    if (ret < 0) {
#if FB_SSL_DEBUG
        fb_log.e(String(F("SSL")), String(F("Failed to construct DRBG seed (err_code: -0x")) + String(abs(ret), HEX) + ")");
#endif
        return -1;
    }

    if ((ret = mbedtls_ssl_config_defaults(&ssl_client->ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
#if FB_SSL_DEBUG
        fb_log.e(String(F("SSL")), String(F("Failed to construct configuration (err_code: -0x")) + String(abs(ret), HEX) + ")");
#endif
        return -1;
    }

    mbedtls_ssl_conf_authmode(&ssl_client->ssl_conf, MBEDTLS_SSL_VERIFY_NONE);

    if ((ret = mbedtls_ssl_set_hostname(&ssl_client->ssl_ctx, host)) != 0) {
#if FB_SSL_DEBUG
        fb_log.e(String(F("SSL")), String(F("Failed to set hostname (err_code: -0x")) + String(abs(ret), HEX) + ")");
#endif
        return -1;
    }

    mbedtls_ssl_conf_rng(&ssl_client->ssl_conf, mbedtls_ctr_drbg_random, &ssl_client->drbg_ctx);

    if ((ret = mbedtls_ssl_setup(&ssl_client->ssl_ctx, &ssl_client->ssl_conf)) != 0) {
#if FB_SSL_DEBUG
        fb_log.e(String(F("SSL")), String(F("Setup failed (err_code: -0x")) + String(abs(ret), HEX) + ")");
#endif
        return -1;
    }

    mbedtls_ssl_set_bio(&ssl_client->ssl_ctx, &ssl_client->socket, mbedtls_net_send, mbedtls_net_recv, NULL);

    unsigned long handshake_start_time = millis();
    while ((ret = mbedtls_ssl_handshake(&ssl_client->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
#if FB_SSL_DEBUG
            fb_log.e(String(F("SSL")), String(F("Handshake failed (err_code: -0x")) + String(abs(ret), HEX) + ")");
#endif
            return -1;
        }
        if ((millis() - handshake_start_time) > ssl_client->handshake_timeout) {
#if FB_SSL_DEBUG
            fb_log.e(String(F("SSL")), String(F("Handshake timeout")));
#endif
            return -1;
        }
        delay(10);
    }

    return ssl_client->socket;
}

void fb_stop_ssl_socket(fb_ssl_context *ssl_client) {
    if (ssl_client->socket >= 0) {
        close(ssl_client->socket);
        ssl_client->socket = -1;
    }

    mbedtls_ssl_free(&ssl_client->ssl_ctx);
    mbedtls_ssl_config_free(&ssl_client->ssl_conf);
    mbedtls_ctr_drbg_free(&ssl_client->drbg_ctx);
    mbedtls_entropy_free(&ssl_client->entropy_ctx);
}

int fb_data_to_read(fb_ssl_context *ssl_client) {
    int ret, res;
    ret = mbedtls_ssl_read(&ssl_client->ssl_ctx, NULL, 0);
    res = mbedtls_ssl_get_bytes_avail(&ssl_client->ssl_ctx);
    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret < 0) {
        return -1;
    }
    return res;
}

int fb_send_ssl_data(fb_ssl_context *ssl_client, const uint8_t *data, uint16_t len) {
    int ret = -1;
    while ((ret = mbedtls_ssl_write(&ssl_client->ssl_ctx, data, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            return -1;
        }
    }
    len = ret;
    return ret;
}

int fb_get_ssl_receive(fb_ssl_context *ssl_client, uint8_t *data, int length) {
    int ret = -1;
    ret = mbedtls_ssl_read(&ssl_client->ssl_ctx, data, length);
    return ret;
}
