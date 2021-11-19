/*
 * Example sketch for the RFID lock prop, using Tap Code to control the lock
 * 
 */

#include <LarpHackableRfidLock.h>

uint32_t test_interval = 1000;
uint32_t time_of_last_test = 0;
uint8_t test_type = 0;
uint8_t number_of_tests = 4;

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); //Note SERIAL_TX_ONLY allows use of RX for a button
  Lock.Debug(Serial);
  Lock.begin(); //Start the lock
}

void loop() {
  Lock.Housekeeping();
  if (Lock.CardPresented())
  {
    Lock.RedLedOn();
    Lock.GreenLedOn();
    Lock.BuzzerOn(220);
    test_type = 5;
    time_of_last_test = millis();
  }

  if(millis() - time_of_last_test > test_interval)
  {
    time_of_last_test = millis();
    switch (test_type) {
    case 0:
      Lock.RedLedOn();
      Lock.GreenLedOff();
      Lock.BuzzerOff();
      break;
    case 1:
      Lock.RedLedOff();
      Lock.GreenLedOff();
      Lock.BuzzerOff();
      break;
    case 2:
      Lock.RedLedOff();
      Lock.GreenLedOn();
      Lock.BuzzerOff();
      break;
    case 3:
      Lock.RedLedOff();
      Lock.GreenLedOff();
      Lock.BuzzerOff();
      break;
    case 4:
      Lock.RedLedOff();
      Lock.GreenLedOff();
      Lock.BuzzerOn(440);
      break;
    case 5:
      Lock.RedLedOff();
      Lock.GreenLedOff();
      Lock.BuzzerOff();
      break;
    default:
      break;
    }
    test_type++;
    test_type = test_type%number_of_tests;
  }
}
