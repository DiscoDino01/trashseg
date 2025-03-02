#include <Arduino.h>
#include <HX711.h>
#include <SPI.h>
#include <Servo.h>
#include <Wire.h>
#include <stdint.h>

#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"
#include "floattypes.h"
#define ANALOG
#define DIGITAL

// #define LOAD_CELL_ENABLED

constexpr const float32_t weight_minthreshold = 0.1F;
constexpr const float32_t weight_maxthreshold = 3.0F;

/// @brief Definition of all using pins
namespace Pins {
	DIGITAL Pin PaperServo(9);		// automatically pinMode-d by `Servo::attach`
	DIGITAL Pin PlasticServo(10);	// ^~~~~~~~
	DIGITAL Pin TopCoverServo(11);	// ^~~~~~~~~~~~~~~~~~~~~

	ANALOG Pin PhotoEl_1(A0, INPUT);
	// ANALOG Pin PhotoEl_2(A1, INPUT);

	ANALOG Pin Capacitive(A2, INPUT);

#ifdef LOAD_CELL_ENABLED
	ANALOG Pin LoadCellDout(A6, INPUT);
	ANALOG Pin LoadCellSCK(A7, INPUT);
#endif
}

enum TrashType : uint8_t {
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

	void open_top_chute() { top_cover_servo.write(90); }

	void close_top_chute() { top_cover_servo.write(0); }

	/**
	 * \brief The chute opens the opposite TrashType, i.e. Plastic moves the Paper's
	 * servo, Paper moves the Plastic's servo.
	 *
	 */
	void open_chute(TrashType tt) {
		Servo* servo = nullptr;
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
		default: {
			return;
		}
		}

		servo->write(90);
		delay(3000);
		servo->write(0);
		delay(1200);
	}

	/**
	 * \brief Closes both the paper and plastic chute, by their servo.
	 */
		
	void close_both_chute() {
		paper_servo.write(0);
		plastic_servo.write(0);
	}

};

#ifdef LOAD_CELL_ENABLED
namespace LoadCell {
	constexpr const float32_t analogtokg = 1.0f;
	HX711 instance;

	void init() { instance.begin(Pins::LoadCellDout, Pins::LoadCellSCK); }
}  // namespace LoadCell
#endif

TrashType determine_trashtype(int32_t photoelectric_data0, int32_t capacitive_data) {
	if (photoelectric_data0 == LOW) {
		return Plastic;
	}

	return Paper;
}

//~ setup && loop ~//

void setup() {
	Serial.begin(/* UNOBAUD: */ 9600);	// for faster prints: 115200

	pinMode(LED_BUILTIN, OUTPUT);

	BinChute::init();

#ifdef LOAD_CELL_ENABLED
	LoadCell::init();
#endif

	delay(3000);  // wait for 3 seconds
}

void loop() {
	delay(800);	 // throttle


	float32_t weightKg =
#ifdef LOAD_CELL_ENABLED
		static_cast<float32_t>(LoadCell::instance.read() / 1e6l) * LoadCell::analogtokg;

	if (weightKg < weight_minthreshold)
		return;
#else
		0.6F;  // example data!
#endif


	Serial << "\n\"start\"\n";
	digitalWrite(LED_BUILTIN, 1);
	delay(15000);


	weightKg =	// redefinition for trash weight
#ifdef LOAD_CELL_ENABLED
		static_cast<float32_t>(LoadCell::instance.read() / 1e6l) * LoadCell::analogtokg;

	if (weightKg < weight_minthreshold)
		return;
#else
		0.611F;	// example data!
#endif


	int32_t CapacitiveData = Pins::Capacitive.read();
	int32_t PE1 = Pins::PhotoEl_1.read();

	TrashType determinedTrashType = determine_trashtype(PE1, CapacitiveData);

	if (determinedTrashType == Unknown)
		while (1);
	;

	Serial << "\n[" << (static_cast<int>(determinedTrashType)) << ','
		   << (roundf(weightKg * 100.0f) / 100.0f) << "]\n";

	BinChute::open_chute(determinedTrashType);
	delay(1200);  // does it have time to unload the current on the servo?
	BinChute::open_top_chute();
	delay(2200);
	BinChute::close_top_chute();
	delay(1100);
	BinChute::close_both_chute();
	delay(1100);

	digitalWrite(LED_BUILTIN, 0);

	return;
}