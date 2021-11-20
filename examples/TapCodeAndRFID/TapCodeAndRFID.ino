/*
 * Example sketch for the RFID lock prop, using Tap Code to control the lock
 * 
 */

#include <LarpHackableRfidLock.h>

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); //Note SERIAL_TX_ONLY allows use of RX for a button
  Lock.Debug(Serial);
  Lock.begin(); //Start the lock
  Lock.enableTapCode();
  Lock.enableRFID();
}

void loop() {
  Lock.Housekeeping();
  if(Lock.CardPresent() && Lock.CardChanged())
  {
    Lock.BuzzerOn(440,100);  //Just blip the buzzer
  }
}
