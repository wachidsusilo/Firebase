#include "FirebaseJWT.h"

#include "FirebaseConstant.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/pk.h"

/*------ Base64 ------*/
String FJWT::base64_encode(const uint8_t *bytesToEncode, uint16_t len) {
    String ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(bytesToEncode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (i = 0; (i < 4); i++) {
                ret += fb_b64[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        for (j = 0; (j < i + 1); j++) {
            ret += fb_b64[char_array_4[j]];
        }
    }
    return ret;
}

String FJWT::base64_encode(String str) {
    return base64_encode((const unsigned char *)str.c_str(), str.length());
}

/*------ FirebaseJWT ------*/
String FJWT::encode(const String &projectEmail, const String &privateKey, const String &scope, uint32_t unixTime) {
    if (unixTime > 1600000000) {
        String header = "{\"typ\":\"JWT\",\"alg\":\"RS256\"}";
        String payload = "{";
        payload += "\"iss\":\"" + projectEmail + "\",";
        payload += "\"sub\":\"" + projectEmail + "\",";
        payload += "\"aud\":\"https://www.googleapis.com/oauth2/v4/token\",";
        payload += "\"iat\":\"" + String(unixTime) + "\",";
        payload += "\"exp\":\"" + String(unixTime + 3600) + "\",";
        payload += "\"scope\":\"" + scope + "\"}";

        String content = base64_encode(header) + "." + base64_encode(payload);

        mbedtls_pk_context pk_context;
        mbedtls_pk_init(&pk_context);
        int rc = mbedtls_pk_parse_key(&pk_context, (uint8_t *)privateKey.c_str(), privateKey.length() + 1, NULL, 0);
        if (rc != 0) {
            mbedtls_pk_free(&pk_context);
            fb_log.e("JWT", "Failed to parse private key (err_code: 0x" + String(rc, HEX) + ")");
            return "";
        }

        mbedtls_entropy_context entropy;
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_context ctr_drbg;
        mbedtls_ctr_drbg_init(&ctr_drbg);
        const char *pers = "firebase-jwt";
        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers));

        uint8_t digest[32];
        rc = mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (uint8_t *)content.c_str(), content.length(), digest);
        if (rc != 0) {
            mbedtls_pk_free(&pk_context);
            mbedtls_entropy_free(&entropy);
            mbedtls_ctr_drbg_free(&ctr_drbg);
            fb_log.e("JWT", "Failed to digest content (err_code: 0x" + String(rc, HEX) + ")");
            return "";
        }

        size_t retSize;
        rc = mbedtls_pk_sign(&pk_context, MBEDTLS_MD_SHA256, digest, sizeof(digest), buffer, &retSize, mbedtls_ctr_drbg_random, &ctr_drbg);
        if (rc != 0) {
            mbedtls_pk_free(&pk_context);
            mbedtls_entropy_free(&entropy);
            mbedtls_ctr_drbg_free(&ctr_drbg);
            fb_log.e("JWT", "Failed to sign content (err_code: 0x" + String(rc, HEX) + ")");
            return "";
        }

        String signature = base64_encode(buffer, retSize);
        mbedtls_pk_free(&pk_context);
        mbedtls_entropy_free(&entropy);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        return content + "." + signature;
    } else {
        fb_log.e("JWT", "Unix timestamp is out of date(err_code: 0)");
        return "";
    }
}

FJWT FirebaseJWT;
