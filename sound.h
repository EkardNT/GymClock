#ifndef GYMCLOCK_SOUND_H
#define GYMCLOCK_SOUND_H

#include <AceRoutine.h>

const int SOUND_NONE = 0;
const int SOUND_BEEP = 1;

class SoundRoutine : public ace_routine::Coroutine {
    public:
        SoundRoutine() {}
        int runCoroutine() override;
        void playSound(int sound);

    private:
        int currentSound = SOUND_NONE;
};

extern SoundRoutine soundRoutine;

#endif