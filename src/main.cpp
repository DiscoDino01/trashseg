#include <Arduino.h>
#include <HX711.h>
#include <SPI.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdint.h>

#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"
#include "floattypes.h"
#define ANALOG
#define DIGITAL

// #define LOAD_CELL_ENABLED
//#define OLED_DISPLAY_ENABLED

constexpr const float32_t weight_minthreshold = 0.1F;
constexpr const float32_t weight_maxthreshold = 3.0F;

#ifdef OLED_DISPLAY_ENABLED
Adafruit_SSD1306 display(128U, 32U, &Wire);
#endif

/// @brief Definition of all using pins
namespace Pins {
	DIGITAL Pin PaperServo(9);		// automatically pinMode-d by `Servo::attach`
	DIGITAL Pin PlasticServo(10);	// ^~~~~~~~
	DIGITAL Pin TopCoverServo0(11);	// ^~~~~~~~~~~~~~~~~~~~~
	DIGITAL Pin TopCoverServo1(6);

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
namespace BinChute { /// Servo: Surpass Hobby S0600M Digital Servo
	constexpr const uint16_t degree_per_sec = 250U; // possibly? manufacturer didn't send datasheet

	Servo paper_servo;
	Servo plastic_servo;
	Servo top_cover_servo0;
	Servo top_cover_servo1;

	void init() {
		paper_servo.attach(Pins::PaperServo);
		paper_servo.write(0);
		delay(500);
		//paper_servo.detach();
		
		plastic_servo.attach(Pins::PlasticServo);
		plastic_servo.write(0);
		delay(500);
		//plastic_servo.detach();
		
		top_cover_servo0.attach(Pins::TopCoverServo0);
		top_cover_servo0.write(0);
		//delay(500);

		top_cover_servo1.attach(Pins::TopCoverServo1);
		top_cover_servo1.write(0);
		delay(500);
		//top_cover_servo.detach();
	}

	void open_top_chute() {
		//top_cover_servo.attach(Pins::TopCoverServo);
		top_cover_servo1.write(90);
		top_cover_servo0.write(90);
		delay(700);
		//top_cover_servo.detach();
	}

	void close_top_chute() {
		//top_cover_servo.attach(Pins::TopCoverServo);
		top_cover_servo1.write(0);
		top_cover_servo0.write(0);
		delay(700);
		//top_cover_servo.detach();
	}

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
			//servo->attach(Pins::PaperServo);
			break;
		}

		case (Paper): {
			servo = &plastic_servo;
			//servo->attach(Pins::PlasticServo);
			break;
		}

		case (Unknown):
		default: {
			return;
		}
		}

		

		servo->write(90);
		delay(3000);
		//servo->write(0);
		//delay(1200);
		//servo->detach();
	}

	/**
	 * \brief Closes both the paper and plastic chute, by their servo.
	 */

	void close_both_chute() {
		//paper_servo.attach(Pins::PaperServo);
		paper_servo.write(0);
		delay(500);
		//paper_servo.detach();

		//plastic_servo.attach(Pins::PlasticServo);
		plastic_servo.write(0);
		delay(500);
		//plastic_servo.detach();
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
	if (photoelectric_data0 > 200) {
		return Plastic;
	}

	return Paper;
}

//~ setup && loop ~//

void setup() {
	Serial.begin(/* UNOBAUD: */ 9600);	// for faster prints: 115200

	pinMode(LED_BUILTIN, OUTPUT);

	BinChute::init();
#ifdef OLED_DISPLAY_ENABLED
	if (!display.begin())
		for(;;);
#endif

#ifdef LOAD_CELL_ENABLED
	LoadCell::init();
#endif

	delay(3000);  // wait for 3 seconds
}

void loop() {
	delay(800); // throttle

	float32_t weightKg =
#ifdef LOAD_CELL_ENABLED
		static_cast<float32_t>(LoadCell::instance.read() / 1e6l) * LoadCell::analogtokg;

	if (weightKg < weight_minthreshold)
		return;
#else
		0.6F;  // example data!
#endif

	digitalWrite(LED_BUILTIN, 1);
	//Serial << "\n\"start\"\n";
#ifdef OLED_DISPLAY_ENABLED
	display.setCursor(0, 0);


	{
		char trash_countdown_time_message[] = "Rem. Time 15s"; // 10, 11
		for (uint8_t rem = 15; rem >= 0; rem--) {
			trash_countdown_time_message[10] = '0' + (rem / 10u);
			trash_countdown_time_message[11] = '0' + (rem % 10u);
			display.print(trash_countdown_time_message);
			delay(1000);
			display.clearDisplay();
		}
	}
#else
	delay(5000);
#endif


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
		while(1); // halt
	;

	Serial << "\n[" << (static_cast<int>(determinedTrashType)) << ','
		<< (roundf(weightKg * 100.0f) / 100.0f) << "]\n";

	BinChute::open_chute(determinedTrashType);
	delay(400);  // does it have time to unload the current on the servo?
	BinChute::open_top_chute();
	delay(5000);
	BinChute::close_top_chute();
	delay(800);
	BinChute::close_both_chute();

	digitalWrite(LED_BUILTIN, 0);

	return;
}