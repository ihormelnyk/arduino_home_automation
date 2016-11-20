#include <Arduino.h>
#include <Wire.h> //SHWire component
#include <avr/wdt.h> //Watchdog timer
#include "Libs/TimerOne/TimerOne.h" //Interrupt timer

#define POLL_COMPONENTS_INTERVAL 10000  //microseconds
#define SERIAL_BAUD_RATE 57600 //115200 isn't recommended due to troubles with interrupt processing

#define CONTROLLER1_ID 0
#define CONTROLLER2_ID 1
#define CONTROLLER3_ID 2

#include "Controller1.h"
Controller1 controller(components, sizeof(components)/sizeof(*components));

void pollComponents()
{
    //avoid using I/O inside - cause hangs
    controller.pollComponents();
}

void setup() 
{
    controller.setup(SERIAL_BAUD_RATE);
    Timer1.initialize(POLL_COMPONENTS_INTERVAL);
    Timer1.attachInterrupt(pollComponents);
    wdt_enable(WDTO_8S);
}

void loop()
{
    wdt_reset();
    controller.loop();
}
