/*
 * Example sketch for the lock prop, using Tap Code to control the lock
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

uint32_t lastStats = 0;

char codeToOpen[] = "Open";
char codeToLock[] = "Lock";
char codeToUnlock[] = "Unlock";

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
  Lock.enableButton3(); //Enable button 3 on default pin
  Lock.enableTapCode(Lock.button3());  //Enable tap code
  Lock.begin(); //Start the lock
}

void loop() {
  Lock.housekeeping();
  if(millis() - lastStats > 5000)
  {
    lastStats = millis();
    Serial.print(F("Uptime: "));
    Serial.print(float(millis())/60000.0);
    Serial.println(F(" minutes"));
  }
}
