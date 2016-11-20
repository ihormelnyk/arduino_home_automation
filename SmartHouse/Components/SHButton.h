#ifndef SHBUTTON_H
#define SHBUTTON_H

#include "../Libs/SmartHouse/SHComponent.h"

class SHButton: public SHComponent
{
    public:
        const uint8_t pin;
        volatile uint8_t state;
        SHButton(uint8_t id, const char *const name, const uint8_t pin, bool enabled = true);
        virtual void setup(SHController *const controller);
        void poll();
    private:
        uint8_t interval_millis;
        volatile uint8_t unstableState;
        volatile unsigned long previous_millis;
};

struct SHButtonStateEvent: SHEvent
{
    static const uint8_t ID = 1;

    bool oldState;
    bool newState;
};

#endif // SHBUTTON_H
