#ifndef SHCOMMAND_H
#define SHCOMMAND_H

#include <Arduino.h>

#define ARGS_BUF_LEN 16

struct SHCommand
{
    int8_t componentId;
    uint8_t id;
    char args[ARGS_BUF_LEN];
    unsigned long time; //ms
    unsigned int delay; //ms
    SHCommand();
    SHCommand(const int8_t componentId, const uint8_t id);
    SHCommand(const int8_t componentId, const uint8_t id, const char *const args);
    void setArgs(const char *const args, size_t count = ARGS_BUF_LEN - 1);
};

#endif // SHCOMMAND_H
