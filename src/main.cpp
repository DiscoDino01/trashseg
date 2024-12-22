//#include <stdio.h>
#undef cout

#include <Arduino.h>
#include <Vector.h>
#include "SerialStream/SerialStream.hpp"

#include "OtherBoard/ob.hpp"

//#define UNOBAUD 9600
#define OUT OUTPUT
#define IN INPUT

#define UltrsncTrigPin A0
#define UltrsncEchoPin 8


auto wait = delay;
auto s = &Serial;

Vector<uint8>& rand_3_num() {
	static uint8 a[3];
    static Vector<uint8> code(a, 3);
	for (unsigned int i = 0; i < 3; i++) {
		code[i] = (uint8)(rand() % 10);
	};
    
    return code;
};



///            ///
/// Namespaces ///
///            ///

namespace Ultrasonic {
	const float cmRatio = 0.017F;
	byte trigger, echo;

	/**
	 * 
	 */
	void init(byte trigpin/* = -1 */, byte echopin/* = -1 */) {
		//if (trigpin == -1 || echopin == -1)
		//	return;
		//

		trigger = trigpin;
		echo = echopin;

		pinMode(trigger, OUT);
		pinMode(echo, IN);
		
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
	 * @brief hi
	 */
	float detectCM() {
		return ( (float) detect() ) * cmRatio;
	};
};


/*
** SETUP AND LOOP
*/

void setup() {
	s->begin(/* UNOBAUD = */ 9600);
	Ultrasonic::init(UltrsncTrigPin, UltrsncEchoPin);

}

void loop() {
	wait(1000);
	
	cout << Ultrasonic::detectCM() << " cm" << '\n';
}