#include "NTP.h"

WiFiUDP NTPUDP::ntpUdp;
String NTPUDP::ntpServer;
uint16_t NTPUDP::ntpPort;
uint32_t NTPUDP::interval;
byte NTPUDP::buffer[48];
uint32_t NTPUDP::lastUpdate;
uint32_t NTPUDP::currentEpoch;

/*------ FreeRTOS Task ------*/
void NTPUDP::timeUpdate(void *parameter) {
  while (true) {
    if (WiFi.isConnected()) {
      ntpUpdate();
    }
    delay(interval);
  }
}

void NTPUDP::ntpSetup(void *parameter) {
  while (true) {
    if (WiFi.isConnected()) {
      ntpUdp.begin(ntpPort);
      ntpUpdate();
      vTaskDelete(NULL);
    }
    delay(1000);
  }
}

void NTPUDP::ntpUpdate() {
  memset(buffer, 0, 48);
  buffer[0] = 0xE3;
  buffer[1] = 0x00;
  buffer[2] = 0x06;
  buffer[3] = 0xEC;
  buffer[12]  = 0x31;
  buffer[13]  = 0x4E;
  buffer[14]  = 0x31;
  buffer[15]  = 0x34;

  ntpUdp.beginPacket(ntpServer.c_str(), 123);
  ntpUdp.write(buffer, 48);
  ntpUdp.endPacket();
  delay(1000);
  if (ntpUdp.parsePacket()) {
    lastUpdate = millis();
    ntpUdp.read(buffer, 48);
    currentEpoch = (word(buffer[40], buffer[41]) << 16 | word(buffer[42], buffer[43])) - 2208988800;
  }
}

/*------ NTP ------*/
NTPUDP& NTPUDP::getInstance() {
  static NTPUDP instance;
  return instance;
};

void NTPUDP::begin(String server, uint16_t port, uint32_t updateInterval) {
  if (WiFi.isConnected()) {
    ntpUdp.begin(port);
  } else {
    if (!isTaskSetup) {
      isTaskSetup = true;
      xTaskCreate(ntpSetup, "ntpSetup", 4096, NULL, 1, NULL);
    }
  }
  ntpPort = port;
  interval = updateInterval;
  ntpServer = server;
  if (!isTaskMain) {
    xTaskCreate(timeUpdate, "timeUpdate", 4096, NULL, 1, NULL);
  }
}

uint32_t NTPUDP::getUnixTime() {
  return currentEpoch + ((millis() - lastUpdate) / 1000);
}
