#ifndef SHMOTIONSENSOR_H
#define SHMOTIONSENSOR_H

#include "SHButton.h"

class SHMotionSensor: public SHButton
{
    public:
        SHMotionSensor(uint8_t id, const char *const name, const uint8_t pin, bool enabled = true);
};

#endif // SHMOTIONSENSOR_H
