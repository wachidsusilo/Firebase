#include "NTP.h"

#include "lwip/dns.h"
#include "lwip/ip_addr.h"

ip_addr_t NTPUDP::ip;
bool NTPUDP::isWaiting;
uint32_t NTPUDP::lastUpdate;
uint32_t NTPUDP::currentEpoch;

/*------ FreeRTOS Task ------*/
void NTPUDP::timeUpdate(void *parameter) {
    isWaiting = false;
    while (true) {
        if (WiFi.isConnected() && !isWaiting) {
            isWaiting = true;
            ip_addr_t srv;
            dns_gethostbyname(
                "pool.ntp.org", &srv, [](const char *name, const ip_addr *ipaddr, void *arg) {
                    NTPUDP::ip = *ipaddr;
                    NTPUDP::isWaiting = false;
                },
                NULL);
            int timeout = 0;
            while (isWaiting) {
                if (timeout++ > 150) {
                    isWaiting = false;
                }
                delay(100);
            }
            if (!ip.u_addr.ip4.addr) {
                fb_log.e("NTP", "Cannot resolve 'pool.ntp.org'");
            } else {
                byte buffer[48];
                memset(buffer, 0, 48);
                buffer[0] = 0xE3;
                buffer[1] = 0x00;
                buffer[2] = 0x06;
                buffer[3] = 0xEC;
                buffer[12] = 0x31;
                buffer[13] = 0x4E;
                buffer[14] = 0x31;
                buffer[15] = 0x34;
                AsyncUDP ntpUdp;
                ntpUdp.connect(&ip, 123);
                ntpUdp.onPacket([](AsyncUDPPacket packet) {
                    NTPUDP::isWaiting = false;
                    byte buffer[48];
                    NTPUDP::lastUpdate = millis();
                    memcpy(buffer, packet.data(), packet.length());
                    NTPUDP::currentEpoch = (word(buffer[40], buffer[41]) << 16 | word(buffer[42], buffer[43])) - 2208988800;
                    fb_log.i("NTP", "Time has been updated");
                });
                if (!ntpUdp.connected()) {
                    fb_log.e("NTP", "Connection failed");
                } else {
                    ntpUdp.write(buffer, 48);
                    isWaiting = true;
                }
                timeout = 0;
                while (isWaiting) {
                    if (timeout++ > 150) {
                        fb_log.e("NTP", "Connection timeout");
                        isWaiting = false;
                    }
                    delay(100);
                }
            }
            if (currentEpoch > 1600000000) delay(600000);
        }
        delay(1000);
    }
}

/*------ NTP ------*/
NTPUDP::NTPUDP()
    : offset(0) {
    xTaskCreate(timeUpdate, "timeUpdate", 4096, NULL, 1, NULL);
}

uint32_t NTPUDP::getUnixTime() {
    return currentEpoch + ((millis() - lastUpdate) / 1000);
}

uint32_t NTPUDP::getMinutes() {
    return ((getUnixTime() + offset) % 86400) / 60;
}

uint32_t NTPUDP::getSeconds() {
    return (getUnixTime() + offset) % 86400;
}

uint16_t NTPUDP::getYear() {
    return DateTime(getUnixTime() + offset).year;
}

uint8_t NTPUDP::getMonth() {
    return DateTime(getUnixTime() + offset).month;
}

uint8_t NTPUDP::getDate() {
    return DateTime(getUnixTime() + offset).date;
}

uint8_t NTPUDP::getHour() {
    return DateTime(getUnixTime() + offset).hour;
}

uint8_t NTPUDP::getMinute() {
    return DateTime(getUnixTime() + offset).minute;
}

uint8_t NTPUDP::getSecond() {
    return DateTime(getUnixTime() + offset).second;
}

uint8_t NTPUDP::getDayOfTheWeek() {
    return DateTime(getUnixTime() + offset).dayOfWeek;
}

NTPUDP::DateTime NTPUDP::now() {
    return DateTime(getUnixTime() + offset);
}

String NTPUDP::getFormattedTime(const String &delimiter) {
    DateTime now(getUnixTime() + offset);
    if (delimiter.length() == 2) {
        return (now.hour < 10 ? "0" : "") + String(now.hour) + delimiter[0] + (now.minute < 10 ? "0" : "") + now.minute + delimiter[1] + (now.second < 10 ? "0" : "") + now.second;
    } else if (delimiter.length() == 3) {
        return String(delimiter[0]) + (now.hour < 10 ? "0" : "") + now.hour + delimiter[1] + (now.minute < 10 ? "0" : "") + now.minute + delimiter[2] + (now.second < 10 ? "0" : "") + now.second;
    }
    return (now.hour < 10 ? "0" : "") + String(now.hour) + delimiter + (now.minute < 10 ? "0" : "") + now.minute + delimiter + (now.second < 10 ? "0" : "") + now.second;
}

String NTPUDP::getFormattedDate(const String &delimiter) {
    DateTime now(getUnixTime() + offset);
    if (delimiter.length() == 2) {
        return (now.date < 10 ? "0" : "") + String(now.date) + delimiter[0] + (now.month < 10 ? "0" : "") + now.month + delimiter[1] + now.year;
    } else if (delimiter.length() == 3) {
        return String(delimiter[0]) + (now.date < 10 ? "0" : "") + now.date + delimiter[1] + (now.month < 10 ? "0" : "") + now.month + delimiter[2] + now.year;
    }
    return (now.date < 10 ? "0" : "") + String(now.date) + delimiter + (now.month < 10 ? "0" : "") + now.month + delimiter + now.year;
}

void NTPUDP::setOffset(const float &offset) {
    this->offset = round(offset * 3600);
}

bool NTPUDP::isReady() {
    return getUnixTime() > 1600000000;
}

NTPUDP NTP;
