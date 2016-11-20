#include "SHLed.h"

SHLed::SHLed(const uint8_t id, const char *const name, const uint8_t pin, const uint8_t onLevel, bool enabled):
    SHComponent(id, name, enabled), pin(pin), onLevel(onLevel), blinkMode(false), blinkInterval(1000), lastOn(0)
{
}

void SHLed::setup(SHController *const controller)
{
    SHComponent::setup(controller);
    pinMode(pin, OUTPUT);
    off(false);
}

int8_t SHLed::getCommandId(const char *const cmd)
{
    if (strcmp(cmd, "on") == 0)
    {
        return SHLed::ON_COMMAND;
    }
    if (strcmp(cmd, "off") == 0)
    {
        return SHLed::OFF_COMMAND;
    }
    if (strcmp(cmd, "switch") == 0)
    {
        return SHLed::SWITCH_COMMAND;
    }
    if (strcmp(cmd, "blink") == 0)
    {
        return SHLed::BLINK_COMMAND;
    }
    return SHComponent::getCommandId(cmd);
}

int SHLed::state()
{
    return digitalRead(pin) == onLevel;
}

void SHLed::on(bool blinkMode = false)
{
    if (!blinkMode) {
        resetBlinkMode();
        unsigned long t = millis();
        if ((lastOn!=0) && ((t - lastOn) > 100) && ((t - lastOn) < 1000))
        {
            setBlinkMode(t - lastOn);
        }
        lastOn = t;
    }
    digitalWrite(pin, onLevel ? HIGH : LOW);
}

void SHLed::off(bool blinkMode = false)
{
    if (!blinkMode) resetBlinkMode();
    digitalWrite(pin, onLevel ? LOW : HIGH);
}

void SHLed::switchState(bool blinkMode = false)
{
    if (!blinkMode) resetBlinkMode();
    if (state())
    {
        off(blinkMode);
    }
    else
    {
        on(blinkMode);
    }
}

void SHLed::on()
{
    on(false);
}

void SHLed::off()
{
    off(false);
}

void SHLed::switchState()
{
    switchState(false);
}
void SHLed::setBlinkMode(unsigned long blinkInterval) {
    this->blinkInterval = blinkInterval;
    this->blinkMode = true;
}

void SHLed::resetBlinkMode() {
    blinkMode = false;
}

void SHLed::longPoll()
{
    if (!enabled) return;

    if (blinkMode) {
        static unsigned long lastTime = 0;
        if (millis() - lastTime > blinkInterval) {
            switchState(true);
            lastTime = millis();
        }
    }
}

bool SHLed::hasState()
{
    return true;
}

uint8_t SHLed::getState(char *const buf, const uint8_t bufLen)
{
    if (buf==NULL) return 0;
    if (state()) {
        strncpy(buf, "on", bufLen);
        return strlen("on");
    }
    else {
        strncpy(buf, "off", bufLen);
        return strlen("off");
    }
}
