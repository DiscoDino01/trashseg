#include <Arduino.h>
#include <stdint.h>
#include <Servo.h>
#include "HX711.h"
#include "floattypes.h"

#define STCAST(x, type) (static_cast<type>(x))
#define BoolWord(x) ((x)?"True":"False")


#include "Pin/Pin.hpp"
#include "SerialStream/SerialStream.hpp"

#include "LiquidCrystal_I2C.h"

namespace Pins
{
    Pin LoadCellDout(A0);
    Pin LoadCellSCK(A1);

    Pin Capacitive(A2);
    Pin Servo(9);
} // namespace Pins

auto weight = HX711();
auto servo = Servo();

int32_t triangle(int32_t val, int32_t min, int32_t max)
{
    int32_t range = max - min;                   // Calculate the range
    int32_t mod_val = (val - min) % (2 * range); // Wrap value within 0 to 2*range

    if (mod_val < 0) // Handle negative values correctly
        mod_val += 2 * range;

    if (mod_val > range)
        return max - (mod_val - range); // Descending part
    else
        return min + mod_val; // Ascending part
}
LiquidCrystal_I2C a(0x27, 16,2);
void setup()
{
	a.begin(16,2);
	a.setBacklight(2);
    ::Serial.begin(9600);
    // ::weight.begin(Pins::LoadCellDout, Pins::LoadCellSCK);
    // ::servo.attach(Pins::Servo);
}

void loop()
{
    delay(200);
    // bool weight_over = false ;
    // auto capacitive_data = analogRead(Pins::Capacitive);

    // if (weight_over && !capacitive_data) {
    //     ::servo.write(0);
    // } else if (!weight_over && capacitive_data > 500) {
    //     ::servo.write(179);
    // } else {
    //     ::servo.write(90);
    // }

    cout << "hi\n";
	a.setCursor(0,0);
	a.print("...");
}