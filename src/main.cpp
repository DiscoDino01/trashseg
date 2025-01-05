#include <Arduino.h> /* header files | required modules */
#include <Vector.h>
#include <Servo.h>
#include "RustType/RustType.hpp"

#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"
#include "Ultrasonic/Ultrasonic.h"

#define ANALOG
#define DIGITAL
#define TESTING
#define ref

#define HALT while(1);

/// QOL variables|functions ///

void wait(f32 seconds) {delay(seconds * 1000);}
auto ms_wait = delay;
auto mc_wait = delayMicroseconds;

const f32 weight_minthreshold = 0.1F;
const f32 weight_maxthreshold = 3.0F;


/// @brief Definition of all using pins
namespace Pins {
	namespace Ultrasonic {
		Pin trig(A0);
		Pin echo(8);
	};

	Pin PaperServo(9); 		// automatically pinMode-ed by servo.attach
	Pin PlasticServo(10);	// ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	DIGITAL Pin PhotoEl_1(3, INPUT);
	DIGITAL Pin PhotoEl_2(4, INPUT);
	DIGITAL Pin PhotoEl_3(5, INPUT);

	ANALOG Pin Capacitive(A0, INPUT);
	ANALOG Pin Weight(A1, INPUT);
}


enum TrashType {
	Unknown = 0,
	Plastic,
	Paper,
};


/// Component Controllers ///
namespace BinChute {
	Servo paper_servo;
	Servo plastic_servo;
	Servo top_cover_servo;


	void init() {
		paper_servo.attach(Pins::PaperServo);
		plastic_servo.attach(Pins::PlasticServo);
		paper_servo.write(0);
		plastic_servo.write(0);
	}


	void open_top_chute() {

	}

	void close_top_chute() {

	}

	void open_chute(TrashType tt) {
		Servo *servo = nullptr;
		switch (tt) {
			case (Plastic):
				servo = &paper_servo;
				break;
			
			case (Paper):
				servo = &plastic_servo;
				break;

			case (Unknown):
			default: {return;}
		}

		servo->write(90);
		wait(3);
		servo->write(0);
		wait(1.2);
	}
};



/// functions i guess ///

i32 triangle_wave(i32 n, i32 floor, i32 ceiling) {
	i32 range = ceiling - floor;

	i32 cyclePosition = (n - floor) % (2 * range);
	if (cyclePosition < 0) {
		cyclePosition += 2 * range;
	}

	if (cyclePosition > range) {
		return ceiling - (cyclePosition - range);
	} //else
	return floor + cyclePosition;
};


inline
const TrashType determine_trashtype(i32 photoelectric_data, i32 capacitive_data) {
	if (photoelectric_data == LOW) {
		return Plastic;
	}

	return Paper;
}


/// setup && loop ///

void setup() {
	Serial.begin(/*UNOBAUD*/ 9600); // for faster prints: 115200

	//Ultrasonic1.init(Pins::Ultrasonic::trig, Pins::Ultrasonic::echo);
	BinChute::init();

	wait(3); // wait for 3 seconds
}

void loop() {
	static f32 analogtokg = 1;
	wait(1); // throttle
	
	ANALOG f32 weightKg = static_cast<f32>(Pins::Weight.read()) * analogtokg;
	if (weightKg < weight_minthreshold) return;

	ANALOG i32 CapacitiveData = Pins::Capacitive.read();

	DIGITAL bool PE1 = Pins::PhotoEl_1.read();
	DIGITAL bool PE2 = Pins::PhotoEl_2.read();
	DIGITAL bool PE3 = Pins::PhotoEl_3.read();
	
	TrashType determinedTrashType = determine_trashtype(PE1, CapacitiveData);
	if (determinedTrashType == Unknown)
		return;

	BinChute::open_chute(determinedTrashType);
	BinChute::open_top_chute();

}