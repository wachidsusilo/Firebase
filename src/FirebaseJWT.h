#ifndef FIREBASEJWT_H
#define FIREBASEJWT_H

#include "Arduino.h"

typedef void(*JWTHandler)(String, bool);

class FJWT {
  public:
    static FJWT& getInstance();
    bool encode(String projectEmail, String privateKey, String scope, uint32_t unixTime);
    void begin(JWTHandler f);

  private:
    FJWT() {};
    FJWT(const FJWT&);
    FJWT& operator=(const FJWT&);
    static String base64_encode(const uint8_t *bytesToEncode, uint16_t len);
    static String base64_encode(String str);
    static void jwtTask(void *parameter);
    static JWTHandler f;
    static String projectEmail;
    static String privateKey;
    static String scope;
    static uint32_t unixTime;
    static uint8_t buffer[2000];

};

#define FirebaseJWT FJWT::getInstance()

#endif
