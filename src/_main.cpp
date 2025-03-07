#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Fonts/FreeMono12pt7b.h>


uint8_t pin_servo[3u] = {9u, 10u, 11u};
Servo list[3] = {{}, {}, {}};

Adafruit_SSD1306 display(128, 32, &Wire);

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false)) while(1);
	display.setFont(&FreeMono12pt7b);
	display.display();
	display.setCursor(0, 16);
	display.print("Starting...");
	display.display();
	delay(100);
	display.clearDisplay();
	display.display();

	for (uint8_t i = 0; i < 3; i++) {
		auto& servo = list[i];
		servo.attach(pin_servo[i]);
		servo.write(0);
	}
}

void loop(void) {
	display.clearDisplay();
	display.display();

	static char message[] = "Moving servo0";
	for (uint8_t i = 0; i < 3; i++) {
		message[13] = '1' + i;
		display.print(message);
		display.display();
		digitalWrite(LED_BUILTIN, 1);
		auto& servo = list[i];
		servo.write(0);
		delay(1000);
		servo.write(180);
		delay(3000);
		digitalWrite(LED_BUILTIN, 0);
		delay(1000);

		display.clearDisplay();
		display.display();
	}
}