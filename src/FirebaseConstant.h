#ifndef FIREBASE_CONSTANT
#define FIREBASE_CONSTANT

#include "Arduino.h"

const char fb_scp[] PROGMEM = "https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/cloud-platform";
const char fb_plh[] PROGMEM = "{\"grant_type\":\"urn:ietf:params:oauth:grant-type:jwt-bearer\",\"assertion\": \"";
const char fb_pau[] PROGMEM = "POST /oauth2/v4/token HTTP/1.1";
const char fb_gap[] PROGMEM = "Host: www.googleapis.com";
const char fb_uag[] PROGMEM = "User-Agent: ESP32";
const char fb_con[] PROGMEM = "Connection: close";
const char fb_kea[] PROGMEM = "Connection: keep-alive";
const char fb_cot[] PROGMEM = "Content-Type: application/json";
const char fb_col[] PROGMEM = "Content-Length: ";
const char fb_chn[] PROGMEM = "Transfer-Encoding: chunked";
const char fb_sil[] PROGMEM = "?print=silent";
const char fb_get[] PROGMEM = "GET ";
const char fb_put[] PROGMEM = "PUT ";
const char fb_pos[] PROGMEM = "POST ";
const char fb_pat[] PROGMEM = "PATCH ";
const char fb_del[] PROGMEM = "DELETE ";
const char fb_ver[] PROGMEM = " HTTP/1.1";
const char fb_hos[] PROGMEM = "Host: ";
const char fb_aut[] PROGMEM = "Authorization: Bearer ";
const char fb_ach[] PROGMEM = "Accept: text/event-stream";
const char fb_acc[] PROGMEM = "access_token";
const char fb_exp[] PROGMEM = "expires_in";
const char fb_tot[] PROGMEM = "token_type";
const char fb_err[] PROGMEM = "err_code";
const char fb_b64[] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
const char fb_nlv[] PROGMEM = "\"nullValue\"";
const char fb_blv[] PROGMEM = "\"booleanValue\"";
const char fb_inv[] PROGMEM = "\"integerValue\"";
const char fb_dbv[] PROGMEM = "\"doubleValue\"";
const char fb_tsv[] PROGMEM = "\"timestampValue\"";
const char fb_stv[] PROGMEM = "\"stringValue\"";
const char fb_byv[] PROGMEM = "\"bytesValue\"";
const char fb_rev[] PROGMEM = "\"referenceValue\"";
const char fb_gev[] PROGMEM = "\"geoPointValue\"";
const char fb_arv[] PROGMEM = "\"arrayValue\"";
const char fb_mav[] PROGMEM = "\"mapValue\"";
const char fb_tru[] PROGMEM = "true";
const char fb_fal[] PROGMEM = "false";
const char fb_nul[] PROGMEM = "null";
const char fb_crl[] PROGMEM = "\r\n";

const char fb_db_put[] PROGMEM = "?x-http-method-override=PUT";
const char fb_db_pat[] PROGMEM = "?x-http-method-override=PATCH";
const char fb_db_del[] PROGMEM = "?x-http-method-override=DELETE";

#endif
