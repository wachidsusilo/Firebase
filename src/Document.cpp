#include "Document.h"

/*--- Timestamp ---*/
Document::Timestamp::Timestamp(const uint16_t &year, const uint8_t &month, const uint8_t &date, const uint8_t &hour, const uint8_t &minute, const uint8_t &second)
    : year(year), month(month), date(date), hour(hour), minute(minute), second(second) {}

void Document::Timestamp::setYear(const uint16_t &year) {
    this->year = year;
}

void Document::Timestamp::setMonth(const uint8_t &month) {
    this->month = month;
}

void Document::Timestamp::setDate(const uint8_t &date) {
    this->date = date;
}

void Document::Timestamp::setHour(const uint8_t &hour) {
    this->hour = hour;
}

void Document::Timestamp::setMinute(const uint8_t &minute) {
    this->minute = minute;
}

void Document::Timestamp::setSecond(const uint8_t &second) {
    this->second = second;
}

String Document::Timestamp::toString() {
    validate();
    return "\"" + String(year) + "-" + (month < 10 ? "0" : "") + month + "-" + (date < 10 ? "0" : "") + date + "T" + (hour < 10 ? "0" : "") + hour + ":" + (minute < 10 ? "0" : "") + minute + ":" + (second < 10 ? "0" : "") + second + "Z\"";
}

void Document::Timestamp::validate() {
    uint8_t dateInMonth[12] = {31, (isLeap() ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 1) {
        fb_log.w("Timestamp", "Year is less than 1");
        year = 1;
    }
    if (month < 1) {
        fb_log.w("Timestamp", "Month is less than 1");
        month = 1;
    }
    if (date < 1) {
        fb_log.w("Timestamp", "Date is less than 1");
        date = 1;
    }
    if (month > 12) {
        fb_log.w("Timestamp", "Month is greater than 12");
        month = 12;
    }
    if (date > dateInMonth[month - 1]) {
        fb_log.w("Timestamp", "Date is greater than " + String(dateInMonth[month - 1]));
        date = dateInMonth[month - 1];
    }
    if (hour > 23) {
        fb_log.w("Timestamp", "Hour is greater than 23");
        hour = 23;
    }
    if (minute > 59) {
        fb_log.w("Timestamp", "Minute is greater than 59");
        minute = 59;
    }
    if (second > 59) {
        fb_log.w("Timestamp", "Second is greater than 59");
        second = 59;
    }
}

bool Document::Timestamp::isLeap() {
    if (year % 4 != 0) {
        return false;
    } else if (year % 100 != 0) {
        return true;
    } else if (year % 400 != 0) {
        return false;
    } else {
        return true;
    }
}

/*--- Byte ---*/
Document::Byte::Byte(const uint8_t *data, const size_t &len) {
    base64 = FirebaseJWT.base64_encode(data, len);
}

String Document::Byte::toString() {
    return "\"" + base64 + "\"";
}

/*--- Reference ---*/
Document::Reference::Reference(const String &path)
    : path(path) {}

void Document::Reference::setPath(const String &path) {
    this->path = path;
}

String Document::Reference::toString() {
    return "\"" + path + "\"";
}

/*--- GeoPoint ---*/
Document::GeoPoint::GeoPoint(const double &latitude, const double &longitude)
    : geoPoint(Json()) {
    geoPoint.add("latitude", latitude);
    geoPoint.add("longitude", longitude);
}

void Document::GeoPoint::setLatitude(const double &latitude) {
    geoPoint.erase("latitude");
    geoPoint.add("latitude", latitude);
}

void Document::GeoPoint::setLongitude(const double &longitude) {
    geoPoint.erase("longitude");
    geoPoint.add("longitude", longitude);
}

String Document::GeoPoint::toString() {
    return geoPoint.toString();
}

/*--- Array ---*/
Document::Array::Array()
    : arrays(JsonArray()) {}

Document::Array &Document::Array::push(Value &value) {
    arrays.push(value.toJson());
    return *this;
}

Document::Array &Document::Array::push(Value &&value) {
    arrays.push(value.toJson());
    return *this;
}

String Document::Array::toString() {
    return "{\"values\":" + arrays.toString() + "}";
}

/*--- Map ---*/
Document::Map::Map()
    : maps(Json()) {}

Document::Map &Document::Map::add(const String &name, Value &value) {
    maps.add(name, value.toJson());
    return *this;
}

Document::Map &Document::Map::add(const String &name, Value &&value) {
    maps.add(name, value.toJson());
    return *this;
}

String Document::Map::toString() {
    return "{\"fields\":" + maps.toString() + "}";
}

/*--- Value ---*/
Document::Value::Value(const void *value) {
    val = String("{") + fb_nlv + ":" + fb_nul + "}";
}

Document::Value::Value(const bool &value) {
    val = String("{") + fb_blv + ":" + (value ? fb_tru : fb_fal) + "}";
}

Document::Value::Value(const int &value) {
    val = String("{") + fb_inv + ":" + value + "}";
}

Document::Value::Value(const float &value) {
    val = String("{") + fb_dbv + ":" + value + "}";
}

Document::Value::Value(const double &value) {
    val = String("{") + fb_dbv + ":" + value + "}";
}

Document::Value::Value(const char *value) {
    val = String("{") + fb_stv + ":\"" + value + "\"}";
}

Document::Value::Value(const String &value) {
    val = String("{") + fb_stv + ":\"" + value + "\"}";
}

Document::Value::Value(Timestamp &value) {
    val = String("{") + fb_tsv + ":" + value.toString() + "}";
}

Document::Value::Value(Timestamp &&value) {
    val = String("{") + fb_tsv + ":" + value.toString() + "}";
}

Document::Value::Value(Reference &value) {
    val = String("{") + fb_rev + ":" + value.toString() + "}";
}

Document::Value::Value(Reference &&value) {
    val = String("{") + fb_rev + ":" + value.toString() + "}";
}

Document::Value::Value(Byte &value) {
    val = String("{") + fb_byv + ":" + value.toString() + "}";
}

Document::Value::Value(Byte &&value) {
    val = String("{") + fb_byv + ":" + value.toString() + "}";
}

Document::Value::Value(GeoPoint &value) {
    val = String("{") + fb_gev + ":" + value.toString() + "}";
}

Document::Value::Value(GeoPoint &&value) {
    val = String("{") + fb_gev + ":" + value.toString() + "}";
}

Document::Value::Value(Array &value) {
    val = String("{") + fb_arv + ":" + value.toString() + "}";
}

Document::Value::Value(Array &&value) {
    val = String("{") + fb_arv + ":" + value.toString() + "}";
}

Document::Value::Value(Map &value) {
    val = String("{") + fb_mav + ":" + value.toString() + "}";
}

Document::Value::Value(Map &&value) {
    val = String("{") + fb_mav + ":" + value.toString() + "}";
}

String Document::Value::toString() {
    return val;
}

Json Document::Value::toJson() {
    return Json(val);
}

/*--- Document ---*/
Document::Document()
    : doc(Json()) {}

Document &Document::addField(const String &name, Value &value) {
    doc.add(name, value.toJson());
    return *this;
}

Document &Document::addField(const String &name, Value &&value) {
    doc.add(name, value.toJson());
    return *this;
}

String Document::toString() {
    return doc.size() ? String("{\"fields\":") + doc.toString() + "}" : "";
}

Json Document::toJson() {
    return doc.size() ? Json().add("fields", doc) : Json();
}

void Document::remove(const String &name) {
    doc.erase(name);
}

void Document::clear() {
    doc.clear();
}
