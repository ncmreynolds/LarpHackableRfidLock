#include <ESP32Servo.h>
#define PIN_SG90 5 // Output pin used

Servo sg90;

void setup() {
  sg90.setPeriodHertz(50); // PWM frequency for SG90
  sg90.attach(PIN_SG90, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
 }
void loop() {
 //rotation from 0 to 180°
  for (int pos = 0; pos <= 180; pos += 1) {
    sg90.write(pos);
    delay(10);
  }
 // Rotation from 180° to 0
  for (int pos = 180; pos >= 0; pos -= 1) {
    sg90.write(pos);
    delay(10);
  }
}
