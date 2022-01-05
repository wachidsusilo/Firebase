#include "FirebaseAuth.h"

String FAUTH::token;
String FAUTH::email;
String FAUTH::key;
long FAUTH::ttl;

/*------ FreeRTOS Task ------*/
void FAUTH::authTask(void *parameter) {
    while (true) {
        if (ttl > 0) {
            ttl -= 5;
        }
        if (WiFi.isConnected() && ttl <= 600 && NTP.getUnixTime() > 1600000000) {
            String payload = FirebaseJWT.encode(email, key, fb_scp, NTP.getUnixTime());
            if (payload.length() > 0) {
                payload = fb_plh + payload + "\"}";
                ClientSecure client;
                if (client.connect(fb_gap, 443)) {
                    client.print(String(fb_pau) + fb_crl + fb_gap + fb_crl + fb_uag + fb_crl + fb_con + fb_crl + fb_cot + fb_crl + fb_col + payload.length() + fb_crl + fb_crl + payload + fb_crl);
                    int timeout = 0;
                    while (client.connected()) {
                        if (++timeout > 1500) {
                            fb_log.e("Auth", "Connection timeout");
                            break;
                        }
                        delay(10);
                        if (client.available()) {
                            String response = client.readString();
                            int at = response.indexOf(fb_acc);
                            int ei = response.indexOf(fb_exp);
                            int tt = response.indexOf(fb_tot);
                            if (at > 0 && ei > 0 && tt > 0) {
                                token = response.substring(at + 15, ei - 3);
                                ttl = response.substring(ei + 12, tt - 2).toInt();
                                client.stop();
                                fb_log.i("Auth", "Access Token has been updated");
                                break;
                            }
                        }
                    }
                } else {
                    fb_log.e("Auth", "Connection failed");
                }
                client.stop();
            }
        }
        delay(5000);
    }
}

/*------ FirebaseAuth ------*/
FAUTH::FAUTH() {
    xTaskCreate(authTask, "authTask", 8192, NULL, 1, NULL);
}

void FAUTH::begin(const String &projectEmail, const String &privateKey) {
    email = projectEmail;
    key = privateKey;
}

String FAUTH::getAccessToken() {
    return token;
}

bool FAUTH::isExpired() {
    return ttl <= 0;
}

FAUTH FirebaseAuth;
