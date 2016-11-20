#include "GSMSerial.h"

GSMSerial::GSMSerial(GSMDebug& debug, GSMSerialHandler* serialHandler = NULL) :
isSerialBusy(false), isInnerLoop(false), debug(debug), serialHandler(serialHandler)
{
};

void GSMSerial::begin(unsigned long baud_rate)
{
	GSM_SERIAL.begin(baud_rate);
}

int GSMSerial::available()
{
	return GSM_SERIAL.available();
}

bool GSMSerial::checkInnerLoop()
{
	if (isInnerLoop)
	{
		debug.println(F("[GSM] Double inner loop"));
		return true;
	}
	return false;
}

int GSMSerial::read()
{
	if (checkInnerLoop()) return -1;
	return GSM_SERIAL.read();
}

void GSMSerial::innerLoop()
{
	if (checkInnerLoop()) return;
	isInnerLoop = true;
	if (serialHandler) {
		serialHandler->innerLoop();
	}
	isInnerLoop = false;
}

bool GSMSerial::handleUnsolicitedCode(const char *const str) {
	if (checkInnerLoop()) return false;
	if (!str) {
		return false;
	}
	if (serialHandler) {
		return serialHandler->handleURC(str);
	}
	return false;
}

void GSMSerial::flush() {
	if (checkInnerLoop()) return;
	//flush rx buffer
	while (GSM_SERIAL.available())
	{
		debug.print((char)GSM_SERIAL.read());
	}
}

const char *const GSMSerial::recvLine(unsigned int timeout, bool optionalCrLfAtStart) {
	if (checkInnerLoop() || !buf || (GSM_BUF_LEN < 1)) return NULL;	
	buf[0] = 0x0;
	short state = -1;	
	unsigned int buf_pos = 0;
	bool invalid_chr = false;
	unsigned long start_time = millis();
	while (true)
	{
		while (GSM_SERIAL.available())
		{
			invalid_chr = false;
			char chr = GSM_SERIAL.read();
			if (chr == '\r') {
				if ((state == -1) || (state == 2)) state++;
				else invalid_chr = true;
			}
			else if (chr == '\n' && state != 1 && state != 2) { //for 1 & 2 states as data
				if ((state == 0) || (state == 3)) state++;
				else invalid_chr = true;
			}
			else {
				if (state == 1) state++;
				else if (optionalCrLfAtStart && (state == -1)) state = 2;
				else if (state != 2) invalid_chr = true;
				if (buf_pos >= (GSM_BUF_LEN - 1)) {
					debug.println(F("[GSM] Response buffer overflow"));
					invalid_chr = true;
				}
				else {
					buf[buf_pos++] = chr;
					buf[buf_pos] = 0x0;
				}
			}
			if (state == 4) {
				//print received string
				//debug.println((const char*)buf);
				return (const char*)buf;
			}
			else if (invalid_chr) {
				debug.print(F("[GSM] Unexpected character '")); debug.print(chr); debug.print(F("' for state "));  debug.println(state);
				//flush
				while ((millis() - start_time) < 100) {
					flush();
				}
				return NULL;
			}
		}
		if ((millis() - start_time) > timeout) {
			debug.print(F("[GSM] End of line recv timeout. Received '")); debug.print((char*)buf); debug.println(F("'"));
			break;
		}
		else
		{
			innerLoop();
		}
	}
	return NULL;
}

bool GSMSerial::recvData(unsigned int data_len, char* responseBuf, unsigned int responseBufLength, unsigned int timeout) {
	if (checkInnerLoop() || !responseBuf || (responseBufLength < 1)) return false;
	responseBuf[0] = 0;

	unsigned long start_time = millis();
	unsigned int pos = 0;
	bool isTimeout = false;
	for (pos = 0; pos < data_len; pos++)
	{
		while (!GSM_SERIAL.available()) {
			if ((millis() - start_time) > timeout) {
				isTimeout = true;
				debug.println(F("[GSM] response timeout"));
				break;
			}
			innerLoop();
		}
		if (isTimeout) break;
		char chr = GSM_SERIAL.read();
		if (pos < (responseBufLength - 1)) {
			responseBuf[pos] = chr;
			responseBuf[pos + 1] = 0;
		}
	}
	if (pos < responseBufLength) {		
		debug.println((const char*)responseBuf);
	}
	else {
		debug.println(F("[GSM] DATA buffer overflow."));
	}
	return pos == data_len;
}

const char *const GSMSerial::recvUnsolicitedCode(const char *const codePrefix, unsigned int timeout) {
	if (checkInnerLoop()) return NULL;
	unsigned long start_time = millis();
	while (true) {
		const char *const str = recvLine(timeout);
		if (!str) return NULL;
		if (!handleUnsolicitedCode(str))
		{
			debug.println(F("[GSM] Response isn't unsolicited code."));
			return NULL;
		}
		if ((strlen(str) >= strlen(codePrefix)) &&
			(strncmp(codePrefix, str, strlen(codePrefix)) == 0)) return str;
		if ((millis() - start_time) > timeout) {
			debug.print(F("[GSM] Receive unsolicited code timeout."));
			break;
		}
	}
	return NULL;
}

bool GSMSerial::sendATCommand(const char *const command, bool checkEcho) {
	if (checkInnerLoop()) return false;
	if (!isSerialBusy) {
		//print sending command
		//debug.print(F("[GSM] send: ")); debug.println(command);
	}
	else {
		debug.print(F("[GSM] Serial is busy. Can't send: ")); debug.println(command);
		return false;
	}
	//flush();

	//write AT command
	if (GSM_SERIAL.println(command) != (strlen(command) + 2)) {
		debug.println(F("[GSM] Error occurred during AT Command sending"));
		return false;
	}
	if (!checkEcho) return true;
	isSerialBusy = true;
	while (true) {
		const char *const str = recvLine(1000, true);
		if (!str) {
			isSerialBusy = false;
			return false;
		}
		if (strcmp(command, str) == 0) {
			isSerialBusy = false;
			return true;
		}
		if (!handleUnsolicitedCode(str))
		{
			debug.print(F("[GSM] Unknown response '")); debug.print(str); debug.println(F("' No command echo received"));
		}
	}
}

bool GSMSerial::write(const char *const data) {
	if (checkInnerLoop()) return false;
	//write data
	if (GSM_SERIAL.print(data) != (strlen(data))) {
		debug.println(F("[GSM] Error occurred during writing data"));
		return false;
	}
	return true;
}

void GSMSerial::process() {
	if (checkInnerLoop()) return;
	if (GSM_SERIAL.available()) {
		const char *const str = recvLine(100);
		handleUnsolicitedCode(str);
	}
}
