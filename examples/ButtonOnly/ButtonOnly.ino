/*
 * Example sketch for the lock prop, using Tap Code, PIN and/or RFID to control the lock
 * 
 */

#define DEBUG_ENABLED

#include <LarpHackableRfidLock.h>
#ifdef DEBUG_ENABLED
  #if ARDUINO_USB_CDC_ON_BOOT == 1
      #define SERIAL_DEBUG_PORT Serial
  #else
      #define SERIAL_DEBUG_PORT USBSerial
  #endif
#endif

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
  Lock.enableButton2(); //Enable button 2 on default pin
  Lock.enableButton3(); //Enable button 3 on default pin
  Lock.enableOpenButton(Lock.button2()); //Enable button 3 on default pin
  Lock.begin(); //Start the lock with a default access ID
}

void loop() {
  Lock.housekeeping();  //This is all you need for basic functionality and interactive behaviour baked into the library
}
