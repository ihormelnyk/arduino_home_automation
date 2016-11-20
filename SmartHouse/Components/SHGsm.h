#ifndef SHGSM_H
#define SHGSM_H

#include "../Libs/SmartHouse/SHComponent.h"
#include "../Libs/GSM/GSM.h"

class SHGSM: public SHComponent, public GSMHandler
{
    public:
        static const uint8_t CALL_COMMAND = 10;
        static const uint8_t HANG_UP_COMMAND = 11;
        static const uint8_t POST_COMMAND = 12;
        static const uint8_t DELETE_ALL_SMS_COMMAND = 13;
        static const uint8_t POLL_COMMAND = 14;

        uint8_t powerPin;
        uint8_t resetPin;
        unsigned long baud_rate;
        SHGSM(uint8_t id, const char *const name, unsigned long baud_rate = 115200, uint8_t powerPin = GSM_ON_OFF_PIN, uint8_t resetPin = GSM_RESET_PIN, bool enabled = true);
        void setup(SHController *const controller);
        virtual int8_t getCommandId(const char *const cmd);
        void longPoll();
        bool canCall();
        bool canHttp();
        bool call();
        void hangUp();
        void deleteAllSMS();
        char *const getHttpBuffer();
        uint16_t getHttpBufferLen();
		void postAsync(const char *const arg);
        void pollAsync();
		void onCall(const char *const number, const char *const name);
		void onSMS(const char *const number, const char *const name, const char *const sms);
        void innerLoop();
        void processHttpResponse(const char *const buffer);
    private:
        GSM gsm;
};


struct SHGSMCallEvent: SHEvent
{
    static const uint8_t ID = 1;
    bool isAdmin;
};

struct SHGSMSmsEvent: SHEvent
{
    static const uint8_t ID = 2;
    static const uint8_t BUF_SIZE = EVENT_DATA_BUF_SIZE - sizeof(bool);
    bool isAdmin;
    char command[SHGSMSmsEvent::BUF_SIZE];
};
#endif // SHGSM_H
