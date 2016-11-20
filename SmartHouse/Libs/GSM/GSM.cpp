#include "GSM.h"

GSM::GSM(GSMHandler* handler = NULL) :
powerPin(GSM_ON_OFF_PIN),
resetPin(GSM_RESET_PIN),
handler(handler),
serial(debug, this),

powerState(-1),
networkState(-1),
GPRSState(-1),
bearerState(-1),
httpState(-1),
httpParamState(-1),
httpRequestState(-1),

lastPowerOn(0),
lastHttpRequest(0),
powerInProgress(false),
callInProgress(false),
httpInProgress(false),
httpMethod(0)
{
};

void GSM::innerLoop()
{
	if (handler) {
		handler->innerLoop();
	}
}

void GSM::processHttpResponse() {
    if (handler) {
		handler->processHttpResponse(httpBuffer);
	}
}

bool GSM::handleURC(const char *const str){
	if (strcmp(str, "RING") == 0) {
		return true;
	}
	if (strcmp(str, "NO CARRIER") == 0) {
		return true;
	}
	if (strcmp(str, "BUSY") == 0) {
		return true;
	}
	if (strstr(str, "+CLIP:") == str) {
		//+CLIP: "+38067xxxxxxx",145,"",,"Name",0
		if (handler) {
			const int numberLen = 16;
			char number[numberLen] = "";
			const int nameLen = 32;
			char name[nameLen] = "";
			if (strlen(str) > 8) {
				char* p = (char*)str + 8;
				char *np = strchr(p, '\"');
				if (np && (np - p < numberLen)) {
					strncat(number, p, np - p);
				}
				if (np) {
					np = strchr(np + 1, '\"');
				}
				if (np) {
					np = strchr(np + 1, '\"');
				}
				if (np) {
					p = strchr(np + 1, '\"');
					if (p) {
						p++;
						np = strchr(p, '\"');
					}
				}
				if (np && (np - p < nameLen)) {
					strncat(name, p, np - p);
				}
			}
			handler->onCall(number, name);
		}
		return true;
	}
	if (strstr(str, "+SAPBR") == str) {
		return true;
	}
	if (strstr(str, "+HTTPACTION:") == str) { //+HTTPACTION:0,200,len
        debug.print(F("[GSM] Response: ")); debug.println(str);
        if (httpRequestState == 1) {
            httpRequestState = 2; //response is ready
            if (strlen(str) > 14) {
                int status = atoi(str + 14);
                if (status >= 600) { //network error
                    bearerState = -1;
                    httpRequestState = -1;
                }
            }
        } else {
            httpState = -2; //terminate prev request
            httpParamState = -1;
            httpRequestState = -1;
        }
		return true;
	}
	if (strstr(str, "+CMT:") == str) { //+CMT: "+38067xxxxxxx","Name","14/04/06,11:38:14+12" //SMS 1310
        const int numberLen = 16;
        char number[numberLen] = "";
        const int nameLen = 32;
        char name[nameLen] = "";
        if (strlen(str) > 7) {
            char* p = (char*)str + 7;
            char *np = strchr(p, '\"');
            if (np && (np - p < numberLen)) {
                strncat(number, p, np - p);
            }
            if (np) {
                p = strchr(np + 1, '\"');
                if (p) {
                    p++;
                    np = strchr(p, '\"');
                }
            }
            if (np && (np - p < nameLen)) {
                strncat(name, p, np - p);
            }
        }
		const char* sms = serial.recvLine(100, true);
		if (handler) {
			handler->onSMS(number, name, sms);
		}
		return true;
	}
	if (strstr(str, "+CMTI:") == str) { //+CMTI: "SM",5
        debug.println(F("[GSM] SMS (Unsupported format)"));
	    return true;
	}
	if (strcmp(str, "RDY") == 0) {
		return true;
	}
	if (strcmp(str, "Call Ready") == 0) {
		return true;
	}
	if (strstr(str, "+CFUN:") == str) {
		return true;
	}
	if (strstr(str, "+CPIN:") == str) {
		return true;
	}
	if (strstr(str, "+CREG:") == str) {
		return true;
	}
	if (strcmp(str, "NORMAL POWER DOWN") == 0) {
		return true;
	}
	if (strcmp(str, "DOWNLOAD") == 0) {
		return true;
	}
	//start code (00 49 49 49 49 FF FF FF FF) - for 115200 baudrate
	return false;
}

bool GSM::recvRDY() {
	char rdy[] = { '\r', '\n', 'R', 'D', 'Y', '\r', '\n' };
	byte pos = 0;
	unsigned long start_time = millis();
	while ((pos < 7) && ((millis() - start_time) < 2000)) {
		if (!serial.available()) continue;
		if (rdy[pos] == serial.read()) {
			pos++;
		}
		else if (pos != 0){
			break;
		}
	}
	return pos == 7;
}

void GSM::powerOnStep1()
{
    debug.println(F("[GSM] Power on..."));
	pinMode(powerPin, OUTPUT);
	digitalWrite(powerPin, HIGH);
}
void GSM::powerOnStep2(){
    digitalWrite(powerPin, LOW);
}

bool GSM::powerOn() {
	//check if modem is online
	if (ATCmd::send(serial).isOk()) {
        powerState = 1;
		return true;
	} else {
	    powerState = 0;
	}
	//send power on impulse
	powerOnStep1();
	delay(1000);
	powerOnStep2();
	//receive RDY
	if (recvRDY()) {
		debug.println(F("[GSM] RDY"));
	}
	else {
		serial.flush();
		debug.println(F("[GSM] Error - RDY timeout"));
	}
	//check if modem is online
	for (byte i = 0; i < 3; i++){
		if (ATCmd::send(serial).isOk()) {
            powerState = 1;
			return true;
		}
	}
	powerState = 0;
	return false;
}

bool GSM::setup(unsigned long baud_rate, uint8_t powerPin, uint8_t resetPin)
{
	this->powerPin = powerPin;
	this->resetPin = resetPin;
	serial.begin(baud_rate); //AT+IPR=57600
	if (!powerOn()) {
		debug.println(F("[GSM] Power on failed"));
		return false;
	}
	debug.println(F("[GSM] Online"));
	//1. check if call ready
	//2. else wait 5s for 'Call ready' unsolicited code
	//3. else check if call ready
	if (!CCALRCmd::send(serial).isCallReady() && !serial.recvUnsolicitedCode("Call Ready", 10000) && !CCALRCmd::send(serial).isCallReady()) {
        debug.println(F("[GSM] Modem isn't ready"));
		return false;
	}
	debug.println(F("[GSM] Call Ready"));
	//set sms to text mode for del all sms command
    if (CMGFCmd::send(serial, 1).isOk()) {
        debug.println(F("Text SMS mode set successfully"));
    } else {
        debug.println(F("Failed to set text SMS mode"));
    }
    //set new sms notification mode
    if (CNMICmd::send(serial, 2, 2).isOk()) {
        debug.println(F("SMS notification mode set successfully"));
    } else {
        debug.println(F("Failed to set SMS notification mode"));
    }
	return true;
}

void GSM::powerOnAsync() {
    if (powerInProgress) return;
    powerInProgress = true;
    Serial.println("powerOnAsync");
}

void GSM::processPower() {
    if (!powerInProgress) return;
    if (ATCmd::send(serial).isOk()) {
        powerState = 1;
        powerInProgress = false;
        return;
    } else {
        powerState = 0;
        if ((millis() - lastPowerOn) > 10000) {
            powerOnStep1();
            lastPowerOn = millis();
            return;
        }
        if ((millis() - lastPowerOn) > 1000) {
            powerOnStep2();
            return;
        }
    }
}

void GSM::processHttp() {
    if (!httpInProgress) return;
     //Request timeout
    if ((millis() - lastHttpRequest) > 20000) {
        httpState = -2; //terminate prev request
        httpParamState = -1;
        httpRequestState = -1;
        httpInProgress = false;
        debug.println(F("Request timeout"));
        return;
    }

    if (powerState != 1) {
        powerOnAsync();
        return;
    }
    //Checking registration status...
	//AT+CREG?
	//+CREG: 1,1
	//OK
    if (networkState != 1) {
        if (CREGCmd::send(serial).isRegisteredInHomeNetwork()) {
            networkState = 1;
            debug.println(F("[GSM] Registered, home network"));
        }
        else {
            networkState = 0;
            powerState = -1; //clear prev step
            debug.println(F("[GSM] Network isn't registered"));
        }
        return;
    }
    //Connect GPRS Service
	//AT+CGATT=1
	//OK
    if (GPRSState != 1) {
        if (CGATTCmd::send(serial, 1).isOk()) {
            GPRSState = 1;
            debug.println(F("[GSM] GPRS is succesfully connected"));
        }
        else {
            GPRSState = 0;
            networkState = -1; //clear prev step
            debug.print(F("[GSM] Error: Can't connect GPRS"));
        }
        return;
    }
    //Querying bearer 1
	//AT+SAPBR=2,1
	//+SAPBR: 1,3,"0.0.0.0"
	//OK
    if (bearerState == -1) {
        if (SAPBRCmd::send(serial, 2).isBearerConnected()) {
            bearerState = 1;
            debug.println(F("[GSM] Bearer is connected already"));
        }
        else {
            bearerState = 0;
            debug.println(F("[GSM] Bearer isn't connected"));
        }
        return;
    }
    //Opening Bearer 1...
    //AT+SAPBR=1,1
    //OK
    if (bearerState != 1) {
        if (SAPBRCmd::send(serial, 1).isOk()) {
            bearerState = 1;
            debug.println(F("[GSM] Bearer is succesfully connected"));
        }
        else {
            bearerState = -1;
            GPRSState = -1; //clear prev step
            debug.print(F("[GSM] Error: Can't connect bearer "));
        }
        return;
    }
    if (httpState == -2) {
        if (HTTPTERMCmd::send(serial).isOk()) {
            httpState = 0;
            debug.println(F("[GSM] HTTP is terminated"));
        }
        else {
            httpState = -1;
            bearerState = -1; //clear prev step
            debug.println(F("[GSM] HTTP termination failed"));
        }
        return;
    }
    //Initializing HTTP service...
	//AT+HTTPINIT
	//OK
    if (httpState != 1) {
        if (HTTPINITCmd::send(serial).isOk()) {
            httpState = 1;
            debug.println(F("[GSM] HTTP is initialized"));
        }
        else {
            httpState = -2; //invalid state, try to terminate
            debug.println(F("[GSM] HTTP isn't initialized"));
        }
        return;
    }
    //Set barer ID
	//AT+HTTPPARA="CID",1
	//OK
    if (httpParamState == -1) {
        if (HTTPPARACmd::send(serial, 1).isOk()) {
            httpParamState = 0;
            debug.println(F("[GSM] CID is set"));
        }
        else {
            httpState = -1; //clear prev step
            debug.println(F("[GSM] CID isn't set"));
        }
        return;
    }
    //Setting up HTTP parameters..
	//AT+HTTPPARA="URL","http://google.com"
	//OK
    if (httpParamState == 0) {
        if (HTTPPARACmd::send(serial, "URL", httpUrl).isOk()) {
            httpParamState = 1;
            debug.println(F("[GSM] Url is set"));
        }
        else {
            httpParamState = -1;
            httpState = -1; //clear prev step
            debug.println(F("[GSM] Url isn't set"));
        }
        return;
    }

    //AT+HTTPPARA="CONTENT","application/x-www-form-urlencoded"
    if ((httpMethod == 1) && (httpParamState == 1)) {
        if (HTTPPARACmd::send(serial, "CONTENT", "application/x-www-form-urlencoded").isOk()) {
            httpParamState = 2;
            debug.println(F("[GSM] Content is set"));
        }
        else {
            httpParamState = -1;
            httpState = -1; //clear prev step
            debug.println(F("[GSM] Content isn't set"));
        }
        return;
    }

    //AT+HTTPDATA=50,20000
    if ((httpMethod == 1) && (httpParamState == 2)) {
        if (HTTPDATACmd::send(serial, httpBuffer).isOk()) {
            httpParamState = 3;
            debug.println(F("[GSM] Data for POST loaded"));
        }
        else {
            httpParamState = -1;
            httpState = -1; //clear prev step
            debug.println(F("[GSM] Data for POST isn't loaded"));
        }
        return;
    }
    //AT+HTTPACTION=0
	//OK
    if (httpRequestState == -1) {
        if (HTTPACTIONCmd::send(serial, httpMethod == 1).isOk()) {
            httpRequestState = 1;
            debug.println(httpMethod == 1 ? F("[GSM] HTTP POST is successful") : F("[GSM] HTTP GET is successful"));
        }
        else {
            httpRequestState = -1;
            httpParamState = -1; //clear prev step
            debug.println(httpMethod == 1 ? F("[GSM] HTTP POST is failed") : F("[GSM] HTTP GET is failed"));
        }
        return;
    }

    //AT+HTTPREAD
	//+HTTPREAD:58
	//OK
    if (httpRequestState == 2) {
        HTTPREADCmd response = HTTPREADCmd::send(serial, httpBuffer, GSM_HTTP_BUF_LEN);
        if (response.isOk()) {
            httpRequestState = 3;
            debug.println(F("[GSM] Successful read HTTP data"));
        }
        else {
            httpRequestState = -1; //clear prev step
            debug.println(F("[GSM] Error reading HTTP data"));
        }
        return;
    }

    //Terminating HTTP session..
	//AT+HTTPTERM
	//OK
    if (httpRequestState == 3) {
        if (HTTPTERMCmd::send(serial).isOk()) {
            httpParamState = 0; //CID initialized
            httpState = 0;
            httpRequestState = -1;
            debug.println(F("[GSM] HTTP is terminated"));
            processHttpResponse();
        }
        else {
            httpParamState = -1;
            httpState = -1;
            httpRequestState = -1;
            debug.println(F("[GSM] HTTP termination failed"));
        }
        httpInProgress = false;
        return;
    }
}

void GSM::process() {
	serial.process();
	processPower();
	processHttp();
}

void GSM::cleanHttpBuffer(){
    if (httpBuffer && GSM_HTTP_BUF_LEN > 0) {
        httpBuffer[0] = 0;
    }
}

char *const GSM::getHttpBuffer() {
    return httpBuffer;
}

uint16_t GSM::getHttpBufferLen() {
    return GSM_HTTP_BUF_LEN;
}


bool GSM::canCall() {
	return CPASCmd::send(serial).isReady();
}

bool GSM::canHttp() {
	return !httpInProgress;
}

bool GSM::call(int cellNumber) {
	if (ATDCmd::send(serial, cellNumber).isOk()) {
		debug.println(F("[GSM] Calling..."));
		return true;
	}
	debug.println(F("[GSM] Call failed"));
	return false;
}

void GSM::hangUp() {
	if (ATHCmd::send(serial, 0).isOk()) {
		debug.println(F("[GSM] HangUping..."));
	}
	else
	{
		debug.println(F("[GSM] HangUp failed"));
	}
}

bool encode_url(const char* url, char *const buf, uint8_t bufLen) {
	//TODO: url encoding http://www.w3schools.com/tags/ref_urlencode.asp
	if (!buf || (bufLen < 1)) return false;
	strcpy(buf, "");
	char* p = (char*)url;
	while (char* i = strchr(p, ' ')) {
		if ((strlen(buf) + (i - p) + 3) >= bufLen) return false;
		strncat(buf, p, i - p);
		strcat(buf, "%20");
		p = i + 1;
	}
	if ((strlen(buf) + strlen(p)) >= bufLen) return false;
	strcat(buf, p);
	return true;
}


int GSM::httpRequestAsync(const char *const url, bool isPost) {
    if (httpInProgress) return 0;
    httpInProgress = true;
    debug.print(F("Request: ")); debug.println(url);
	if (!encode_url(url, httpUrl, GSM_URL_BUF_LEN)) {
        httpInProgress = false;
        debug.print(F("Invalid url: ")); debug.println(url);
		return 0;
	}
	httpMethod = isPost ? 1 : 0;
	lastHttpRequest = millis();
	return 1;
}

void GSM::deleteAllSMS() {
	if (CMGDACmd::send(serial, "DEL ALL").isOk()) {
		debug.println(F("[GSM] ALL SMS deleted"));
	}
	else
	{
		debug.println(F("[GSM] SMS deletion failed"));
	}
}
