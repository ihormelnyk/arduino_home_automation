#ifndef SHWIRE_H
#define SHWIRE_H

#include <Wire.h>
#include "../Libs/SmartHouse/SHComponent.h"

//!!! only one SHWire component instance per controller allowed

class SHWire: public SHComponent
{
    private:
        const uint8_t wId;
    public:
        static const uint8_t SEND_COMMAND = 10;

        SHWire(uint8_t id, const char *const name, const uint8_t wId, bool enabled = true);
        virtual void setup(SHController *const controller);
        virtual int8_t getCommandId(const char *const cmd);
        bool isMaster();
        void send(const uint8_t controllerId, const char *const buf);
        unsigned int recv(const uint8_t controllerId, char *const buf, const unsigned int bufLen);
};

struct SHWireRecvEvent: SHEvent
{
    static const uint8_t ID = 1;
    static const uint8_t BUF_SIZE = EVENT_DATA_BUF_SIZE;
    char command[SHWireRecvEvent::BUF_SIZE];
};

#endif // SHWIRE_H
