// the ridiculous use of namespaces :)

#include <Arduino.h> /* header files | required modules */
#include <HX711.h>
#include <Servo.h>
#include <Vector.h>
#include <stdint.h>

#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"
#include "floattypes.h"

#define ANALOG
#define DIGITAL


/// QOL variables|functions ///

void wait(float32_t seconds) { delay(seconds * 1000); }
auto ms_wait = delay;
auto mc_wait = delayMicroseconds;

const float32_t weight_minthreshold = 0.1F;
const float32_t weight_maxthreshold = 3.0F;

/// @brief Definition of all using pins
namespace Pins {
    DIGITAL Pin PaperServo(9);      // automatically pinMode-d by `Servo::attach`
    DIGITAL Pin PlasticServo(10);   // ^
    DIGITAL Pin TopCoverServo(11);  // ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    DIGITAL Pin PhotoEl_1(3, INPUT);
    DIGITAL Pin PhotoEl_2(4, INPUT);
    DIGITAL Pin PhotoEl_3(5, INPUT);

    ANALOG Pin Capacitive(A0, INPUT);

    ANALOG Pin LoadCellDout(A1, INPUT);
	ANALOG Pin LoadCellSCK(A2, INPUT);
}  // namespace Pins

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

    void open_top_chute() {
        top_cover_servo.write(90);
    }

    void close_top_chute() {
        top_cover_servo.write(0);
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
		default: {
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
    void close_both_chute() {
        paper_servo.write(0);
        plastic_servo.write(0);
    }

};  // namespace BinChute

namespace LoadCell {
    HX711 instance;

    void init() {
		instance.begin(Pins::LoadCellDout, Pins::LoadCellSCK);
    }
}  // namespace LoadCell


TrashType determine_trashtype(int32_t photoelectric_data, int32_t capacitive_data) {
    if (photoelectric_data == LOW) {
        return Plastic;
    }

    return Paper;
}

//~ setup && loop ~//

void setup() {
    Serial.begin(/*UNOBAUD*/ 9600);  // for faster prints: 115200

    BinChute::init();
	LoadCell::init();

    wait(3);  // wait for 3 seconds
}

void loop() {
    static float32_t analogtokg = 1;
    wait(0.8f);  // throttle

    float32_t weightKg = static_cast<float32_t>(LoadCell::instance.read() / 1e6l) * analogtokg;
    if (weightKg < weight_minthreshold)
        return;

    DIGITAL int32_t CapacitiveData = Pins::Capacitive.read();

    DIGITAL int32_t PE1 = Pins::PhotoEl_1.read();
    DIGITAL int32_t PE2 = Pins::PhotoEl_2.read();
    DIGITAL int32_t PE3 = Pins::PhotoEl_3.read();

    TrashType determinedTrashType = determine_trashtype(PE1, CapacitiveData);
    if (determinedTrashType == Unknown)
        return;

    BinChute::open_chute(determinedTrashType);
    BinChute::open_top_chute();
    wait(2.2f);
    BinChute::close_top_chute();
    wait(1.1f);
    BinChute::close_both_chute();
    wait(1.1f);
}