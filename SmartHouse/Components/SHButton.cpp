#include "SHButton.h"
#include "../Libs/SmartHouse/SHEvent.h"

SHButton::SHButton(const uint8_t id, const char *const name, const uint8_t pin, bool enabled):
    SHComponent(id, name, enabled), pin(pin)
{
    interval_millis = 10;
    previous_millis = 0;
}

void SHButton::setup(SHController *const controller)
{
    SHComponent::setup(controller);
    pinMode(pin, INPUT);
    state = unstableState = digitalRead(pin);
}

void SHButton::poll()
{
    if (!enabled) return;
    if (digitalRead(pin) != unstableState) {
        unstableState = !unstableState;
        previous_millis = millis();
	}
    if ((unstableState!=state) && ((millis()-previous_millis)>=interval_millis))
    {
        SHButtonStateEvent* event = (SHButtonStateEvent*)pushEvent();
        if (event != NULL) {
            event->componentId = this->id;
            event->eventId = SHButtonStateEvent::ID;
            event->oldState = state;
            event->newState = unstableState;
        }
        state = unstableState;
    }
}
