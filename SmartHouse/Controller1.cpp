#include <avr/wdt.h>
#include "Controller1.h"

Controller1::Controller1(SHComponent *const *const components, const uint8_t componentsCount) :
SHController(components, componentsCount), lastMotionCall(0)
{
};

void Controller1::initialize()
{
	//on alarm
	SHAlarm &alarm = (SHAlarm&)getComponent(SH_ALARM_ID);
	alarm.on();
}

void Controller1::handleEvent(SHEvent* event)
{
	switch (event->componentId)
	{
	case SH_ALARM_ID:
	{
		if (event->eventId == SHAlarmStartEvent::ID) {
			Serial.println(F("Alarm started"));
			SHAlarm &alarm = (SHAlarm&)getComponent(event->componentId);
			pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, alarm.state() ? "Alarm is on" : "Alarm is off"));
		}
	}
	break;
	case SH_GSM_ID:
	{
		SHGSM &gsm = (SHGSM&)getComponent(event->componentId);
		if (event->eventId == SHGSMCallEvent::ID) {
			bool isAdmin = ((SHGSMCallEvent*)event)->isAdmin;
			Serial.print(F("Call: ")); Serial.println(isAdmin ? F("Admin") : F("Unknown"));
			pushCommand(SHCommand(SH_GSM_ID, SHGSM::HANG_UP_COMMAND));
			//If the incoming call is from an authorized number
			if (isAdmin)
			{
				SHAlarm &alarm = (SHAlarm&)getComponent(SH_ALARM_ID);
				if (alarm.state()) {
					pushCommand(SHCommand(SH_ALARM_ID, SHAlarm::OFF_COMMAND));
				}
				else {
					pushCommand(SHCommand(SH_ALARM_ID, SHAlarm::ON_COMMAND));
				}
			}
		}
		else if (event->eventId == SHGSMSmsEvent::ID) {
			SHGSMSmsEvent* smsEvent = (SHGSMSmsEvent*)event;
			Serial.print(F("SMS Cmd: "));
			Serial.println(smsEvent->command);
			if (smsEvent->isAdmin)
			{
				SHCommand cmd;
				if (parseCommand(cmd, smsEvent->command)) {
					pushCommand(cmd);
				}
				else {
					pushCommand(SHCommand(SH_WIRE_ID, SHWire::SEND_COMMAND, smsEvent->command));
				}
			}
			pushCommand(SHCommand(SH_GSM_ID, SHGSM::DELETE_ALL_SMS_COMMAND));
		}
	}
	break;
	case SH_BUTTON1_ID:
	case SH_BUTTON2_ID:
	case SH_BUTTON3_ID:
	case SH_BUTTON4_ID:
	case SH_BUTTON5_ID:
	case SH_BUTTON6_ID:
	case SH_BUTTON7_ID:
	case SH_BUTTON8_ID:
	case SH_BUTTON9_ID:
	case SH_BUTTON10_ID:
	case SH_BUTTON11_ID:
	case SH_BUTTON12_ID:
	case SH_BUTTON13_ID:
	case SH_BUTTON14_ID:
	case SH_BUTTON15_ID:
	case SH_BUTTON16_ID:
		if (event->eventId == SHButtonStateEvent::ID && ((SHButtonStateEvent*)event)->newState == HIGH)
		{
			SHButton &b = (SHButton&)getComponent(event->componentId);
			Serial.print(F("Pressed button ")); Serial.println(b.name);
			pushCommand(SHCommand((event->componentId) + 16, SHLed::SWITCH_COMMAND));
		}
		break;
	case SH_MOTION_SENSOR1_ID:
	case SH_MOTION_SENSOR2_ID:
	case SH_MOTION_SENSOR3_ID:
    case SH_MOTION_SENSOR4_ID:
		if (event->eventId == SHButtonStateEvent::ID && ((SHButtonStateEvent*)event)->newState == LOW)
		{
			SHMotionSensor &s = (SHMotionSensor&)getComponent(event->componentId);
			Serial.print(F("Motion detected on sensor ")); Serial.println(s.name);
			static const unsigned long callInterval = 60000;
			static const unsigned long postInterval = 60000;
			static unsigned long lastMotionPost = 0;

			SHAlarm &alarm = (SHAlarm&)getComponent(SH_ALARM_ID);
			if (alarm.state()) //alarm is on
			{
				if ((lastMotionCall == 0) || (millis() - lastMotionCall > callInterval)) {
					Serial.println(F("Push call on motion"));
					pushCommand(SHCommand(SH_GSM_ID, SHGSM::CALL_COMMAND));
					lastMotionCall = millis();
				}
				if ((lastMotionPost == 0) || (millis() - lastMotionPost > postInterval)) {
					Serial.println(F("Push post on motion"));
					pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, s.name));
					lastMotionPost = millis();
				}
			}
		}
		break;
	}
};

void Controller1::handleCommand(SHCommand* command)
{
	if (command->id == SHComponent::DISABLE_COMMAND)
	{
		SHComponent &component = (SHComponent&)getComponent(command->componentId);
		Serial.print(F("Disable ")); Serial.println(component.name);
		component.enabled = false;
		return;
	}
	if (command->id == SHComponent::ENABLE_COMMAND)
	{
		SHComponent &component = (SHComponent&)getComponent(command->componentId);
		Serial.print(F("Enable ")); Serial.println(component.name);
		component.enabled = true;
		return;
	}


	switch (command->componentId)
	{
	case SH_ALARM_ID:
	{
		SHAlarm &alarm = (SHAlarm&)getComponent(command->componentId);
		if (command->id == SHAlarm::ON_COMMAND)
		{
			Serial.println(F("Alarm on"));
			alarm.on();
			pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, "Alarm ON"));
		}
		else if (command->id == SHAlarm::OFF_COMMAND)
		{
			Serial.println(F("Alarm off"));
			alarm.off();
			pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, "Alarm OFF"));
		}
	}
	break;
	case SH_GSM_ID:
	{
		SHGSM &gsm = (SHGSM&)getComponent(command->componentId);
		//if (true) {} else
		if (command->id == SHGSM::CALL_COMMAND)
		{
			if (gsm.canCall()) {
				Serial.println(F("Call to Admin"));
				gsm.call();
			}
			else {
				Serial.println(F("GSM is busy for call. Resending command..."));
				SHCommand* cmd = pushCommand(SHCommand(SH_GSM_ID, SHGSM::CALL_COMMAND));
				if (cmd != NULL) {
					cmd->time = millis();
					cmd->delay = 1000;
				}
			}
		}
		else if (command->id == SHGSM::HANG_UP_COMMAND)
		{
			Serial.println(F("Call HangUp"));
			gsm.hangUp();
		}
		else if (command->id == SHGSM::POLL_COMMAND)
		{
			if (gsm.canHttp()) {
				Serial.println(F("GPRS CMD POLL"));
				SHWire &wire = (SHWire&)getComponent(SH_WIRE_ID);
				char *const comBuf = gsm.getHttpBuffer();
				uint16_t comBufPos = 0;
				comBuf[comBufPos] = 0;
				static bool mainState = true;
				if (mainState) {
					mainState = false;
					for (uint8_t i = 0; i < componentCount; i++) {
						SHComponent& c = getComponent(i);
						if (c.hasState()) {
							strncat(comBuf + comBufPos, c.name, min(strlen(c.name), gsm.getHttpBufferLen() - comBufPos - 1));
							comBufPos += strlen(c.name);
							comBuf[comBufPos++] = '=';
							comBufPos += c.getState(comBuf + comBufPos, gsm.getHttpBufferLen() - comBufPos - 1);
							if (comBufPos < gsm.getHttpBufferLen()) {
                                comBuf[comBufPos++] = '&';
							}
						}
					}
					if (comBufPos > 0)
					{
						comBufPos--;
						comBuf[comBufPos] = 0;
					}
					Serial.println(comBuf);
					Serial.print(F("[C1]: state length:")); Serial.println(comBufPos);
				}
				else {
					mainState = true;
					const int CONTROLLER2_ID = 1;
					static bool controller2Available = true;
					for (uint8_t i = 0; i <= 127; i++)
					{
						uint8_t len = wire.recv(CONTROLLER2_ID, comBuf + comBufPos, gsm.getHttpBufferLen() - comBufPos - 1);
						if (len > 0) {
							Serial.print(F("[C2]: "));
							Serial.println(comBuf + comBufPos);
							comBufPos += len;
							comBuf[comBufPos++] = '&';
						}
						else {
							if (comBufPos > 0)
							{
								comBufPos--;
								comBuf[comBufPos] = 0;
							}
							break;
						}
					}
					Serial.print(F("[C2]: state length:")); Serial.println(comBufPos);
					if (controller2Available != (comBufPos > 0)) {
					    controller2Available = !controller2Available;
					    pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, controller2Available ? "C2 is on" : "C2 is off"));
					}
				}
				gsm.pollAsync();
			}
		}
		else if (command->id == SHGSM::POST_COMMAND)
		{
			if (gsm.canHttp()) {
				Serial.println(F("GPRS POST"));
				gsm.postAsync(command->args);
			}
			else {
				Serial.println(F("GSM is busy for post. Resending command..."));
				SHCommand* cmd = pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, command->args));
				if (cmd != NULL) {
					cmd->time = millis();
					cmd->delay = 1000;
				}
			}
		}
		else if (command->id == SHGSM::DELETE_ALL_SMS_COMMAND)
		{
			Serial.println(F("Delete All SMS"));
			gsm.deleteAllSMS();
		}
	}
	break;
	case SH_WIRE_ID:
	{
		SHWire &wire = (SHWire&)getComponent(command->componentId);
		if (command->id == SHWire::SEND_COMMAND)
		{
			const int CONTROLLER2_ID = 1;
			wire.send(CONTROLLER2_ID, command->args);
		}
	}
	break;
	case SH_LED1_ID:
	case SH_LED2_ID:
	case SH_LED3_ID:
	case SH_LED4_ID:
	case SH_LED5_ID:
	case SH_LED6_ID:
	case SH_LED7_ID:
	case SH_LED8_ID:
	case SH_LED9_ID:
	case SH_LED10_ID:
	case SH_LED11_ID:
	case SH_LED12_ID:
	case SH_LED13_ID:
	case SH_LED14_ID:
	case SH_LED15_ID:
	case SH_LED16_ID:
	case SH_SIGNAL_ID:
	{
		SHLed &led = (SHLed&)getComponent(command->componentId);
		if (command->id == SHLed::ON_COMMAND)
		{
			led.on();
		}
		else if (command->id == SHLed::OFF_COMMAND)
		{
			led.off();
		}
		else if (command->id == SHLed::SWITCH_COMMAND)
		{
			led.switchState();
		}
		else if (command->id == SHLed::BLINK_COMMAND)
		{
			led.setBlinkMode(1000);
		}
	}
	break;
	}
};

void Controller1::processState() {
	//procedure for check motion sensors state
	static const unsigned long motionAlarmInterval = 60000;
	static unsigned long previousMotionStateCheck = 0;
	if (millis() - previousMotionStateCheck > motionAlarmInterval)
	{
		previousMotionStateCheck = millis();
		bool needAlarm = false;
		for (int i = SH_MOTION_SENSOR1_ID; i <= SH_MOTION_SENSOR4_ID; i++) {
			SHMotionSensor &m = (SHMotionSensor&)getComponent(i);
			if (m.enabled && !m.state) {
				Serial.print(F("Motion ")); Serial.println(m.name);
				needAlarm = true;
			}
		}
		if (needAlarm) {
			SHAlarm &alarm = (SHAlarm&)getComponent(SH_ALARM_ID);
			if (alarm.state()) { //if alarm is on
				if (previousMotionStateCheck - lastMotionCall > motionAlarmInterval) {
					pushCommand(SHCommand(SH_GSM_ID, SHGSM::CALL_COMMAND));
					//pushCommand(SHCommand(SH_GSM_ID, SHGSM::POST_COMMAND, "call"));
				}
				else {
					previousMotionStateCheck = lastMotionCall;
				}
			}
		}
	}
	//Poll
	static unsigned long previousPoll = 0;
	if (millis() - previousPoll > 10000) {
		pushCommand(SHCommand(SH_GSM_ID, SHGSM::POLL_COMMAND));
		previousPoll = millis();
	}
}

bool isNonGsmCommand(SHCommand* command) {
	return command->componentId != SH_GSM_ID;
}

void Controller1::innerLoop() {
	//non GSM actions
	wdt_reset();
	processEvents();
	processState();
	processCommands(isNonGsmCommand);
}

void Controller1::processHttpResponse(const char *const buffer) {
    if (strlen(buffer) == 0) return;
    Serial.print(F("Received command: ")); Serial.println(buffer);
    SHCommand cmd;
    if (parseCommand(cmd, buffer)) {
        pushCommand(cmd);
    }
    else {
        pushCommand(SHCommand(SH_WIRE_ID, SHWire::SEND_COMMAND, buffer));
    }
}
