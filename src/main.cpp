#include <Arduino.h> /* header files | required modules */
#include <Vector.h>
#include <Servo.h>
#include "SerialStream/SerialStream.hpp"
#include "Ultrasonic/Ultrasonic.h"


/// QOL variables|functions ///

auto s = &Serial;
void wait(int seconds) {delay(seconds * 1000);}
auto ms_wait = delay;
auto mc_wait = delayMicroseconds;


/// @brief Definition of all using pins
namespace Pins {
    namespace Ultrasonic {
        int trigger_pin = A0;
        int echo_pin = 8;
    }

    int Servo = 9;
}


/// Component Controllers ///

Ultrasonic uts1(Pins::Ultrasonic::trigger_pin, Pins::Ultrasonic::echo_pin);
Servo servo;


// functions i guess //

int triangle_wave(int n, int floor, int ceiling) {
    int range = ceiling - floor;

    int cyclePosition = (n - floor) % (2 * range);
    if (cyclePosition < 0) {
        cyclePosition += 2 * range;
    }

    if (cyclePosition > range) {
        return ceiling - (cyclePosition - range);
    } //else 
    return floor + cyclePosition;    
}


int angle = 0;
void servo_next_angle(int multiplier = 1) {
    angle < 360 ? angle++ : angle = 1;
    servo.write(triangle_wave(angle * multiplier, 0, 189));
}



/// setup && loop ///
void setup() {
	s->begin(/* UNOBAUD = */ 9600);
	servo.attach(Pins::Servo);
	servo.write(0); // initialize to angle 0

    wait(3); // wait for 3 seconds
}

void loop() {
	wait(0.3);
	auto dist = uts1.detectCM();
	//cout << dist << endl;
	//cout << "loop" << endl;

	if (dist <= 5.0F) {
        servo_next_angle(6);
	};
}