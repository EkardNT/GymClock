#include <Arduino.h>
#include "debug.h"

DebugPrint Debug;

size_t DebugPrint::write(uint8_t data) {
    // Use UDP if enabled.
    if (this->udpEnabled) {
        if (!this->udp.beginPacket(this->debugIp, this->debugPort)) {
            Serial.println("Failed to begin UDP debug packet");
            return 1;
        }

        size_t written = this->udp.write(data);
        if (written != 1) {
            Serial.println("Unexpected response from UDP single-byte write");
            return 1;
        }

        if (!this->udp.endPacket()) {
            Serial.println("Failed to send UDP debug packet");
            return 1;
        }

        return 1;
    }

    // Otherwise, just delegate to serial.
    return Serial.write(data);
}

size_t DebugPrint::write(const uint8_t *buffer, size_t size) {
    // Use UDP if enabled.
    if (this->udpEnabled) {
        if (!this->udp.beginPacket(this->debugIp, this->debugPort)) {
            Serial.println("Failed to begin UDP multi-byte debug packet");
            return size;
        }

        size_t written = this->udp.write(buffer, size);
        if (written != size) {
            Serial.println("Unexpected response from UDP multi-byte write");
            return size;
        }

        if (!this->udp.endPacket()) {
            Serial.println("Failed to send UDP multi-byte debug packet");
            return size;
        }

        return size;
    }

    // Otherwise, just delegate to serial.
    return Serial.write(buffer, size);
}

int DebugPrint::availableForWrite() {
    return 0;
}

void DebugPrint::flush() {
    Serial.flush();
}

void DebugPrint::enableUdp(IPAddress debugIp, uint16_t debugPort) {
    this->udpEnabled = true;
    this->debugIp = debugIp;
    this->debugPort = debugPort;
}

void DebugPrint::disableUdp() {
    this->udpEnabled = false;
}

bool DebugPrint::isUdpEnabled() {
    return this->udpEnabled;
}