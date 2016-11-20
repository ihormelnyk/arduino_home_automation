#ifndef SHALARM_H
#define SHALARM_H

#include "SHLed.h"

struct SHAlarmStartEvent: SHEvent
{
    static const uint8_t ID = 1;
};

class SHAlarm: public SHLed
{
    private:
        volatile bool firstStart;
    public:
        SHAlarm(uint8_t id, const char *const name, const uint8_t pin, bool enabled = true);
        void setup(SHController *const controller);
        void poll();

};

#endif // SHALARM_H
