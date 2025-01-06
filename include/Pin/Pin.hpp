#pragma once

#include <Arduino.h>
#include<USBAPI.h>
#include "Types/Types.hpp"

class Pin {
public:
	explicit Pin(u8 pin) {
		this->val = pin;
	};

	explicit Pin(u8 pin, u8 mode) {
		this->val = pin;
		this->mode = mode;
		pinMode(pin, mode);
	};

	i32 read() {
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

	inline u8 get_pin() {
		return val;
	}
	
	//void set_mode(int);
	
	operator i32(){return val;}; // probably needs static_cast (?)
	operator u32(){return val;};
	operator u8(){return val;};	

private:
	u8 val;
	u8 mode;
	//bool is_analog;
};