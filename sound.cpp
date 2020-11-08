#include "sound.h"
#include "common.h"

using namespace ace_routine;

int SoundRoutine::runCoroutine() {
    COROUTINE_LOOP() {
        COROUTINE_AWAIT(this->currentSound != SOUND_NONE);
        Serial.println("Current sound is not NONE!");
        switch (this->currentSound) {
            case SOUND_NONE:
                break;
            case SOUND_BEEP:
                Serial.println("Current sound is SOUND_BEEP");
                tone(tonePin, 261, 500);
                break;
        }
        this->currentSound = SOUND_NONE;
    }
}

void SoundRoutine::playSound(int sound) {
    Serial.printf("Playing sound %d\r\n", sound);
    this->currentSound = sound;
    this->reset();
}