#ifndef SHLED_H
#define SHLED_H

#include "../Libs/SmartHouse/SHComponent.h"

class SHLed: public SHComponent
{
    private:
        bool blinkMode;
        unsigned long lastOn;
        unsigned long blinkInterval;
        void on(bool blinkMode);
        void off(bool blinkMode);
        void switchState(bool blinkMode);
    public:
         static const uint8_t ON_COMMAND = 10;
         static const uint8_t OFF_COMMAND = 11;
         static const uint8_t SWITCH_COMMAND = 12;
         static const uint8_t BLINK_COMMAND = 13;

         const uint8_t pin;
         const uint8_t onLevel;
         SHLed(uint8_t id, const char *const name, const uint8_t pin, const uint8_t onLevel = HIGH, bool enabled = true);
         virtual void setup(SHController *const controller);
         void longPoll();
         virtual int8_t getCommandId(const char *const cmd);
         int state();
         void on();
         void off();
         void switchState();
         void setBlinkMode(unsigned long blinkInterval = 1000);
         void resetBlinkMode();
         bool hasState();
         uint8_t getState(char *const buf, const uint8_t bufLen);
};

#endif // SHLED_H
