#ifndef CONTROLLER1_H
#define CONTROLLER1_H

#include "Libs/SmartHouse/SHController.h"
#include "Libs/SmartHouse/SHComponent.h"
#include "Components/SHAlarm.h"
#include "Components/SHButton.h"
#include "Components/SHLed.h"
#include "Components/SHGSM.h"
#include "Components/SHMotionSensor.h"
#include "Components/SHWire.h"

#define SH_ALARM_ID          0
#define SH_GSM_ID            1
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

#define SH_MOTION_SENSOR1_ID 34
#define SH_MOTION_SENSOR2_ID 35
#define SH_MOTION_SENSOR3_ID 36
#define SH_MOTION_SENSOR4_ID 37
#define SH_WIRE_ID 38
#define SH_SIGNAL_ID 39

SHComponent *const components[] =
{
    new SHAlarm(SH_ALARM_ID, "Alarm1", 13),
    //1. setup baud rate AT+IPR=57600  //115200 doesnt work properly if there is quite long intterupt
    //2. echo on
    //3. setup bearer 1 AT+SAPBR=3,1,"APN","www.ab.kyivstar.net"
    //4. save bearer to NVRAM AT+SAPBR=5,1
    //5. enable CLI notification AT+CLIP=1

    new SHGSM(SH_GSM_ID, "GSM", 57600, 10, 11), //8,9,18,19

    new SHButton(SH_BUTTON1_ID, "B1.1", 23),
    new SHButton(SH_BUTTON2_ID, "B1.2", 25),
    new SHButton(SH_BUTTON3_ID, "B1.3", 27),
    new SHButton(SH_BUTTON4_ID, "B1.4", 29),
    new SHButton(SH_BUTTON5_ID, "B1.5", 31),
    new SHButton(SH_BUTTON6_ID, "B1.6", 33),
    new SHButton(SH_BUTTON7_ID, "B1.7", 35),
    new SHButton(SH_BUTTON8_ID, "B1.8", 37),
    new SHButton(SH_BUTTON9_ID, "B1.9", 39),
    new SHButton(SH_BUTTON10_ID, "B1.10", 41),
    new SHButton(SH_BUTTON11_ID, "B1.11", 43),
    new SHButton(SH_BUTTON12_ID, "B1.12", 45),
    new SHButton(SH_BUTTON13_ID, "B1.13", 47),
    new SHButton(SH_BUTTON14_ID, "B1.14", 49),
    new SHButton(SH_BUTTON15_ID, "B1.15", 51),
    new SHButton(SH_BUTTON16_ID, "B1.16", 53),

    new SHLed(SH_LED1_ID, "L1.1", 22, LOW),
    new SHLed(SH_LED2_ID, "L1.2", 24, LOW),
    new SHLed(SH_LED3_ID, "L1.3", 26, LOW),
    new SHLed(SH_LED4_ID, "L1.4", 28, LOW),
    new SHLed(SH_LED5_ID, "L1.5", 30, LOW),
    new SHLed(SH_LED6_ID, "L1.6", 32, LOW),
    new SHLed(SH_LED7_ID, "L1.7", 34, LOW),
    new SHLed(SH_LED8_ID, "L1.8", 36, LOW),
    new SHLed(SH_LED9_ID, "L1.9", 38, LOW),
    new SHLed(SH_LED10_ID, "L1.10", 40, LOW),
    new SHLed(SH_LED11_ID, "L1.11", 42, LOW),
    new SHLed(SH_LED12_ID, "L1.12", 44, LOW),
    new SHLed(SH_LED13_ID, "L1.13", 46, LOW),
    new SHLed(SH_LED14_ID, "L1.14", 48, LOW),
    new SHLed(SH_LED15_ID, "L1.15", 50, LOW),
    new SHLed(SH_LED16_ID, "L1.16", 52, LOW),

    new SHMotionSensor(SH_MOTION_SENSOR1_ID, "M1", 3),
    new SHMotionSensor(SH_MOTION_SENSOR2_ID, "M2", 4),
    new SHMotionSensor(SH_MOTION_SENSOR3_ID, "M3", 5),
    new SHMotionSensor(SH_MOTION_SENSOR4_ID, "M4", 6),
    new SHWire(SH_WIRE_ID, "W1", 0),
    new SHLed(SH_SIGNAL_ID, "S", 12, LOW),
};

class Controller1: public SHController
{
    private:
        unsigned long lastMotionCall;
    protected:
        void handleEvent(SHEvent* event);
        void handleCommand(SHCommand* command);
        void initialize();
        void processState();
    public:
        Controller1(SHComponent *const *const components, const uint8_t componentsCount);
        void innerLoop();
        void processHttpResponse(const char *const buffer);
};

#endif // CONTROLLER1_H
