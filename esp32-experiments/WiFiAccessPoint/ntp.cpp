#include <WiFiUdp.h>
#include <NTPClient.h>
#include "debug.h"
#include "ntp.h"

WiFiUDP ntpUdp;
NTPClient ntpClient(ntpUdp, "us.pool.ntp.org", 0, 3600000);

void initializeNtp() {
    Debug.println("Starting NTP client");
    ntpClient.begin();
}

void shutdownNtp() {
    Debug.println("Shutting down NTP client");
    ntpClient.end();
}

void updateNtp() {
    ntpClient.update();
}

unsigned long getEpochTime() {
    return ntpClient.getEpochTime();
}

int getHours(unsigned long epochTime) const {
  return ((epochTime % 86400L) / 3600);
}

int getMinutes(unsigned long epochTime) const {
  return ((epochTime % 3600) / 60);
}

int getSeconds(unsigned long epochTime) const {
  return (epochTime % 60);
}