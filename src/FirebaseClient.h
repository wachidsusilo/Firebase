#ifndef FIREBASECLIENT_H
#define FIREBASECLIENT_H

#include "Arduino.h"
#include "ClientSecure.h"
#include "FirebaseAuth.h"
#include "FirebaseConstant.h"
#include "Json.h"
#include "Log.h"
#include "WiFi.h"

class FC {
   public:
    using DBHandler = void (*)(Json);
    struct request_data_t {
        String server;
        String path;
        String payload;
        const char *method;
        DBHandler handler;

        request_data_t(const String &server, const String &path, const String &payload, const char *method, DBHandler handler = NULL)
            : server(server), path(path), payload(payload), method(method), handler(handler) {}
        size_t size() {
            return server.length() + path.length() + payload.length() + strlen(method) + sizeof(DBHandler);
        }
    };

    FC();
    bool GET(const String &server, const String &path, DBHandler handler = NULL);
    bool PUT(const String &server, const String &path, const String &payload, DBHandler handler = NULL);
    bool POST(const String &server, const String &path, const String &payload, DBHandler handler = NULL);
    bool PATCH(const String &server, const String &path, const String &payload, DBHandler handler = NULL);
    bool DELETE(const String &server, const String &path, const String &payload, DBHandler handler = NULL);

   private:
    static QueueHandle_t queue;
    static void request(void *);
};

extern FC FirebaseClient;

#endif
