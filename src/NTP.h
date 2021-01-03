#ifndef NTPUDP_H
#define NTPUDP_H

#include "Arduino.h"
#include "WiFi.h"
#include "WiFiUdp.h"

class NTPUDP {
  public:
    static NTPUDP& getInstance();
    void begin(String server = "pool.ntp.org", uint16_t port = 1337, uint32_t updateInterval = 60000);
    uint32_t getUnixTime();

  private:
    NTPUDP() {};
    NTPUDP(const NTPUDP&);
    NTPUDP& operator=(const NTPUDP&);
    bool isTaskSetup, isTaskMain;
    static WiFiUDP ntpUdp;
    static String ntpServer;
    static uint16_t ntpPort;
    static uint32_t interval;
    static byte buffer[48];
    static uint32_t lastUpdate;
    static uint32_t currentEpoch;
    static void ntpUpdate();
    static void timeUpdate(void *parameter);
    static void ntpSetup(void *parameter);
};

#define NTP NTPUDP::getInstance()

#endif
