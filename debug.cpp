#include <Arduino.h>
#include <AceRoutine.h>
#include "common.h"
#include "debug.h"
#include "programs.h"

DebugPrint Debug;

size_t DebugPrint::write(uint8_t data) {
    // Use UDP if enabled.
    if (this->udpEnabled && networkActive) {
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
    if (this->udpEnabled && networkActive) {
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

void printlnStatus(ace_routine::Coroutine *co) {
    if (co->isSuspended()) {
        Debug.println("suspended");
    } else if (co->isYielding()) {
        Debug.println("yielding");
    } else if (co->isDelaying()) {
        Debug.println("delaying");
    } else if (co->isRunning()) {
        Debug.println("running");
    } else if (co->isEnding()) {
        Debug.println("ending");
    } else if (co->isTerminated()) {
        Debug.println("terminated");
    }
}

void DebugPrint::dump() {
    Debug.println(F("================ State Dump ================"));
    Debug.println(F("----- CoroutineScheduler::list results -----"));
    ace_routine::CoroutineScheduler::list(Debug);
    Debug.println(F("------------- Coroutine states -------------"));
    Debug.print(F("Init: "));
    printlnStatus(&initProgram);
    Debug.print(F("Test: "));
    printlnStatus(&testProgram);
    Debug.print(F("Clock: "));
    printlnStatus(&clockProgram);
    Debug.print(F("Stopwatch: "));
    printlnStatus(&stopwatchProgram);
    Debug.print(F("Countdown: "));
    printlnStatus(&countdownProgram);
    Debug.print(F("Scored Countdown: "));
    printlnStatus(&scoredCountdownProgram);
}