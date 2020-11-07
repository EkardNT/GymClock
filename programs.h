#ifndef GYMCLOCK_PROGRAMS_H
#define GYMCLOCK_PROGRAMS_H

#include <AceRoutine.h>

const int PROGRAM_INIT = 0;
const int PROGRAM_TEST = 1;
const int PROGRAM_CLOCK = 2;

int currentProgram();
void changeProgram(int newProgram);
void suspendAll(int exceptProgram);

class TestProgramCo : public ace_routine::Coroutine {
    public:
        TestProgramCo() {}
        int runCoroutine() override;
};

class InitProgramCo : public ace_routine::Coroutine {
    public:
        InitProgramCo() {}
        int runCoroutine() override;
};

class ClockProgramCo : public ace_routine::Coroutine {
    public:
        ClockProgramCo()
            : format24(false) {}
        int runCoroutine() override;

        bool format24;
};

class LoopProgramCo : public ace_routine::Coroutine {
    public:
        LoopProgramCo() {}
        int runCoroutine() override;
};

extern TestProgramCo testProgramCo;
extern InitProgramCo initProgramCo;
extern ClockProgramCo clockProgramCo;
extern LoopProgramCo loopProgramCo;

#endif