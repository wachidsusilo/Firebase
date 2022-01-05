#include "Firebase.h"

const String projectId(F("project-bcd12"));
const String clientEmail(F("firebase-adminsdk-abcde@project-bcd12.iam.gserviceaccount.com"));
const String privateKey(F("-----BEGIN PRIVATE KEY-----\n YOUR_PRIVATE_KEY \n-----END PRIVATE KEY-----\n"));
const String dbUrl(F("https://project-bcd12-default-rtdb.asia-southeast1.firebasedatabase.app"));

void callback(Json payload) {
    JSUtil.prettyPrint(Serial, payload);
}

void setup() {
    Serial.begin(115200);

    WiFi.begin("SSID", "PASSWORD");
    WiFi.setAutoReconnect(true);
    Serial.println("Connecting to WiFi...");
    while (!WiFi.isConnected()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi is connected");

    Firebase.attachDebugger(Serial);
    Firebase.begin(projectId, clientEmail, privateKey);
    Serial.println("Connecting to Firebase service...");
    while (!Firebase.isReady()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Firebase is ready");

    Firebase.database(dbUrl).path("parent/child/grandchild").get(callback);
}

void loop() {}
