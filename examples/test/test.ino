/*
 * Test sketch for the RFID lock prop
 * 
 * This flashes the lights, sounds the buzzer and reacts to an RFID card, if one is presented
 * 
 */

#include <LarpHackableRfidLock.h>

uint32_t test_interval = 5000;
uint32_t time_of_last_test = 0;
uint8_t test_type = 0;
uint8_t number_of_tests = 4;

void setup() {
  Lock.begin(); //Start the lock
}

void loop() {
  Lock.Housekeeping();
  if (Lock.CardPresented())
  {
    Lock.RedLedOn();
    Lock.GreenLedOn();
    Lock.BuzzerOn(220);
    delay(1000);
    Lock.RedLedOff();
    Lock.GreenLedOff();
    Lock.BuzzerOff();
    test_type = 0;
    time_of_last_test = millis();
  }

  if(millis() - time_of_last_test > test_interval)
  {
    time_of_last_test = millis();
    switch (test_type) {
    case 0:
      Lock.RedLedOn();
      break;
    case 1:
      Lock.RedLedOff();
      break;
    case 2:
      Lock.GreenLedOn();
      break;
    case 3:
      Lock.GreenLedOff();
      break;
    case 4:
      Lock.BuzzerOn(440);
      break;
    case 5:
      Lock.BuzzerOff();
      break;
    default:
      break;
    }
    test_type++;
    test_type = test_type%number_of_tests;
  }
}
