#ifndef FIREBASE_AUTH_H
#define FIREBASE_AUTH_H

#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "HTTPClient.h"
#include "FirebaseJWT.h"
#include "NTP.h"

struct firebase_auth_t {
  String token;
  String jwt;

  firebase_auth_t() {};
  firebase_auth_t(String token, String jwt)
    : token(token), jwt(jwt) {};
};

struct firebase_account_t {
  String email;
  String key;

  firebase_account_t() {}
  firebase_account_t(String email, String key)
    : email(email), key(key) {}
};

class FAUTH {
  public:
    static FAUTH& getInstance();
    void begin(String projectEmail, String privateKey);
    String getAccessToken();
    String getJWT();
    bool isExpired();
    long getTTL();

  private:
    FAUTH() {}
    FAUTH(const FAUTH&);
    FAUTH& operator=(const FAUTH&);
    bool isBegin;
    static firebase_auth_t data;
    static firebase_account_t account;
    static bool isAuthenticating;
    static long ttl;
    static void jwtCallback(String jwt, bool res);
    static void authTask(void *parameter);
};

#define FirebaseAuth FAUTH::getInstance()

#endif
