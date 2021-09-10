#include <AceTime.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "debug.h"
#include "ntp.h"

using namespace ace_time;

WiFiUDP ntpUdp;
NTPClient ntpClient(ntpUdp, "us.pool.ntp.org", 0, 3600000);

const int ZONE_MGR_CACHE_SIZE = 3;
BasicZoneManager<ZONE_MGR_CACHE_SIZE> zoneManager(
    zonedb::kZoneRegistrySize, zonedb::kZoneRegistry);
TimeZone localTz;

void initializeNtp() {
    Debug.println("Starting NTP client");
    ntpClient.begin();

    localTz = zoneManager.createForZoneInfo(&zonedb::kZoneAmerica_Los_Angeles);

    // Debug.println("Listing time zones");
    // for (uint16_t n = 0; n < ace_time::zonedb::kZoneRegistrySize; n++) {
    //     auto a = ace_time::zonedb::kZoneRegistry[n];

    //     BasicZone basicZone(a);
    //     basicZone.printNameTo(Debug);
    //     Debug.print(' ');
    //     basicZone.printShortNameTo(Debug);
    //     Debug.println();
    // }
}

void shutdownNtp() {
    Debug.println("Shutting down NTP client");
    ntpClient.end();
}

void updateNtp() {
    ntpClient.update();
}

bool isNtpTimeSet() {
    return ntpClient.isTimeSet();
}

ZonedDateTime getLocalTime() {
    return ZonedDateTime::forUnixSeconds(ntpClient.getEpochTime(), localTz);
}