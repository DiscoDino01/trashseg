#include "Pin.hpp"
#include "Arduino.h"
Pin::Pin(uint8_t pin) {
	this->val = pin;
}

Pin::Pin(uint8_t pin, uint8_t mode) {
	this->val = pin;
	this->mode = mode;
	pinMode(pin, mode);
}

int Pin::read() {
	if (digitalPinToPort(this->val) != NOT_A_PIN) {
		return digitalRead(this->val);
	} 
	return analogRead(this->val);
}

void Pin::write(int value) {
	if (digitalPinToPort(this->val) != NOT_A_PIN) {
        digitalWrite(this->val, value);
    } else 
		analogWrite(this->val, value)
	;
}

Pin::operator int(){
	return this->val;
}

Pin::operator uint32_t() { // unsigned int
	return this->val;
}

Pin::operator uint8_t() { // unsigned char
	return this->val;
}
