#ifndef CLIENTSECURE_H
#define CLIENTSECURE_H

#include "Arduino.h"
#include "SSL.h"
#include "WiFi.h"

class ClientSecure : public WiFiClient {
   protected:
    fb_ssl_context sslclient;

    int _lastError = 0;
    int _peek = -1;
    int _timeout = 0;

   public:
    ClientSecure();
    ~ClientSecure();
    int connect(const char *host, uint16_t port);
    int available();
    size_t write(const uint8_t *buf, size_t size);
    int read(uint8_t *buf, size_t size);
    void stop();
    uint8_t connected();

   private:
    char *_streamLoad(Stream &stream, size_t size);
    using Print::write;
};

#endif
