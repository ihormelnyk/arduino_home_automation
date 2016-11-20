#include "GSMDebug.h"

size_t GSMDebug::write(const char *str) {
    #ifdef DEBUG
	return Serial.write(str);
    #endif
}
size_t GSMDebug::write(const uint8_t *buffer, size_t size) {
    #ifdef DEBUG
	return Serial.write(buffer, size);
    #endif
};

size_t GSMDebug::print(const __FlashStringHelper *ifsh) {
    #ifdef DEBUG
	return Serial.print(ifsh);
    #endif
};
size_t GSMDebug::print(const String &s) {
    #ifdef DEBUG
	return Serial.print(s);
    #endif
};
size_t GSMDebug::print(const char str[]) {
    #ifdef DEBUG
	return Serial.print(str);
    #endif
};
size_t GSMDebug::print(char c) {
    #ifdef DEBUG
	return Serial.print(c);
    #endif
};
size_t GSMDebug::print(unsigned char c, int base) {
    #ifdef DEBUG
	return Serial.print(c, base);
    #endif
};
size_t GSMDebug::print(int n, int base) {
    #ifdef DEBUG
	return Serial.print(n, base);
    #endif
};
size_t GSMDebug::print(unsigned int n, int base) {
    #ifdef DEBUG
	return Serial.print(n, base);
    #endif
};
size_t GSMDebug::print(long n, int base) {
    #ifdef DEBUG
	return Serial.print(n, base);
    #endif
};
size_t GSMDebug::print(unsigned long n, int base) {
    #ifdef DEBUG
	return Serial.print(n, base);
    #endif
};
size_t GSMDebug::print(double n, int base) {
    #ifdef DEBUG
	return Serial.print(n, base);
    #endif
};
size_t GSMDebug::print(const Printable& p) {
    #ifdef DEBUG
	return Serial.print(p);
    #endif
};

size_t GSMDebug::println(const __FlashStringHelper *ifsh) {
    #ifdef DEBUG
	return Serial.println(ifsh);
    #endif
};
size_t GSMDebug::println(const String &s) {
    #ifdef DEBUG
	return Serial.println(s);
    #endif
};
size_t GSMDebug::println(const char str[]) {
    #ifdef DEBUG
	return Serial.println(str);
    #endif
};
size_t GSMDebug::println(char c) {
    #ifdef DEBUG
	return Serial.println(c);
    #endif
};
size_t GSMDebug::println(unsigned char c, int base) {
    #ifdef DEBUG
	return Serial.println(c, base);
    #endif
};
size_t GSMDebug::println(int n, int base) {
    #ifdef DEBUG
	return Serial.println(n, base);
    #endif
};
size_t GSMDebug::println(unsigned int n, int base) {
    #ifdef DEBUG
	return Serial.println(n, base);
    #endif
};
size_t GSMDebug::println(long n, int base) {
    #ifdef DEBUG
	return Serial.println(n, base);
    #endif
};
size_t GSMDebug::println(unsigned long n, int base) {
    #ifdef DEBUG
	return Serial.println(n, base);
    #endif
};
size_t GSMDebug::println(double n, int base) {
    #ifdef DEBUG
	return Serial.println(n, base);
    #endif
};
size_t GSMDebug::println(const Printable& p) {
    #ifdef DEBUG
	return Serial.println(p);
    #endif
};
size_t GSMDebug::println(void) {
    #ifdef DEBUG
    return Serial.println();
    #endif
};
