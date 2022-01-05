#ifndef FIREBASE_AUTH_H
#define FIREBASE_AUTH_H

#include "Arduino.h"
#include "ClientSecure.h"
#include "FirebaseConstant.h"
#include "FirebaseJWT.h"
#include "Log.h"
#include "NTP.h"
#include "WiFi.h"

class FAUTH {
   public:
    FAUTH();
    void begin(const String &projectEmail, const String &privateKey);
    String getAccessToken();
    bool isExpired();

   private:
    static String token;
    static String email;
    static String key;
    static long ttl;
    static void authTask(void *parameter);
};

extern FAUTH FirebaseAuth;

#endif
