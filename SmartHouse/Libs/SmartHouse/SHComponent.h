#ifndef SHCOMPONENT_H
#define SHCOMPONENT_H

#include <inttypes.h>
#include "SHController.h"

class SHController;

class SHComponent
{
    protected:
        SHController * controller;
        SHEventBuf* pushEvent(bool noInterrups = false);
    public:
        static const uint8_t DISABLE_COMMAND = 0;
        static const uint8_t ENABLE_COMMAND = 1;

        const uint8_t id;
        const char *const name;
        bool enabled;
        SHComponent(const uint8_t id, const char *const name, bool enabled = true);
        virtual void setup(SHController *const controller);
        virtual void poll() {};
        virtual void longPoll() {};
        virtual int8_t getCommandId(const char *const cmd);
        virtual bool hasState();
        virtual uint8_t getState(char *const buf, const uint8_t bufLen);
};
#endif // SHCOMPONENT_H
