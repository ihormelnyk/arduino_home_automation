#include <Arduino.h>
#include "GSMSerial.h"

class ATCmdBase
{
  public:
    byte is_valid;
    short error;
    ATCmdBase():is_valid(0), error(-1){};
    bool isOk() {return is_valid && (error == 0);}
	void parseOkErrorResponse(const char *const str);
};

class ATCmd : public ATCmdBase
{
    public:
        static ATCmd send(GSMSerial& serial);
};

class ATECmd : public ATCmdBase
{
    public:
        static ATECmd send(GSMSerial& serial, byte mode);
};

class CCALRCmd : public ATCmdBase
{
  public:
    byte mode;
    CCALRCmd() : ATCmdBase(), mode(0) {}
    bool isCallReady() {return is_valid && (mode == 1);}
    static CCALRCmd send(GSMSerial& serial);
};

class CREGCmd : public ATCmdBase
{
  public:
    byte n;
    byte stat;
    CREGCmd() : ATCmdBase(), n(0), stat(0) {}
    bool isRegisteredInHomeNetwork() {return is_valid && (stat == 1);}
    static CREGCmd send(GSMSerial& serial);
};

class SAPBRCmd : public ATCmdBase
{
  public:
    byte cid;
    byte status;
    SAPBRCmd() : ATCmdBase(), cid(0), status(0) {}
    bool isBearerConnected() {return is_valid && (status == 1);}
    static SAPBRCmd send(GSMSerial& serial, byte cmd_type);
};

class HTTPINITCmd : public ATCmdBase
{
    public:
        static HTTPINITCmd send(GSMSerial& serial);
};

class HTTPTERMCmd : public ATCmdBase
{
    public:
        static HTTPTERMCmd send(GSMSerial& serial);
};

class HTTPPARACmd : public ATCmdBase
{
    public:
		static HTTPPARACmd send(GSMSerial& serial, const char *const param, const char *const value);
        static HTTPPARACmd send(GSMSerial& serial, byte cid);
};

class HTTPACTIONCmd : public ATCmdBase
{
    public:
        static HTTPACTIONCmd send(GSMSerial& serial, byte action, bool waitForResponse = false);
};

class HTTPREADCmd : public ATCmdBase
{
  public:
    int data_len;
    HTTPREADCmd() : ATCmdBase(), data_len(0) {}
    static HTTPREADCmd send(GSMSerial& serial, char* responseBuf, unsigned int responseBufLength);
};

class ATDCmd : public ATCmdBase
{
    public:
        static ATDCmd send(GSMSerial& serial, int cellNumber);
};

class ATHCmd : public ATCmdBase
{
    public:
        static ATHCmd send(GSMSerial& serial, byte n);
};

class CPASCmd : public ATCmdBase
{
  public:
    byte mode;
    CPASCmd() : ATCmdBase(), mode(2) {}
    bool isReady() {return is_valid && (mode == 0);}
    static CPASCmd send(GSMSerial& serial);
};

class CMGDACmd : public ATCmdBase
{
    public:
		static CMGDACmd send(GSMSerial& serial, const char *const type);
};


class CGATTCmd : public ATCmdBase
{
  public:
    byte state;
    CGATTCmd() : ATCmdBase(), state(0) {}
    bool isConnected() {return is_valid && (state == 1);}
    static CGATTCmd send(GSMSerial& serial);
    static CGATTCmd send(GSMSerial& serial, byte state);
};

class HTTPDATACmd : public ATCmdBase
{
    public:
        static HTTPDATACmd send(GSMSerial& serial, const char *const data);
};

class CMGFCmd : public ATCmdBase
{
  public:
    static CMGFCmd send(GSMSerial& serial, byte mode);
};

class CNMICmd : public ATCmdBase
{
  public:
    static CNMICmd send(GSMSerial& serial, byte mode, byte mt);
};
