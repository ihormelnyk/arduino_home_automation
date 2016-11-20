#include "SHCommand.h"

SHCommand::SHCommand():
    componentId(-1),
    id(-1),
    time(0),
    delay(0)
{
}

SHCommand::SHCommand(const int8_t componentId, const uint8_t id):
    componentId(componentId),
    id(id),
    time(0),
    delay(0)
{
}

SHCommand::SHCommand(const int8_t componentId, const uint8_t id, const char *const args):
    componentId(componentId),
    id(id),
    time(0),
    delay(0)
{
    setArgs(args);
}

void SHCommand::setArgs(const char *const args, size_t count)
{
    if (ARGS_BUF_LEN > 0) {
        this->args[0] = 0;
        strncat(this->args, args, min(count, ARGS_BUF_LEN - 1));
    }
}
