/*
 * Example sketch for the lock prop, using Tap Code, PIN and/or RFID to control the lock
 * 
 */
#include <LarpHackableRfidLock.h>

//#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
  #if ARDUINO_USB_CDC_ON_BOOT == 1
      #define SERIAL_DEBUG_PORT Serial
  #else
      #define SERIAL_DEBUG_PORT USBSerial
  #endif
#endif

#ifdef DEBUG_ENABLED
  uint32_t lastStats = 0;
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
  Lock.enableRedLed();  //Enable the red LED on default pin
  Lock.enableGreenLed();  //Enable the green LED on default pin
  Lock.enableMatrixKeypad();  //Enable the matrix keypad on default pins
  #ifndef DEBUG_ENABLED
    Lock.enableBuzzer();  //Enable the buzzer on the default pin
  #endif
  Lock.enableRFID();  //Enable RFID reader authorisation, CS on default pin, using default card sector
  Lock.begin(); //Start the lock with a default access ID
}

void loop() {
  Lock.housekeeping();  //This is all you need for basic functionality and interactive behaviour baked into the library
  #ifdef DEBUG_ENABLED
    if(millis() - lastStats > 5000)
    {
      lastStats = millis();
      Serial.print(F("Uptime: "));
      Serial.print(float(millis())/60000.0);
      Serial.println(F(" minutes"));
    }
  #endif
}
