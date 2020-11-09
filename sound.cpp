#include "sound.h"
#include "common.h"

using namespace ace_routine;

int SoundRoutine::runCoroutine() {
    static int noteOffset = 0;
    static int elapsedMillis = 0;
    COROUTINE_LOOP() {
        COROUTINE_AWAIT(this->checkAndClearInterrupt());
        Serial.printf("Sound coroutine starting sound %d\r\n", this->currentSound);
        noteOffset = 0;
        elapsedMillis = 0;
        while (true) {
            // First, check for end-of-song.
            this->frequency = this->currentSound[noteOffset * 3];
            if (this->frequency == 0) {
                break;
            }
            // Next, wait until its time to start this node.
            this->absoluteStart = this->currentSound[noteOffset * 3 + 2];
            if (this->absoluteStart > elapsedMillis) {
                COROUTINE_DELAY(this->absoluteStart - elapsedMillis);
                elapsedMillis = this->absoluteStart;
            }

            // TODO: allow melody interruption by checking interrupt here? That would not
            // be perfect however becaue the interrupt would not occur until the delay
            // finishes, which could be a while after the sound should have changed.
            // See https://github.com/bxparks/AceRoutine/issues/20
            // Until that's resolved, I'm just not going to support sound interruption.

            // Now play the note.
            int duration = this->currentSound[noteOffset * 3 + 1];
            tone(tonePin, this->frequency, duration);

            // Move on to the next note.
            noteOffset++;
        }
    }
}

bool SoundRoutine::checkAndClearInterrupt() {
    if (this->generation != this->expectedGeneration) {
      this->expectedGeneration = this->generation;
      return true;
    }
    return false;
}

void SoundRoutine::playSound(const int *sound) {
    Serial.printf("Playing sound %d\r\n", sound);
    this->currentSound = sound;
    this->generation++;
}