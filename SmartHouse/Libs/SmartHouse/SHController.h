#ifndef SHCONTROLLER_H
#define SHCONTROLLER_H

#include "../RingBuffer/RingBuffer.h"
#include "SHEvent.h"
#include "SHCommand.h"

#define EVENT_QUEUE_SIZE 16
#define COMMAND_QUEUE_SIZE 16

typedef bool (*filterCommand_t)(SHCommand* command);
class SHComponent;

class SHController
{
    private:
        RingBuffer<SHEventBuf, EVENT_QUEUE_SIZE> eventQueue;
        RingBuffer<SHCommand, COMMAND_QUEUE_SIZE> commandQueue;
        void processEvent(SHEvent* event);
        void processCommand(SHCommand* command);

    protected:
        //events
        void processEvents();
        //commands
        void processCommands(filterCommand_t filterCommandCallback = NULL);
        bool parseCommand(SHCommand& cmd, const char *const str);
        virtual void initialize() {};
        virtual void processState() {};
        virtual void handleEvent(SHEvent* event) = 0;
        virtual void handleCommand(SHCommand* command) = 0;
    public:
        SHController(SHComponent *const *const components, const uint8_t componentCount);
        volatile unsigned long lastPing;
        //components
        SHComponent *const *const components;
        const uint8_t componentCount;
        SHComponent& getComponent(uint16_t id);
        SHComponent* getComponentByName(const char *const name);
        //
        void setup(unsigned long serialBaudRate = 57600);
        void loop();
        virtual void innerLoop() {};
        virtual void processHttpResponse(const char *const buffer) {}; //TODO: remove from base class
        void pollComponents();
        void longPollComponents();
        SHEventBuf* pushEvent(bool noInterrupts = false);
        SHCommand* pushCommand(SHCommand command);
};

#endif // SHCONTROLLER_H
