#pragma once

//#ifndef ULTRASONIC_H
#include <Arduino.h>
//#define ULTRASONIC_H

class Ultrasonic {
private:
	unsigned trigger, echo;
public:
	static const float cmRatio = 0.017F;

	unsigned get_trigpin() {
		return trigger;
	}
	unsigned get_echopin(){return echo;}

	/**
	 * 
	 */
	void init(unsigned trigpin/* = -1 */, unsigned echopin/* = -1 */) {
		//if (trigpin == -1 || echopin == -1)
		//	return;
		//

		trigger = trigpin;
		echo = echopin;

		pinMode(trigger, OUTPUT);
		pinMode(echo, INPUT);
		
	};

	/**
	 * @brief
	 * @return duration
	 */
	unsigned long detect() {
		digitalWrite(trigger, LOW);
		delayMicroseconds(2);
		digitalWrite(trigger, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigger, LOW);

		unsigned long duration = pulseIn(echo, HIGH);
		return duration;
	};


	/**
	 * @param void
	 * @return CM in distance
	 * @brief 
	 */
	float detectCM() {
		return ( (float) detect() ) * cmRatio;
	};
};

//#endif