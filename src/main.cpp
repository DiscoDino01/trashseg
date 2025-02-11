#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Arduino.h> /* header files | required modules */
#include <HX711.h>
#include <Servo.h>
#include <Vector.h>
#include <stdint.h>

#include "Color3.hpp"
#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"

#include "SlowSoftI2CMaster.h"
#include "floattypes.h"

#define ANALOG
#define DIGITAL

void wait(float32_t seconds) { delay(seconds * 1000); }
auto ms_wait = delay;
auto mc_wait = delayMicroseconds;

constexpr const float32_t weight_minthreshold = 0.1F;
constexpr const float32_t weight_maxthreshold = 3.0F;



/// @brief Definition of all using pins
namespace Pins
{
	ANALOG Pin PaperServo(9);	  // automatically pinMode-d by `Servo::attach`
	ANALOG Pin PlasticServo(10);  // ^~~~~~~~
	ANALOG Pin TopCoverServo(11); // ^~~~~~~~~~~~~~~~~~~~~

	ANALOG Pin PhotoEl_1(A0, INPUT);
	ANALOG Pin PhotoEl_2(A1, INPUT);

	ANALOG Pin Capacitive(A2, INPUT);

	ANALOG Pin LoadCellDout(A3, INPUT);
	ANALOG Pin LoadCellSCK(A4, INPUT);
} // namespace Pins

enum TrashType
{
	Unknown = 0,
	Plastic,
	Paper,
};

/// Component Controllers ///
namespace BinChute
{
	Servo paper_servo;
	Servo plastic_servo;
	Servo top_cover_servo;

	void init()
	{
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
	void open_chute(TrashType tt)
	{
		Servo *servo = nullptr;
		switch (tt)
		{
		case (Plastic):
		{
			servo = &paper_servo;
			break;
		}

		case (Paper):
		{
			servo = &plastic_servo;
			break;
		}

		case (Unknown):
		default:
		{
			return;
		}
		}

		servo->write(90);
		wait(3);
		servo->write(0);
		wait(1.2);
	}

	/**
	 * \brief Closes both the paper and plastic chute, by their servo.
	 */
	void close_both_chute()
	{
		paper_servo.write(0);
		plastic_servo.write(0);
	}

}; // namespace BinChute

namespace LoadCell
{
	HX711 instance;

	void init() { instance.begin(Pins::LoadCellDout, Pins::LoadCellSCK); }
} // namespace LoadCell

namespace ColorSensor
{
	Adafruit_TCS34725 S1;
	Adafruit_TCS34725 S2;

	void start()
	{
		if (!(S1.begin() && S2.begin()))
			while (1)
				;
	}
	void stop()
	{
		S1.disable();
		S1.disable();
	}
} // namespace ColorSensor

TrashType determine_trashtype(int32_t photoelectric_data0,
							  int32_t photoelectric_data1,
							  int32_t capacitive_data,
							  const Color3 &colordata0,
							  const Color3 &colordata1)
{
	if (photoelectric_data0 == LOW)
	{
		return Plastic;
	}

	return Paper;
}

//~ setup && loop ~//

void setup()
{
	Serial.begin(/* UNOBAUD: */ 9600); // for faster prints: 115200

	BinChute::init();
	LoadCell::init();

	wait(3); // wait for 3 seconds
}

void loop()
{
	static constexpr const float32_t analogtokg = 1;
	static constexpr const float32_t PlasticKgToSeconds = 0.4;
	static constexpr const float32_t PaperKgToSeconds = 0.3;

	wait(0.8f); // throttle

	float32_t weightKg =
		static_cast<float32_t>(LoadCell::instance.read() / 1e6l) * analogtokg;
	if (weightKg < weight_minthreshold)
		return;

	ANALOG int32_t CapacitiveData = Pins::Capacitive.read();

	ANALOG int32_t PE1 = Pins::PhotoEl_1.read();
	ANALOG int32_t PE2 = Pins::PhotoEl_2.read();

	ColorSensor::start();

	Color3 s1;
	Color3 s2;
	s1.from_tcs(ColorSensor::S1);
	s2.from_tcs(ColorSensor::S2);

	ColorSensor::stop();

	TrashType determinedTrashType = ::determine_trashtype(PE1, PE2, CapacitiveData, s1, s2);
	if (determinedTrashType == Unknown)
		return;

	BinChute::open_chute(determinedTrashType);
	BinChute::open_top_chute();
	wait(2.2f);
	BinChute::close_top_chute();
	wait(1.1f);
	BinChute::close_both_chute();
	wait(1.1f);

	Serial.print('[');
	Serial.print(
		static_cast<uint32_t>(determinedTrashType == Paper ? weightKg * PaperKgToSeconds : weightKg * PlasticKgToSeconds),
		HEX);

	Serial.print(']');

}