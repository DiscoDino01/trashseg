#include <Arduino.h>

void setup() {
	Serial.begin(9600);
}

void loop() {
	Serial.print("\n\"hello esp8266!\"\n");
	delay(600);
}