#ifndef SHEVENT_H
#define SHEVENT_H

#include <inttypes.h>
#define EVENT_DATA_BUF_SIZE 16

struct SHEvent
{
    int16_t componentId;
    int16_t eventId;

    public:
        virtual void init();
};

struct SHEventBuf: SHEvent
{
    char data[EVENT_DATA_BUF_SIZE];
    public:
        virtual void init();
};

#endif // SHEVENT_H
