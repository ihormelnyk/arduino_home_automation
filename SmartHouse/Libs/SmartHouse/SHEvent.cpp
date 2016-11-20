#include "SHEvent.h"

void SHEvent::init()
{
    this->componentId = -1;
    this->eventId = -1;
}

void SHEventBuf::init()
{
    SHEvent::init();
    for (uint8_t i=0;i<EVENT_DATA_BUF_SIZE;i++) {
        this->data[i]=0;
    }
}
