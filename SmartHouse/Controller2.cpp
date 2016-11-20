#include "Controller2.h"

Controller2::Controller2(SHComponent *const *const components, const uint8_t componentsCount):
    SHController(components, componentsCount)
{
}

void Controller2::initialize()
{
    //on alarm
    SHAlarm &alarm = (SHAlarm&)getComponent(SH_ALARM_ID);
    alarm.on();
}

void Controller2::handleEvent(SHEvent* event)
{
    switch(event->componentId)
    {
        case SH_ALARM_ID:
            {
                if (event->eventId == SHAlarmStartEvent::ID) {
                    Serial.println(F("Alarm started"));
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
        case SH_WIRE_ID:
            if (event->eventId == SHWireRecvEvent::ID) {
                SHWireRecvEvent* recvEvent = (SHWireRecvEvent*)event;
                Serial.print(F("WIRE Cmd: "));
                Serial.println(recvEvent->command);
                SHCommand cmd;
                if (parseCommand(cmd, recvEvent->command)) {
                    pushCommand(cmd);
                }
                else {
                    Serial.println(F("Command isn't recognized"));
                }
            }
            break;
    }
};

void Controller2::handleCommand(SHCommand* command)
{
    switch(command->componentId)
    {
        case SH_ALARM_ID:
            {
                SHAlarm &alarm = (SHAlarm&)getComponent(command->componentId);
                if (command->id == SHAlarm::SWITCH_COMMAND)
                {
                    int state = alarm.state();
                    if (state)
                    {
                        Serial.println(F("Alarm off"));
                        alarm.off();
                    }
                    else
                    {
                        Serial.println(F("Alarm on"));
                        alarm.on();
                    }
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

void Controller2::processState() {
    static const unsigned long pingInterval = 300000;
    noInterrupts();
    unsigned long lastPingValue = lastPing;
    interrupts();

    //was first ping & interval
    if((lastPingValue != 0) && ((millis() - lastPingValue) > pingInterval))
    {
        lastPing = millis();
        Serial.println(F("Wire ping time out. Resetting Controller 1"));
        SHLed &controller1Reset = (SHLed&)getComponent(SH_LED_TEST_ID);
        controller1Reset.on();
        delay(2);
        controller1Reset.off();
    }
}
