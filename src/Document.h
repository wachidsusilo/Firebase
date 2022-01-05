#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "Arduino.h"
#include "FirebaseConstant.h"
#include "FirebaseJWT.h"
#include "Json.h"
#include "Log.h"

class Document {
   public:
    struct Value;
    struct Timestamp {
       public:
        Timestamp(const uint16_t &year = 1, const uint8_t &month = 1, const uint8_t &date = 1, const uint8_t &hour = 0, const uint8_t &minute = 0, const uint8_t &second = 0);
        void setYear(const uint16_t &year);
        void setMonth(const uint8_t &month);
        void setDate(const uint8_t &date);
        void setHour(const uint8_t &hour);
        void setMinute(const uint8_t &minute);
        void setSecond(const uint8_t &second);
        String toString();

       private:
        uint16_t year;
        uint8_t month;
        uint8_t date;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        void validate();
        bool isLeap();
    };

    struct Byte {
       public:
        Byte(const uint8_t *data, const size_t &len);
        String toString();

       private:
        String base64;
    };

    struct Reference {
       public:
        Reference(const String &path = "");
        void setPath(const String &path);
        String toString();

       private:
        String path;
    };

    struct GeoPoint {
       public:
        GeoPoint(const double &latitude = 0, const double &longitude = 0);
        void setLatitude(const double &latitude);
        void setLongitude(const double &latitude);
        String toString();

       private:
        Json geoPoint;
    };

    struct Array {
       public:
        Array();
        Array &push(Value &value);
        Array &push(Value &&value);
        String toString();

       private:
        JsonArray arrays;
    };

    struct Map {
       public:
        Map();
        Map &add(const String &name, Value &value);
        Map &add(const String &name, Value &&value);
        String toString();

       private:
        Json maps;
    };

    struct Value {
       public:
        Value(const void *value);
        Value(const bool &value);
        Value(const int &value);
        Value(const float &value);
        Value(const double &value);
        Value(const char *value);
        Value(const String &value);
        Value(Timestamp &value);
        Value(Timestamp &&value);
        Value(Reference &value);
        Value(Reference &&value);
        Value(Byte &value);
        Value(Byte &&value);
        Value(GeoPoint &value);
        Value(GeoPoint &&value);
        Value(Array &value);
        Value(Array &&value);
        Value(Map &value);
        Value(Map &&value);
        String toString();
        Json toJson();

       private:
        String val;
    };

    Document();
    Document &addField(const String &name, Value &value);
    Document &addField(const String &name, Value &&value);
    String toString();
    Json toJson();
    void remove(const String &name);
    void clear();

   private:
    Json doc;
};

#endif
