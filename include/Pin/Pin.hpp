#pragma once

#include <Arduino.h>

class Pin {
public:
	Pin(unsigned char pin) {
		this->val = pin;
	};

	Pin(unsigned char pin, unsigned char mode) {
		this->val = pin;
		this->mode = mode;
		pinMode(pin, mode);
	};

	int read() {
		if (digitalPinToPort(this->val) != NOT_A_PIN) {
			return digitalRead(this->val);
		} 
		return analogRead(this->val);
	};
	void write(int value){
		if (digitalPinToPort(this->val) != NOT_A_PIN) {
			digitalWrite(this->val, value);
		} else 
			analogWrite(this->val, value)
		;
	};

	
	//void set_mode(int);

	operator int() {return val;};
	operator unsigned(){return val;};
	operator unsigned char(){return val;};	

private:
	unsigned char val;
	unsigned char mode;
	//bool is_analog;
};