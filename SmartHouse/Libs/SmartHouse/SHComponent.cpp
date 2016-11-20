#include "SHComponent.h"

SHComponent::SHComponent(const uint8_t id, const char *const name, bool enabled):
     id(id), name(name), enabled(enabled), controller(NULL)
{
}

void SHComponent::setup(SHController* controller)
{
    this->controller = controller;
    Serial.print(name); Serial.println(F(" setup"));
}

SHEventBuf* SHComponent::pushEvent(bool noInterrupts) {
    if (controller) {
        return controller->pushEvent(noInterrupts);
    }
	return NULL;
}

int8_t SHComponent::getCommandId(const char *const cmd)
{
    if (strcmp(cmd, "disable") == 0)
    {
        return SHComponent::DISABLE_COMMAND;
    }
    if (strcmp(cmd, "enable") == 0)
    {
        return SHComponent::ENABLE_COMMAND;
    }
    return -1;
}

bool SHComponent::hasState()
{
    return false;
}

uint8_t SHComponent::getState(char *const buf, const uint8_t bufLen)
{
    return 0;
}
