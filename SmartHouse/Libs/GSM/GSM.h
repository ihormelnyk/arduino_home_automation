#define GSM_ON_OFF_PIN          8
#define GSM_RESET_PIN           9
#define GSM_URL_BUF_LEN         128
#define GSM_HTTP_BUF_LEN        256

#include <Arduino.h>
#include "GSMCommands.h"
#include "GSMHandler.h"

class GSM: public GSMSerialHandler
{
    private:
        GSMSerial serial;
        GSMHandler* handler;
        GSMDebug debug;

        //state
        int8_t powerState;
        int8_t networkState;
        int8_t GPRSState;
        int8_t bearerState;
        int8_t httpState;
        int8_t httpParamState;
        int8_t httpRequestState;
        //
        unsigned long lastPowerOn;
        unsigned long lastHttpRequest;
        bool powerInProgress;
        bool callInProgress;
        bool httpInProgress;
        int httpMethod;
        char httpUrl[GSM_URL_BUF_LEN];
        char httpBuffer[GSM_HTTP_BUF_LEN];
        //
        bool recvRDY();
        void powerOnStep1();
        void powerOnStep2();
        bool powerOn();
		bool handleURC(const char *const str);
        void innerLoop();
        void processPower();
        void processHttp();
        void processHttpResponse();
    public:
        int powerPin;
        int resetPin;
        GSM(GSMHandler* handler);
        bool setup(unsigned long baud_rate, uint8_t powerPin = GSM_ON_OFF_PIN, uint8_t resetPin = GSM_RESET_PIN);
        void process();

        bool canCall();
        bool canHttp();
        bool call(int cellNumber);
        void hangUp();
        void deleteAllSMS();
        void powerOnAsync();
		void cleanHttpBuffer();
		char *const getHttpBuffer();
		uint16_t getHttpBufferLen();
		int httpRequestAsync(const char *const url, bool isPost);
};
