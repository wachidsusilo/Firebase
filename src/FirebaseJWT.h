#ifndef FIREBASEJWT_H
#define FIREBASEJWT_H

#include "Arduino.h"
#include "Log.h"

class FJWT {
  public:
    String base64_encode(String str);
    String base64_encode(const uint8_t *bytesToEncode, uint16_t len);
    String encode(const String &projectEmail, const String &privateKey, const String &scope, uint32_t unixTime);

  private:
    uint8_t buffer[2000];

};

extern FJWT FirebaseJWT;

#endif
