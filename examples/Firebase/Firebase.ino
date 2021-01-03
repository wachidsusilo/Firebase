#include <WiFi.h>
#include "Firebase.h"

String projectId = "project-name-1ab2c";
String serviceAccountEmail = "firebase-adminsdk-1abcd@project-name-1ab2c.iam.gserviceaccount.com";
String privateKey = "-----BEGIN PRIVATE KEY-----\n....privateKey....\n-----END PRIVATE KEY-----\n";

void setup() {
  Serial.begin(115200);
  Firebase.begin(projectId, serviceAccountEmail, privateKey);
  WiFi.begin("SSID", "PASSWORD");
  WiFi.setAutoReconnect(true);
  Serial.println("Ready");
}

void loop() {
  if (Serial.available()) {
    String str = Serial.readString();
    if (str.equals("s")) {
      Serial.println(Firebase.getAccessToken());
    } else if (str.equals("d")) {
      Serial.println(Firebase.getMessageBody(message));
    } else if (str.equals("f")) {
      Serial.println("Message: " + Firebase.sendAndroidNotification(Message::TOPIC, "topic", "Title", "Description", "channel-id"));
    } else if (str.equals("g")) {
      Serial.println(FirebaseAuth.getTTL());
    } else {
      Serial.println("FreeHeap: " + String(ESP.getFreeHeap()) + " Byte");
      Serial.println(WiFi.isConnected() ? "WiFi: Connected" : "WiFi: Disconnected");
    }
  }
}
