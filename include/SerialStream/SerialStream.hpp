#pragma once

#include <Arduino.h>

class SerialStreamPrint {
public:
	bool enabled = true;
	// Constructor to allow specifying which HardwareSerial port to use, defaults to Serial
	SerialStreamPrint(HardwareSerial& hs = Serial) : _serial(hs) {}

	template<typename T>
	SerialStreamPrint& operator<<(const T& value) {
		if(enabled)_serial.print(value);
		return *this;
	}

	// Support for manipulators like '\n'
	SerialStreamPrint& operator<<(const char& value) {
		if(!enabled)return*this;

		if (value == '\n') {
			_serial.println();
		} else {
			_serial.print(value);
		}
		return *this;
	}

private:
	HardwareSerial& _serial; // Reference to the selected HardwareSerial port
	
};


/// Declare (more like define) global instances

// you can now `cout` like in regular c++! -- [[stupid extern wont work]]
/* extern */ static SerialStreamPrint cout;
const char endl = '\n';
