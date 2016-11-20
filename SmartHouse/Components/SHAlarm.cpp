#include "SHAlarm.h"

SHAlarm::SHAlarm(const uint8_t id, const char *const name, const uint8_t pin, bool enabled):
    SHLed(id, name, pin, enabled), firstStart(true)
{
}

void SHAlarm::setup(SHController *const controller)
{
    SHLed::setup(controller);
    //on(); //alarm on
}

void SHAlarm::poll()
{
    if (!enabled) return;
    if (firstStart) {
        SHAlarmStartEvent* event = (SHAlarmStartEvent*)pushEvent();
        if (event != NULL) {
            event->componentId = this->id;
            event->eventId = SHAlarmStartEvent::ID;
        }
        firstStart = false;
    }
}
