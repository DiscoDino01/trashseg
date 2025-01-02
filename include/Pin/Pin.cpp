#include "Pin.hpp"
//#include <Arduino.h>

//Pin::Pin(unsigned char pin) {
//	this->val = pin;
//}

//Pin::Pin(unsigned char pin, unsigned char mode) {
//	this->val = pin;
//	this->mode = mode;
//	pinMode(pin, mode);
//}

//int Pin::read() {
//	if (digitalPinToPort(this->val) != NOT_A_PIN) {
//		return digitalRead(this->val);
//	} 
//	return analogRead(this->val);
//}

//void Pin::write(int value) {
//	if (digitalPinToPort(this->val) != NOT_A_PIN) {
//        digitalWrite(this->val, value);
//    } else 
//		analogWrite(this->val, value)
//	;
//}

//Pin::operator int(){
//	return this->val;
//}

//Pin::operator unsigned() { // unsigned int
//	return this->val;
//}

//Pin::operator unsigned char() { // unsigned char
//	return this->val;
//}
