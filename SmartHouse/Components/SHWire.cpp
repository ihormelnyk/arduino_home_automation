#include "SHWire.h"

#define STATE_BUF_LEN 16

SHWire::SHWire(const uint8_t id, const char *const name, const uint8_t wId, bool enabled):
    SHComponent(id, name, enabled), wId(wId)
{
}

class WireCallback
{
    public:
        static SHController * controller;
        static int8_t wireComponentId;
        volatile static int8_t lastPolledComponentId;

        static void receiveCallback(int len)
        {
            SHWireRecvEvent* event = (SHWireRecvEvent*)controller->pushEvent();
            if (event != NULL) {
                event->componentId = wireComponentId;
                event->eventId = SHWireRecvEvent::ID;
                uint8_t index = 0;
                while(Wire.available())
                {
                    if (index < (SHWireRecvEvent::BUF_SIZE - 1)) {
                        (event->command)[index++] = Wire.read();
                        (event->command)[index] = 0;
                    } else {
                        //read the rest of characters
                        Wire.read();
                    }
                }
            }
        }
        static void requestCallback() //no serial inside
        {
            controller->lastPing = millis();
            for(uint8_t i=0; i<controller->componentCount;i++) {
                if ((lastPolledComponentId) < 0) {
                    lastPolledComponentId = 0;
                } else {
                    lastPolledComponentId++;
                }
                if (lastPolledComponentId >= (controller->componentCount))
                {
                    lastPolledComponentId = -1;
                    Wire.write("");
                    break;
                }

                SHComponent& c = controller->getComponent(lastPolledComponentId);
                static char stateBuf[STATE_BUF_LEN];
                if (c.hasState()) {
                    uint8_t pos = 0;
                    stateBuf[pos]= 0;
                    strncat(stateBuf, c.name, STATE_BUF_LEN - 1);
                    pos +=((strlen(c.name) > (STATE_BUF_LEN - 1)) ? STATE_BUF_LEN - 1 : strlen(c.name));
                    if (pos < (STATE_BUF_LEN - 2)) {
                        stateBuf[pos++]= '=';
                        pos += c.getState(stateBuf + pos, STATE_BUF_LEN - 1 - pos);
                    }
                    stateBuf[pos]= 0;
                    Wire.write(stateBuf);
                    break;
                }
            }
        }
};

SHController* WireCallback::controller = NULL;
int8_t WireCallback::wireComponentId = -1;
volatile int8_t WireCallback::lastPolledComponentId = -1;

bool SHWire::isMaster()
{
    return wId == 0;
}

void SHWire::setup(SHController *const controller)
{
    SHComponent::setup(controller);
    if (isMaster()) {
        Wire.begin();
    } else {
        Wire.begin(wId);
    }
    if (!isMaster()) {
        WireCallback::controller = controller;
        WireCallback::wireComponentId = id;
        Wire.onReceive(WireCallback::receiveCallback);
        Wire.onRequest(WireCallback::requestCallback);
    }
}

void SHWire::send(const uint8_t controllerId, const char *const buf)
{
    Wire.beginTransmission(controllerId);
    size_t n = 0;
    for (uint16_t i = 0; i < strlen(buf); i++) {
        n += Wire.write(buf[i]);
    }
    Wire.endTransmission();
}

unsigned int SHWire::recv(const uint8_t controllerId, char *const buf, const unsigned int bufLen)
{
    if ((buf == NULL) || (bufLen < 1)) return 0;
    unsigned int bufPos = 0;
    buf[bufPos] = 0;
    Wire.requestFrom(controllerId, min(bufLen, 32)); //32b max
    while(Wire.available())  // slave may send less than requested
    {
        if (bufPos >= (bufLen - 1))
        {
            Serial.println("[WIRE] Buffer overflow");
            while(Wire.available()) {
                Wire.read();
            }
            break;
        }
        byte b = Wire.read();
        if (b == 255) {
           buf[bufPos] = 0;
           break;
        }
        buf[bufPos++] = b;
        buf[bufPos] = 0;
    }
    return strlen(buf);
}

int8_t SHWire::getCommandId(const char *const cmd)
{
    if (strcmp(cmd, "send") == 0)
    {
        return SHWire::SEND_COMMAND;
    }
    return SHComponent::getCommandId(cmd);
}
