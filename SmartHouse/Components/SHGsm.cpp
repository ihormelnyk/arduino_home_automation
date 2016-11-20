#include "SHGSM.h"

SHGSM::SHGSM(const uint8_t id, const char *const name, unsigned long baud_rate, uint8_t powerPin, uint8_t resetPin, bool enabled):
     SHComponent(id, name, enabled),
     baud_rate(baud_rate),
     powerPin(powerPin),
     resetPin(resetPin),
     gsm(this)
{
}

void SHGSM::setup(SHController *const controller)
{
    SHComponent::setup(controller);
    if (gsm.setup(baud_rate, powerPin, resetPin)){ //for IComSat power - 9, reset - 8
        Serial.println(F("GSM is READY"));
    }
    else
    {
        Serial.println(F("GSM setup failed"));
    }
}

void SHGSM::innerLoop() {
    controller->innerLoop();
}

void SHGSM::processHttpResponse(const char *const buffer) {
    controller->processHttpResponse(buffer);
}

int8_t SHGSM::getCommandId(const char *const cmd)
{
    if (strcmp(cmd, "call") == 0)
    {
        return SHGSM::CALL_COMMAND;
    }
    if (strcmp(cmd, "hangUp") == 0)
    {
        return SHGSM::HANG_UP_COMMAND;
    }
    if (strcmp(cmd, "post") == 0)
    {
        return SHGSM::POST_COMMAND;
    }
    if (strcmp(cmd, "poll") == 0)
    {
        return SHGSM::POLL_COMMAND;
    }
    if (strcmp(cmd, "deleteAllSMS") == 0)
    {
        return SHGSM::DELETE_ALL_SMS_COMMAND;
    }
    return SHComponent::getCommandId(cmd);
}

void SHGSM::onCall(const char *const number, const char *const name)
{
    noInterrupts();
    SHGSMCallEvent* event = (SHGSMCallEvent*)pushEvent();
    if (event != NULL) {
        event->componentId = this->id;
        event->eventId = SHGSMCallEvent::ID;
        event->isAdmin = name && (strcmp(name, "Admin") == 0);
    }
    interrupts();
}

void SHGSM::onSMS(const char *const number, const char *const name, const char *const sms)
{
    Serial.print(F("SMS: ")); Serial.print(number);
    Serial.print(F(" ")); Serial.print(name);
    Serial.print(F(" ")); Serial.println(sms);

    bool isAdmin = name && (strcmp(name, "Admin") == 0);
    if (isAdmin) {
    Serial.println("admin");
    }
    noInterrupts();
    SHGSMSmsEvent* event = (SHGSMSmsEvent*)pushEvent();
    if (event != NULL) {
        event->componentId = this->id;
        event->eventId = SHGSMSmsEvent::ID;
		event->isAdmin = isAdmin;
		char* cmd = ((SHGSMSmsEvent*)event)->command;
		strcpy(cmd, "");
		if (sms) {
			strncat(cmd, sms, SHGSMSmsEvent::BUF_SIZE);
		}
    }
    interrupts();
}

void SHGSM::longPoll()
{
    if (!enabled) return;
    gsm.process();
}

bool SHGSM::canCall()
{
    return gsm.canCall();
}

bool SHGSM::canHttp()
{
    return gsm.canHttp();
}

bool SHGSM::call()
{
    if (gsm.call(1)) {
        return true;
    }
    return false;
}

void SHGSM::hangUp()
{
    gsm.hangUp();
}

void SHGSM::deleteAllSMS()
{
    gsm.deleteAllSMS();
}

char *const SHGSM::getHttpBuffer() {
    return gsm.getHttpBuffer();
}

uint16_t SHGSM::getHttpBufferLen() {
    return gsm.getHttpBufferLen();
}

void SHGSM::postAsync(const char *const arg)
{
    //symbol / is required at the end
	const int urlLen = 128;
	char url[urlLen];
	if (strlen(arg) >= urlLen - 45) return;
	sprintf(url, "http://ihormelnyk.com/smarthouse/movement/%s/", arg);
	gsm.cleanHttpBuffer();
    gsm.httpRequestAsync(url, 1);
}

void SHGSM::pollAsync()
{
    gsm.httpRequestAsync("http://ihormelnyk.com/smarthouse/poll/", 1);
}
