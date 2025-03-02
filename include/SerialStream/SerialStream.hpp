#pragma once

#include <Arduino.h>
template <typename T>
HardwareSerial& operator<<(HardwareSerial &hs, const T &val) {
	hs.print(val);
	return hs;
}