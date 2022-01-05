#include "Database.h"

RTDB &RTDB::database(const String &dbUrl) {
    host = dbUrl;
    paths = "";
    payloads = "";
    return *this;
}

RTDB &RTDB::path(const String &pathName) {
    paths = pathName;
    if (!paths.startsWith("/")) {
        paths = "/" + paths;
    }
    if (!paths.endsWith(".json")) {
        paths = paths + ".json";
    }
    return *this;
}

RTDB &RTDB::payload(Json &json) {
    payloads = json.toString();
    return *this;
}

bool RTDB::get(DBHandler handler) {
    return FirebaseClient.GET(host, paths + (!handler ? fb_sil : ""), handler);
}

bool RTDB::set(DBHandler handler) {
    return FirebaseClient.PUT(host, paths + (!handler ? fb_sil : ""), payloads, handler);
}

bool RTDB::push(DBHandler handler) {
    return FirebaseClient.POST(host, paths + (!handler ? fb_sil : ""), payloads, handler);
}

bool RTDB::update(DBHandler handler) {
    return FirebaseClient.PATCH(host, paths + (!handler ? fb_sil : ""), payloads, handler);
}

bool RTDB::remove(DBHandler handler) {
    return FirebaseClient.DELETE(host, paths + (!handler ? fb_sil : ""), payloads, handler);
}

void RTDB::listen(DBHandler handler) {
    static RTDB::db_stream_t data;
    data.server = host;
    data.path = paths;
    data.handler = handler;
    xTaskCreate(streamTask, "stream", 8192, &data, 1, NULL);
}

void RTDB::streamTask(void *vParam) {
    ClientSecure client;
    RTDB::db_stream_t data = *(static_cast<RTDB::db_stream_t *>(vParam));
    while (true) {
        if (WiFi.isConnected() && !FirebaseAuth.isExpired() && client.connect(data.server.c_str(), 443)) {
            client.println(fb_get + data.path + fb_ver);
            client.println(fb_hos + data.server);
            client.println(fb_uag);
            client.println(fb_kea);
            client.println(fb_ach);
            client.println(fb_aut + FirebaseAuth.getAccessToken());
            client.println();
            while (client.connected()) {
                delay(100);
                if (client.available()) {
                    delay(100);
                    String response = client.readString();
                    if (response.length() > 0) {
                        bool isChunked = response.indexOf(fb_chn) > 0;
                        if (response.indexOf("Content-Type") >= 0) {
                            response = response.substring(response.indexOf("\r\n\r\n"));
                        }
                        response.trim();
                        if (isChunked) {
                            response = response.substring(response.indexOf("\r\n"), response.indexOf("\r\n0"));
                        }
                        response.trim();
                        response = response.substring(response.indexOf("data:") + 6);
                        response.trim();
                        if (response.equals("null") || response.isEmpty()) continue;
                        if (!response.startsWith("{") || !response.endsWith("}")) {
                            response = "{\"response\":" + response + "}";
                        }
                        if (data.handler) data.handler(Json(response));
                    }
                }
            }
        }
        delay(1000);
    }
}
