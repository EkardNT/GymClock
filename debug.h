#ifndef GYMCLOCK_DEBUG_H
#define GYMCLOCK_DEBUG_H

#include <Print.h>
#include <WiFiUdp.h>
#include <IPAddress.h>

class DebugPrint : public Print {
    public:
        size_t write(uint8_t);
        size_t write(const uint8_t *buffer, size_t size);
        int availableForWrite();
        void flush();
        void enableUdp(IPAddress debugIp, uint16_t debugPort);
        void disableUdp();
        bool isUdpEnabled();
        void dump();

    private:
        bool udpEnabled = false;
        IPAddress debugIp;
        uint16_t debugPort = 0;
        WiFiUDP udp;
};

extern DebugPrint Debug;

#endif