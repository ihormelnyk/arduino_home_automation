#define GSM_BUF_LEN           128
#define GSM_SERIAL Serial1

#include <Arduino.h>
#include "GSMDebug.h"
#include "GSMSerialHandler.h"

class GSMSerial
{
    private:
        char buf[GSM_BUF_LEN];
        bool isSerialBusy;
        bool isInnerLoop;
        GSMSerialHandler* serialHandler;
        GSMDebug &debug;
        void innerLoop();
        bool checkInnerLoop();
        bool handleUnsolicitedCode(const char *const str);
    public:
        GSMSerial(GSMDebug& debug, GSMSerialHandler* serialHandler);
        void begin(unsigned long baud_rate);
        int available();
        int read();
        bool write(const char *const data);
        const char *const recvLine(unsigned int timeout, bool optionalCrLfAtStart = false);
        bool recvData(unsigned int data_len, char* responseBuf, unsigned int responseBufLength, unsigned int timeout);
        const char *const recvUnsolicitedCode(const char *const codePrefix, unsigned int timeout);
		bool sendATCommand(const char *const command, bool checkEcho = true);
        void process();
        void flush();
};
