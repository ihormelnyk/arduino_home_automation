#ifndef GSMSERIALHANDLER_H
#define GSMSERIALHANDLER_H

class GSMSerialHandler{
    public:
		virtual bool handleURC(const char *const str) = 0;
        virtual void innerLoop() = 0;
};

#endif // GSMSERIALHANDLER_H
