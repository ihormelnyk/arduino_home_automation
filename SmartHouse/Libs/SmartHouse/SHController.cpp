#include "SHComponent.h"
#include "SHController.h"

SHController::SHController(SHComponent *const *const components, const uint8_t componentCount):
    components(components),
    componentCount(componentCount),
    lastPing(0)
{
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void SHController::setup(unsigned long serialBaudRate)
{
    Serial.begin(serialBaudRate);
    Serial.println(F("--Start setup--"));
    for (int i=0; i<componentCount; i++)
    {
        components[i]->setup(this);
    }

    initialize();

    Serial.println(F("--End setup--"));
    Serial.print(F("Free RAM: ")); Serial.println(freeRam());
}

SHComponent& SHController::getComponent(uint16_t id)
{
    return *(components[id]);
}

SHComponent* SHController::getComponentByName(const char *const name)
{
    for (int i=0; i<componentCount; i++)
    {
        if (strcmp(components[i]->name, name)==0)
        {
            return (components[i]);
        }
    }
    return NULL;
}

SHEventBuf* SHController::pushEvent(bool noInterrups) {
    if (noInterrups) noInterrupts();
    SHEventBuf* event = eventQueue.push();
    if (event != NULL) {
        event->init();
    }
    if (noInterrups) interrupts();
    return event;
}

SHCommand* SHController::pushCommand(SHCommand command) {
    SHCommand* cmd = commandQueue.push();
    if (cmd != NULL) {
        *cmd = command;
    } else {
        Serial.println(F(">>> Error: command buffer is full, command isn't allocated"));
    }
    return cmd;
}

void SHController::pollComponents()
{
    for (int i=0; i<componentCount; i++)
    {
        components[i]->poll();
    }
}

void SHController::longPollComponents()
{
    for (int i=0; i<componentCount; i++)
    {
        components[i]->longPoll();
    }
}

void SHController::processEvents()
{
    if (eventQueue.isFull()) {
        Serial.println(F(">>> Warning: event buffer is full, some events might be lost"));
    }
    SHEventBuf eventCopy;

    noInterrupts();
    SHEvent *event = eventQueue.pop();
    if (event != NULL)
    {
        eventCopy = *((SHEventBuf*)event); //copy from buffer
    }
    interrupts();

    if (event != NULL)
    {
        processEvent(&eventCopy);
    }
}

void SHController::processCommands(filterCommand_t filterCommandCallback)
{
    SHCommand *command = commandQueue.pop();
    if (command != NULL)
    {
        if (((command->delay <= 0) || (millis()-command->time > command->delay)) &&
            ((filterCommandCallback == NULL) || filterCommandCallback(command))) {
            SHCommand cmdCopy = *command; //copy from buffer
            processCommand(&cmdCopy);
        }
        else {
            //requeue command
            SHCommand* cmd = commandQueue.push();
            if (cmd != NULL) {
                *cmd = *command;
            }
        }
    }
}

void SHController::processEvent(SHEvent* event)
{
    Serial.print(F("Event: ")); Serial.print(event->componentId); Serial.print(F(" ")); Serial.println(event->eventId);
    this->handleEvent(event);
}

void SHController::processCommand(SHCommand* command)
{
    Serial.print(F("Cmd: ")); Serial.print(command->componentId); Serial.print(F(" ")); Serial.println(command->id);
    this->handleCommand(command);
}


void printState(unsigned long loopTime){
    static const unsigned long interval = 1000;
    static unsigned long previousMillis = 0;

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval)
    {
        previousMillis = currentMillis;

        if (loopTime > 5) {
            Serial.print(F("L:")); Serial.print(loopTime); Serial.print(F(" M:")); Serial.println(freeRam());
        }
        else {
            Serial.print(F("."));
        }
    }
}

void SHController::loop()
{
    unsigned long startLoopTime = millis();
    longPollComponents();
    processEvents();
    processState();
    processCommands();
    printState(millis()-startLoopTime);
}

bool SHController::parseCommand(SHCommand& cmd, const char *const str)
{
    if ((str == NULL) || (strlen(str) == 0)) return false;
	const char* p = str;
	//component
	const char* separator = strchr(p, ' ');
	if (separator == 0) return false;
	cmd.setArgs(p, separator - p);
	SHComponent* component = getComponentByName(cmd.args);
	if (component == NULL) return false;
	cmd.componentId = component->id;
	//command
	p = separator + 1;
	separator = strchr(p, ' ');
	if (separator != 0) {
	    cmd.setArgs(p, separator - p);
	}
	else {
	    cmd.setArgs(p);
	}
	cmd.id = component->getCommandId(cmd.args);
	if (cmd.id < 0) return false;
	if (separator == 0) {
		return true;
	}
	//args
	p = separator + 1;
	cmd.setArgs(p);
	return true;
}
