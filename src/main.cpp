#include <Arduino.h> /* header files | required modules */
#include <Vector.h>
#include <Servo.h>
#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"
#include "Ultrasonic/Ultrasonic.h"



/// QOL variables|functions ///

void wait(float seconds) {delay(seconds * 1000);}
auto ms_wait = delay;
auto mc_wait = delayMicroseconds;


/// @brief Definition of all using pins
namespace Pins {
	namespace Ultrasonic {
		Pin trig(A0);
		Pin echo(8);
	};

	Pin Servo(9); // automatically pinMode-ed by servo.attach
	Pin PhotoEl_1(10, INPUT);
	Pin PhotoEl_2(11, INPUT);
	Pin PhotoEl_3(12, INPUT);

	Pin Capacitive(A0, INPUT);
	Pin Weight(A1, INPUT);
}


enum TrashType {
	Plastic = 1,
	Paper,
};


/// Component Controllers ///

Ultrasonic Ultrasonic1;
Servo servo;


/// functions i guess ///

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

inline TrashType determine_trashtype(int photoelectric_data, int capacitive_data) {
	if (photoelectric_data == LOW){
		return Plastic;
	}

	return Paper;
}

/// setup && loop ///

void setup() {
	Serial.begin(/*UNOBAUD*/ 115200);

	Ultrasonic1.init(Pins::Ultrasonic::trig, Pins::Ultrasonic::echo);

	servo.attach(Pins::Servo);
	servo.write(0); // initialize to angle 0
	
	wait(3); // wait for 3 seconds
}

void loop() {
	wait(1);
	float distCM = Ultrasonic1.detectCM();
	float weightKG = Pins::Weight.read(); // TODO: convert analog signal to kilograms
	if (distCM > 50.0F && weightKG) {
		return;
	}
	
	// boolean because its digital pins
	bool PE_data1 = Pins::PhotoEl_1.read() == HIGH;
	bool PE_data2 = Pins::PhotoEl_2.read() == HIGH;
	bool PE_data3 = Pins::PhotoEl_3.read() == HIGH;

	int Capacitive_data = Pins::Capacitive.read(); // analog

	TrashType trashtype = determine_trashtype(PE_data1 == true ? HIGH : LOW, Capacitive_data);
}