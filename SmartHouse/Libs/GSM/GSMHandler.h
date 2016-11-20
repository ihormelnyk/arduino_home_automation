#ifndef GSMHANDLER_H
#define GSMHANDLER_H

class GSMHandler{
    public:
		virtual void onCall(const char *const number, const char *const name) = 0;
		virtual void onSMS(const char *const number, const char *const name, const char *const sms) = 0;
        virtual void innerLoop() {};
		virtual void processHttpResponse(const char *const buffer) {};
};

#endif // GSMHANDLER_H
