#include "SHMotionSensor.h"

SHMotionSensor::SHMotionSensor(uint8_t id, const char *const name, const uint8_t pin, bool enabled):
    SHButton(id, name, pin, enabled)
{
}

