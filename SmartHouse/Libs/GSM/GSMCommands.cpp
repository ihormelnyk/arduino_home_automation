#include "GSMCommands.h"

void ATCmdBase::parseOkErrorResponse(const char *const str) {
	if (!str) {
		return;
	}
	if (strcmp(str, "OK") == 0) {
		error = 0;
		is_valid = true;
	}
	else if (strcmp(str, "ERROR") == 0) {
		error = -1;
		is_valid = true;
	}
	else if (strstr(str, "+CME ERROR: ") == str) {
		error = atoi(str + 12);
		is_valid = true;
	}
};

ATCmd ATCmd::send(GSMSerial& serial) {
	ATCmd resp;
	if (!serial.sendATCommand("AT")) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
};

ATECmd ATECmd::send(GSMSerial& serial, byte mode) {
	//Set Command Echo Mode
	ATECmd resp;
	if (!serial.sendATCommand(mode ? "ATE1" : "ATE0")) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

CCALRCmd CCALRCmd::send(GSMSerial& serial) {
	//Call Ready Query
	CCALRCmd resp;
	if (!serial.sendATCommand("AT+CCALR?")) return resp;
	const char *str = serial.recvLine(5000);
	resp.parseOkErrorResponse(str);
	if (!resp.is_valid) {
		if (strstr(str, "+CCALR:") == str) {
			resp.mode = atoi(str + 8);
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			str = serial.recvLine(1000);
			resp.parseOkErrorResponse(str);
		}
	}
	return resp;
}

CREGCmd CREGCmd::send(GSMSerial& serial) {
	//Network Registration
	CREGCmd resp;
	if (!serial.sendATCommand("AT+CREG?")) return resp;
	const char *str = serial.recvLine(1000);
	resp.parseOkErrorResponse(str);
	if (!resp.is_valid) {
		if (strstr(str, "+CREG: ") == str) {
			resp.n = atoi(str + 7);
			resp.stat = atoi(str + 9);
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			str = serial.recvLine(1000);
			resp.parseOkErrorResponse(str);
		}
	}
	return resp;
}

SAPBRCmd SAPBRCmd::send(GSMSerial& serial, byte cmd_type) {
	//Bearer Settings for Applications Based on IP
	SAPBRCmd resp;
	if (!serial.sendATCommand(cmd_type == 1 ? "AT+SAPBR=1,1" : "AT+SAPBR=2,1")) return resp;
	const char *str = serial.recvLine(2000);
	resp.parseOkErrorResponse(str);
	if (!resp.is_valid) {
		if (strstr(str, "+SAPBR: ") == str) {
			resp.cid = atoi(str + 8);
			resp.status = atoi(str + 10);
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			str = serial.recvLine(1000);
			resp.parseOkErrorResponse(str);
		}
	}
	return resp;
}

HTTPINITCmd HTTPINITCmd::send(GSMSerial& serial) {
	//Initialize HTTP Service
	HTTPINITCmd resp;
	if (!serial.sendATCommand("AT+HTTPINIT")) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

HTTPTERMCmd HTTPTERMCmd::send(GSMSerial& serial) {
	//Terminate HTTP Service
	HTTPTERMCmd resp;
	if (!serial.sendATCommand("AT+HTTPTERM")) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

HTTPPARACmd HTTPPARACmd::send(GSMSerial& serial, const char *const param, const char *const value) {
	//Set HTTP Parameters Value
	HTTPPARACmd resp;
	const int maxLen = 128;
	char cmd[maxLen];
	if ((strlen(param) + strlen(value)) >= maxLen - 17) return resp;
	sprintf(cmd, "AT+HTTPPARA=\"%s\",\"%s\"", param, value);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

HTTPPARACmd HTTPPARACmd::send(GSMSerial& serial, byte cid) {
	//Set HTTP Parameters Value
	HTTPPARACmd resp;
	char cmd[32];
	sprintf(cmd, "AT+HTTPPARA=\"CID\",%d", cid);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

//TODO: parse action and status code
HTTPACTIONCmd HTTPACTIONCmd::send(GSMSerial& serial, byte action, bool waitForResponse) {
	//HTTP Method Action
	HTTPACTIONCmd resp;
	if (action > 9) return resp;
	char cmd[16];
	sprintf(cmd, "AT+HTTPACTION=%d", action);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));

    if (waitForResponse) {
        if (resp.is_valid && resp.isOk()) {
            resp.is_valid = false;
            if (!serial.recvUnsolicitedCode("+HTTPACTION:", 10000)) return resp; //+HTTPACTION:0,200,len
            resp.is_valid = true;
        }
    }
	return resp;
}

HTTPREADCmd HTTPREADCmd::send(GSMSerial& serial, char* responseBuf, unsigned int responseBufLength) {
	//Read the HTTP Server Response
	HTTPREADCmd resp;
	if (responseBufLength > 0) {
		responseBuf[0] = 0x00;
	}
	if (!serial.sendATCommand("AT+HTTPREAD")) return resp;
	const char *str = serial.recvLine(1000);
	resp.parseOkErrorResponse(str);
	if (!resp.is_valid) {
		if (strstr(str, "+HTTPREAD:") == str) {
			resp.data_len = atoi(str + 10);
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			if (!serial.recvData(resp.data_len, responseBuf, responseBufLength, 10000)) return resp;
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			str = serial.recvLine(1000);
			resp.parseOkErrorResponse(str);
		}
	}
	return resp;
}

ATDCmd ATDCmd::send(GSMSerial& serial, int cellNumber) {
	//Mobile Originated Call to Dial A Number
	// ATD>"SM" 1;<CR>
	ATDCmd resp;
	if ((cellNumber < 0) || (cellNumber > 999)) return resp;
	char cmd[16];
	sprintf(cmd, "ATD>\"SM\" %d;", cellNumber);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

ATHCmd ATHCmd::send(GSMSerial& serial, byte n) {
	//Disconnect Existing Connection
	ATHCmd resp;
	char cmd[16];
	sprintf(cmd, "ATH%d", n);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

CPASCmd CPASCmd::send(GSMSerial& serial) {
	//Phone Activity Status
	CPASCmd resp;
	if (!serial.sendATCommand("AT+CPAS")) return resp;
	const char *str = serial.recvLine(1000);
	resp.parseOkErrorResponse(str);
	if (!resp.is_valid) {
		if (strstr(str, "+CPAS:") == str) {
			resp.mode = atoi(str + 7);
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			str = serial.recvLine(1000);
			resp.parseOkErrorResponse(str);
		}
	}
	return resp;
}

CMGDACmd CMGDACmd::send(GSMSerial& serial, const char *const type) {
	//Delete All SMS
	//DEL ALL
	const int maxLen = 32;
	CMGDACmd resp;
	if (strlen(type) >= maxLen - 11) return resp;
	char cmd[maxLen];
	sprintf(cmd, "AT+CMGDA=\"%s\"", type);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(5000));
	return resp;
}

CGATTCmd CGATTCmd::send(GSMSerial& serial) {
	//Attach or Detach from GPRS Service
	CGATTCmd resp;
	if (!serial.sendATCommand("AT+CGATT?")) return resp;
	const char *str = serial.recvLine(1000);
	resp.parseOkErrorResponse(str);
	if (!resp.is_valid) {
		if (strstr(str, "+CGATT:") == str) {
			resp.state = atoi(str + 8);
			resp.is_valid = true;
		}
		if (resp.is_valid) {
			resp.is_valid = false;
			str = serial.recvLine(1000);
			resp.parseOkErrorResponse(str);
		}
	}
	return resp;
}

CGATTCmd CGATTCmd::send(GSMSerial& serial, byte state) {
	//Attach or Detach from GPRS Service
	CGATTCmd resp;
	char cmd[16];
	sprintf(cmd, "AT+CGATT=%d", state);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(5000));
	return resp;
}

HTTPDATACmd HTTPDATACmd::send(GSMSerial& serial, const char *const data) {
	//Attach or Detach from GPRS Service
	HTTPDATACmd resp;
	char cmd[32];
	size_t len = strlen(data);
	sprintf(cmd, "AT+HTTPDATA=%d,5000", len);
	if (!serial.sendATCommand(cmd)) return resp;
	if (len > 0) {
        if (!serial.recvUnsolicitedCode("DOWNLOAD", 1000)) return resp;
        if (!serial.write(data)) return resp;
	}
	resp.parseOkErrorResponse(serial.recvLine(5000));
	return resp;
}

CMGFCmd CMGFCmd::send(GSMSerial& serial, byte mode) {
	//Select SMS message format
	//0 PDU mode
	//1 text mode
	CMGFCmd resp;
	char cmd[16];
	sprintf(cmd, "AT+CMGF=%d", mode);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}

CNMICmd CNMICmd::send(GSMSerial& serial, byte mode, byte mt) {
	//New SMS message indication
	CNMICmd resp;
	char cmd[16];
	sprintf(cmd, "AT+CNMI=%d,%d", mode, mt);
	if (!serial.sendATCommand(cmd)) return resp;
	resp.parseOkErrorResponse(serial.recvLine(1000));
	return resp;
}


