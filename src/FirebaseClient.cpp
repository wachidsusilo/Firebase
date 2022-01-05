#include "FirebaseClient.h"

QueueHandle_t FC::queue;

/*------ FirebaseClient ------*/
FC::FC() {
    queue = xQueueCreate(10, sizeof(request_data_t *));
    xTaskCreate(request, "request", 8192 * 2, NULL, 1, NULL);
}

bool FC::GET(const String &server, const String &path, DBHandler handler) {
    request_data_t *data = new request_data_t(server, path, String(), fb_get, handler);
    bool sent = xQueueSend(queue, &data, 1000) == pdTRUE;
    if (!sent) delete data;
    fb_log.i("TCP", sent ? "Request queued" : "Queue is full");
    return data && sent;
}

bool FC::PUT(const String &server, const String &path, const String &payload, DBHandler handler) {
    request_data_t *data = new request_data_t(server, path, payload, fb_put, handler);
    bool sent = xQueueSend(queue, &data, 1000) == pdTRUE;
    if (!sent) delete data;
    fb_log.i("TCP", sent ? "Request queued" : "Queue is full");
    return data && sent;
}

bool FC::POST(const String &server, const String &path, const String &payload, DBHandler handler) {
    request_data_t *data = new request_data_t(server, path, payload, fb_pos, handler);
    bool sent = xQueueSend(queue, &data, 1000) == pdTRUE;
    if (!sent) delete data;
    fb_log.i("TCP", sent ? "Request queued" : "Queue is full");
    return data && sent;
}

bool FC::PATCH(const String &server, const String &path, const String &payload, DBHandler handler) {
    request_data_t *data = new request_data_t(server, path, payload, fb_pat, handler);
    bool sent = xQueueSend(queue, &data, 1000) == pdTRUE;
    if (!sent) delete data;
    fb_log.i("TCP", sent ? "Request queued" : "Queue is full");
    return data && sent;
}

bool FC::DELETE(const String &server, const String &path, const String &payload, DBHandler handler) {
    request_data_t *data = new request_data_t(server, path, payload, fb_del, handler);
    bool sent = xQueueSend(queue, &data, 1000) == pdTRUE;
    if (!sent) delete data;
    fb_log.i("TCP", sent ? "Request queued" : "Queue is full");
    return data && sent;
}

void FC::request(void *) {
    while (true) {
        delay(100);
        request_data_t *data = NULL;
        if (uxQueueMessagesWaiting(queue) > 0) xQueueReceive(queue, &data, 0);
        if (!data) continue;
        fb_log.i("TCP", "Executing request");
        ClientSecure client;
        int retry = 0;
        bool isSucceed = false;
        while (++retry < 10 && !isSucceed && client.connect(data->server.c_str(), 443)) {
            fb_log.i("TCP", "Connected to '" + data->server + "'");
            client.print(data->method + data->path + fb_ver + fb_crl + fb_hos + data->server + fb_crl + fb_uag + fb_crl + fb_con + fb_crl + (strcmp(data->method, fb_get) ? String(fb_cot) + fb_crl + fb_col + data->payload.length() + fb_crl : "") + fb_aut + FirebaseAuth.getAccessToken() + fb_crl + fb_crl + (data->payload.length() > 0 ? data->payload + fb_crl : ""));
            if (!data->handler) isSucceed = true;
            int timeout = 0;
            while (client.connected() && !isSucceed) {
                if (++timeout > 150) {
                    client.stop();
                    break;
                }
                delay(100);
                if (client.available()) {
                    String response = client.readString();
                    if (response.length() > 0) {
                        bool isChunked = response.indexOf(fb_chn) > 0;
                        response = response.substring(response.indexOf("\r\n\r\n"));
                        response.trim();
                        if (isChunked) {
                            response = response.substring(response.indexOf("\r\n"), response.indexOf("\r\n0"));
                        }
                        response.trim();
                        if (response.isEmpty()) {
                            response = "null";
                        }
                        if (!response.startsWith("{") || !response.endsWith("}")) {
                            response = "{\"response\":" + response + "}";
                        }
                        client.stop();
                        if (data->handler) data->handler(Json(response));
                        isSucceed = true;
                        break;
                    }
                }
            }
        }
        if (data->handler && !isSucceed) data->handler(Json("{\"error\":\"Connection timeout\"}"));
        if (!isSucceed) {
            fb_log.i("TCP", "Connection timeout after " + String(retry) + " retries");
        } else {
            fb_log.i("TCP", "Connection successful after " + String(retry) + " retries");
        }
        delete data;
    }
}

FC FirebaseClient;
