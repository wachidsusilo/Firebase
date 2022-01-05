#include "ClientSecure.h"

#include <errno.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

#undef connect
#undef write
#undef read

ClientSecure::ClientSecure() {
    _connected = false;

    sslclient = fb_ssl_context();
    fb_ssl_init(&sslclient);
    sslclient.socket = -1;
    sslclient.handshake_timeout = 120000;
}

ClientSecure::~ClientSecure() {
    stop();
}

void ClientSecure::stop() {
    if (sslclient.socket >= 0) {
        close(sslclient.socket);
        sslclient.socket = -1;
        _connected = false;
        _peek = -1;
    }
    fb_stop_ssl_socket(&sslclient);
}

int ClientSecure::connect(const char *host, uint16_t port) {
    int ret = fb_start_ssl_client(&sslclient, host, port, _timeout);
    if (ret < 0) {
        stop();
        return 0;
    }
    _connected = true;
    return 1;
}

int ClientSecure::available() {
    int peeked = (_peek >= 0);
    if (!_connected) {
        return peeked;
    }
    int res = fb_data_to_read(&sslclient);
    if (res < 0) {
        stop();
        return peeked ? peeked : res;
    }
    return res + peeked;
}

size_t ClientSecure::write(const uint8_t *buf, size_t size) {
    if (!_connected) {
        return 0;
    }
    int res = fb_send_ssl_data(&sslclient, buf, size);
    if (res < 0) {
        stop();
        res = 0;
    }
    return res;
}

int ClientSecure::read(uint8_t *buf, size_t size) {
    int peeked = 0;
    int avail = available();
    if ((!buf && size) || avail <= 0) {
        return -1;
    }
    if (!size) {
        return 0;
    }
    if (_peek >= 0) {
        buf[0] = _peek;
        _peek = -1;
        size--;
        avail--;
        if (!size || !avail) {
            return 1;
        }
        buf++;
        peeked = 1;
    }

    int res = fb_get_ssl_receive(&sslclient, buf, size);
    if (res < 0) {
        stop();
        return peeked ? peeked : res;
    }
    return res + peeked;
}

uint8_t ClientSecure::connected() {
    uint8_t dummy = 0;
    read(&dummy, 0);
    return _connected;
}
