#include "FirebaseAuth.h"

firebase_auth_t FAUTH::data;
firebase_account_t FAUTH::account;
long FAUTH::ttl;
bool FAUTH::isAuthenticating;

/*------ Callback ------*/
void FAUTH::jwtCallback(String content, bool res) {
  if (res) {
    data.jwt = content;
    String payload = "{\"grant_type\":\"urn:ietf:params:oauth:grant-type:jwt-bearer\",\"assertion\": \"" + content + "\"}";
    HTTPClient *http = new HTTPClient();
    WiFiClientSecure *client = new WiFiClientSecure();
    http->begin(*client, "https://www.googleapis.com/oauth2/v4/token");
    http->addHeader("Content-Type", "application/json", false, true);
    int httpCode = http->POST(payload);
    if (httpCode == 200) {
      String response = http->getString();
      int at = response.indexOf("access_token");
      int ei = response.indexOf("expires_in");
      int tt = response.indexOf("token_type");
      if (at > 0 && ei > 0 && tt > 0) {
        data.token = response.substring(at + 15, ei - 3);
        ttl = response.substring(ei + 12, tt - 2).toInt();
      }
    }
    Serial.println("Authentication: " + String(httpCode));
    http->end();
    delete http;
    delete client;
  }
  isAuthenticating = false;
}

/*------ FreeRTOS Task ------*/
void FAUTH::authTask(void *parameter) {
  while (true) {
    if (ttl > 0) {
      ttl -= 5;
    }
    if (WiFi.isConnected() && ttl <= 600 && !isAuthenticating && NTP.getUnixTime() > 1600000000) {
      isAuthenticating = true;
      FirebaseJWT.encode(account.email, account.key, "https://www.googleapis.com/auth/cloud-platform", NTP.getUnixTime());
    }
    delay(5000);
  }
}

/*------ FirebaseJWT ------*/
FAUTH& FAUTH::getInstance() {
  static FAUTH instance;
  return instance;
};

void FAUTH::begin(String projectEmail, String privateKey) {
  NTP.begin();
  FirebaseJWT.begin(jwtCallback);
  account = firebase_account_t(projectEmail, privateKey);
  if (!isBegin) {
    isBegin = true;
    xTaskCreate(authTask, "authTask", 4096, NULL, 1, NULL);
  }
}

String FAUTH::getAccessToken() {
  return data.token;
}

String FAUTH::getJWT() {
  return data.jwt;
}

bool FAUTH::isExpired() {
  return ttl <= 0;
}

long FAUTH::getTTL() {
  return ttl;
}
