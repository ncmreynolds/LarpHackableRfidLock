/*
 * Example sketch for the lock prop, using Tap Code, PIN and/or RFID to control the lock
 * 
 */

#define DEBUG_ENABLED
#define ENABLE_SERVO

#include <LarpHackableRfidLock.h>
#ifdef DEBUG_ENABLED
  #if ARDUINO_USB_CDC_ON_BOOT == 1
      #define SERIAL_DEBUG_PORT Serial
  #else
      #define SERIAL_DEBUG_PORT USBSerial
  #endif
#endif

uint32_t lastStats = 0;

void setup() {
  #ifdef DEBUG_ENABLED
    #if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
      SERIAL_DEBUG_PORT.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); //Note SERIAL_TX_ONLY allows use of RX for a button
    #elif defined(ARDUINO_ESP32C3_DEV)
      #if ARDUINO_USB_CDC_ON_BOOT == 1
        SERIAL_DEBUG_PORT.begin();
      #else
        SERIAL_DEBUG_PORT.begin(115200);
      #endif
    #endif
    Lock.debug(SERIAL_DEBUG_PORT);
    delay(5000);
  #endif
  #ifndef DEBUG_ENABLED
    Lock.enableButton1(); //Enable button 1 on default pin
  #endif
  //Lock.enableButton2(); //Enable button 2 on default pin
  //Lock.enableButton3(); //Enable button 3 on default pin
  Lock.enableRedLed();  //Enable the red LED on default pin
  Lock.enableMatrixKeypad();  //Enable the matrix keypad on default pins
  Lock.addPinToOpen((char*)"11111");
  Lock.addPinToLock((char*)"22222");
  Lock.addPinToUnlock((char*)"33333");
  Lock.addPinToWipeNextCard((char*)"44444");
  Lock.addPinToAllowNextCard((char*)"55555");
  Lock.addPinToDenyNextCard((char*)"66666");
  Lock.setPinLength(5); //Change PIN length from 8 to 5
  //Lock.setPinTimeout(1000); //Change PIN entry timeout to 1000ms
  Lock.enableGreenLed();  //Enable the green LED on default pin
  #ifndef DEBUG_ENABLED
    Lock.enableBuzzer();  //Enable the buzzer on the default pin
  #endif
  Lock.enableTapCode(Lock.button3());  //Enable tap code on previously enabled button 3
  Lock.addCodeToOpen((char*)"Open");
  Lock.addCodeToLock((char*)"Lock");
  Lock.addCodeToUnlock((char*)"Unlock");
  Lock.addCodeToWipeNextCard((char*)"Wipe");
  Lock.addCodeToAllowNextCard((char*)"Allow");
  Lock.addCodeToDenyNextCard((char*)"Deny");
  Lock.enableRFID();  //Enable RFID reader authorisation, CS on default pin, using default card sector
  Lock.begin(); //Start the lock with a default access ID
}

void loop() {
  Lock.housekeeping();  //This is all you need for basic functionality and interactive behaviour baked into the library
  if(millis() - lastStats > 5000)
  {
    lastStats = millis();
    Serial.print(F("Uptime: "));
    Serial.print(float(millis())/60000.0);
    Serial.println(F(" minutes"));
  }
}

#ifdef ENABLE_SERVO
void enableServo()
{
  
}
#endif
