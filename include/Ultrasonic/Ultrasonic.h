#pragma once

//#ifndef ULTRASONIC_H
#include <Arduino.h>
#include "RustType/RustType.hpp" 
//#define ULTRASONIC_H

class Ultrasonic {
private:
	u8 trigger, echo;
public:
	static const f32 cmRatio = 0.017F;

	u8 get_trigpin() {
		return trigger;
	}
	u8 get_echopin(){return echo;}

	/**
	 * 
	 */
	void init(u8 trigpin/* = -1 */, u8 echopin/* = -1 */) {
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
	u64 detect(i32 firstMicrosecondDelay = 2, i32 secondMicroSecondDelay = 10) {
		digitalWrite(trigger, LOW);
		delayMicroseconds(firstMicrosecondDelay);
		digitalWrite(trigger, HIGH);
		delayMicroseconds(secondMicroSecondDelay);
		digitalWrite(trigger, LOW);

		u64 duration = pulseIn(echo, HIGH);
		return duration;
	};


	/**
	 * @param void
	 * @return CM in distance
	 * @brief 
	 */
	f32 detectCM() {
		return ( static_cast<f32>( detect() ) ) * cmRatio;
	};
};

//#endif