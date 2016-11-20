#ifndef CONTROLLER2_H
#define CONTROLLER2_H

#include "Libs/SmartHouse/SHController.h"
#include "Libs/SmartHouse/SHComponent.h"
#include "Components/SHAlarm.h"
#include "Components/SHButton.h"
#include "Components/SHLed.h"
#include "Components/SHWire.h"

#define SH_ALARM_ID          0
#define SH_LED_TEST_ID       1
#define SH_BUTTON1_ID        2
#define SH_BUTTON2_ID        3
#define SH_BUTTON3_ID        4
#define SH_BUTTON4_ID        5
#define SH_BUTTON5_ID        6
#define SH_BUTTON6_ID        7
#define SH_BUTTON7_ID        8
#define SH_BUTTON8_ID        9
#define SH_BUTTON9_ID        10
#define SH_BUTTON10_ID       11
#define SH_BUTTON11_ID       12
#define SH_BUTTON12_ID       13
#define SH_BUTTON13_ID       14
#define SH_BUTTON14_ID       15
#define SH_BUTTON15_ID       16
#define SH_BUTTON16_ID       17
#define SH_LED1_ID           18
#define SH_LED2_ID           19
#define SH_LED3_ID           20
#define SH_LED4_ID           21
#define SH_LED5_ID           22
#define SH_LED6_ID           23
#define SH_LED7_ID           24
#define SH_LED8_ID           25
#define SH_LED9_ID           26
#define SH_LED10_ID          27
#define SH_LED11_ID          28
#define SH_LED12_ID          29
#define SH_LED13_ID          30
#define SH_LED14_ID          31
#define SH_LED15_ID          32
#define SH_LED16_ID          33
#define SH_WIRE_ID           34

SHComponent *const components[] =
{
    new SHAlarm(SH_ALARM_ID, "Alarm2", 13),
    new SHLed(SH_LED_TEST_ID, "LT", 12, LOW),

    new SHButton(SH_BUTTON1_ID, "B2.1", 23),
    new SHButton(SH_BUTTON2_ID, "B2.2", 25),
    new SHButton(SH_BUTTON3_ID, "B2.3", 27),
    new SHButton(SH_BUTTON4_ID, "B2.4", 29),
    new SHButton(SH_BUTTON5_ID, "B2.5", 31),
    new SHButton(SH_BUTTON6_ID, "B2.6", 33),
    new SHButton(SH_BUTTON7_ID, "B2.7", 35),
    new SHButton(SH_BUTTON8_ID, "B2.8", 37),
    new SHButton(SH_BUTTON9_ID, "B2.9", 39),
    new SHButton(SH_BUTTON10_ID, "B2.10", 41),
    new SHButton(SH_BUTTON11_ID, "B2.11", 43),
    new SHButton(SH_BUTTON12_ID, "B2.12", 45),
    new SHButton(SH_BUTTON13_ID, "B2.13", 47),
    new SHButton(SH_BUTTON14_ID, "B2.14", 49),
    new SHButton(SH_BUTTON15_ID, "B2.15", 51),
    new SHButton(SH_BUTTON16_ID, "B2.16", 53),

    new SHLed(SH_LED1_ID, "L2.1", 22, LOW),
    new SHLed(SH_LED2_ID, "L2.2", 24, LOW),
    new SHLed(SH_LED3_ID, "L2.3", 26, LOW),
    new SHLed(SH_LED4_ID, "L2.4", 28, LOW),
    new SHLed(SH_LED5_ID, "L2.5", 30, LOW),
    new SHLed(SH_LED6_ID, "L2.6", 32, LOW),
    new SHLed(SH_LED7_ID, "L2.7", 34, LOW),
    new SHLed(SH_LED8_ID, "L2.8", 36, LOW),
    new SHLed(SH_LED9_ID, "L2.9", 38, LOW),
    new SHLed(SH_LED10_ID, "L2.10", 40, LOW),
    new SHLed(SH_LED11_ID, "L2.11", 42, LOW),
    new SHLed(SH_LED12_ID, "L2.12", 44, LOW),
    new SHLed(SH_LED13_ID, "L2.13", 46, LOW),
    new SHLed(SH_LED14_ID, "L2.14", 48, LOW),
    new SHLed(SH_LED15_ID, "L2.15", 50, LOW),
    new SHLed(SH_LED16_ID, "L2.16", 52, LOW),
    new SHWire(SH_WIRE_ID, "W2", 1),
};

class Controller2: public SHController
{
    private:
    protected:
        void handleEvent(SHEvent* event);
        void handleCommand(SHCommand* command);
        void initialize();
        void processState();
    public:
        Controller2(SHComponent *const *const components, const uint8_t componentsCount);
};

#endif // CONTROLLER2_H
