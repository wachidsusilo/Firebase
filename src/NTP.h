#ifndef NTPUDP_H
#define NTPUDP_H

#include "Arduino.h"
#include "AsyncUDP.h"
#include "Log.h"
#include "WiFi.h"
#include "time.h"

class NTPUDP {
   public:
    struct DateTime {
        uint16_t year;
        uint8_t month;
        uint8_t date;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t dayOfWeek;

        DateTime(uint32_t unix) {
            int z = unix / 86400 + 719468;
            int era = (z >= 0 ? z : z - 146096) / 146097;
            unsigned doe = static_cast<unsigned>(z - era * 146097);
            unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
            year = static_cast<uint16_t>(yoe) + era * 400;
            unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
            unsigned mp = (5 * doy + 2) / 153;
            date = doy - (153 * mp + 2) / 5 + 1;
            month = mp + (mp < 10 ? 3 : -9);
            year += (month <= 2);
            int sod = unix % 86400;
            hour = sod / 3600;
            minute = (sod % 3600) / 60;
            second = sod % 60;
            dayOfWeek = ((uint32_t)round(unix / 86400.0) % 7) + 5;
        }
    };

    NTPUDP();
    uint32_t getUnixTime();
    uint32_t getMinutes();
    uint32_t getSeconds();
    uint16_t getYear();
    uint8_t getMonth();
    uint8_t getDate();
    uint8_t getHour();
    uint8_t getMinute();
    uint8_t getSecond();
    uint8_t getDayOfTheWeek();
    DateTime now();
    String getFormattedTime(const String &delimiter = ":");
    String getFormattedDate(const String &delimiter = "/");
    void setOffset(const float &offset);
    bool isReady();

   private:
    uint32_t offset;
    static bool isWaiting;
    static ip_addr_t ip;
    static uint32_t lastUpdate;
    static uint32_t currentEpoch;
    static void timeUpdate(void *parameter);
};

extern NTPUDP NTP;

#endif
