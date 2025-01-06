// the ridiculous use of namespaces :)

#include <Arduino.h> /* header files | required modules */
#include <Vector.h>
#include <Servo.h>
#include "Types/Types.hpp"

#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"

#define ANALOG  
#define DIGITAL  
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
	DIGITAL Pin PaperServo(9); 		// automatically pinMode-d by `Servo::attach`
	DIGITAL Pin PlasticServo(10);	// ^
	DIGITAL Pin TopCoverServo(11);	// ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
		top_cover_servo.attach(Pins::TopCoverServo);

		paper_servo.write(0);
		plastic_servo.write(0);
		top_cover_servo.write(0);
	}


	void open_top_chute(bool delay = false) {
		top_cover_servo.write(90);
		if (delay) wait(1.5f);
	}

	void close_top_chute(bool delay = false) {
		top_cover_servo.write(0);
		if (delay) wait(1.5f);
	}

	/**
	 * \brief The chute opens the opposite TrashType, i.e. Plastic moves the Paper's servo, Paper moves the Plastic's servo.
	 * 
	 */
	void open_chute(TrashType tt) {
		Servo *servo = nullptr;
		switch (tt) {
			case (Plastic): {
				servo = &paper_servo;
				break;
			}
			
			case (Paper): {
				servo = &plastic_servo;
				break;
			}

			case (Unknown):
			default: {return;}
		}

		servo->write(90);
		wait(3);
		servo->write(0);
		wait(1.2);
	}

	/**
	 * \brief Closes both the paper and plastic chute, by their servo.
	 */
	void close_both_chute(bool delay = false) {
		paper_servo.write(0);
		plastic_servo.write(0);

		if (delay) wait(1.5f);
	}

	/**
	 * \brief `ANALOG i32 read_weight()`
	 */
	ANALOG i32 read_weight() {
		return Pins::Weight.read();
	}

	
};



/// functions i guess ///

TrashType determine_trashtype(i32 photoelectric_data, i32 capacitive_data) {
	if (photoelectric_data == LOW) {
		return Plastic;
	}

	return Paper;
}


//~ setup && loop ~//

void setup() {
	Serial.begin(/*UNOBAUD*/ 9600); // for faster prints: 115200

	BinChute::init();

	wait(3); // wait for 3 seconds
};


void loop() {
	static f32 analogtokg = 1;
	wait(0.8f); // throttle
	
	f32 weightKg = static_cast<f32>(BinChute::read_weight()) * analogtokg;
	if (weightKg < weight_minthreshold)
		return;
		/*
	else if (weightKg > weight_maxthreshold)
		HALT; // stop execution*/

	a_val CapacitiveData = Pins::Capacitive.read();

	d_val PE1 = Pins::PhotoEl_1.read();
	d_val PE2 = Pins::PhotoEl_2.read();
	d_val PE3 = Pins::PhotoEl_3.read();
	
	TrashType determinedTrashType = determine_trashtype(PE1, CapacitiveData);
	if (determinedTrashType == Unknown)
		return;

	BinChute::open_chute(determinedTrashType);
	BinChute::open_top_chute(true);
	wait(2.2f);
	BinChute::close_top_chute(true);
	BinChute::close_both_chute(true);
}