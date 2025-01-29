#pragma once

#include <Arduino.h>
#include <stdint.h>

class Pin {
public:
	explicit Pin() = default;

	explicit Pin(uint8_t pin) {
		this->val = pin;
	};

	explicit Pin(uint8_t pin, uint8_t mode) {
		this->val = pin;
		this->mode = mode;
		pinMode(pin, mode);
	};

	int32_t read() {
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

	inline uint8_t get_pin() {
		return val;
	}
	
	//void set_mode(int);
	
	operator int32_t(){return val;}; // probably needs static_cast (?)
	operator u32(){return val;};
	operator uint8_t(){return val;};	

private:
	uint8_t val;
	uint8_t mode;
	//bool is_analog;
};