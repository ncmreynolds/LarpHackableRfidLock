/*
 * Example sketch for the lock prop, using Tap Code, PIN and/or RFID to control the lock
 * 
 */

//#define DEBUG_ENABLED

#include <LarpHackableRfidLock.h>
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
    //if(usbConnected())
    {
      Lock.debug(SERIAL_DEBUG_PORT);
    }
  #endif
  Lock.enableButton2(); //Enable button 2 on default pin, which will be used to start 'lock picking'
  Lock.enableRedLed();  //Enable the red LED on default pin
  Lock.enableGreenLed();  //Enable the green LED on default pin
  #ifndef DEBUG_ENABLED
    Lock.enableBuzzer();  //Enable the buzzer on the default pin. On the hardware I'm using the GPIO is shared with USB-CDC so disabled when DEBUG is enabled
  #else
    /*
    if(usbConnected() == false)
    {
      Lock.enableBuzzer();  //Enable the buzzer on the default pin
    }
    */
  #endif
  Lock.enableMatrixKeypad();  //Enable the matrix keypad on default pins
  Lock.enableRFID();  //Enable RFID reader authorisation, CS on default pin, using default card sector
  Lock.enableLockPicking(Lock.button2()); //Enable fast PIN entry if you push the 'lock picking' button
  Lock.begin(); //Start the lock with a default access ID
}

void loop() {
  Lock.housekeeping();  //This is all you need for basic functionality and interactive behaviour baked into the library
  #ifdef DEBUG_ENABLED
  /*
    if(millis() - lastStats > 5000)
    {
      lastStats = millis();
      SERIAL_DEBUG_PORT.print(F("Uptime: "));
      SERIAL_DEBUG_PORT.print(float(millis())/60000.0);
      SERIAL_DEBUG_PORT.println(F(" minutes"));
    }
  */
  #endif
}

bool usbConnected()
{
  //uint32_t DR_REG_USB_SERIAL_JTAG_BASE = 0x60043000;  //see https://github.com/search?q=DR_REG_USB_SERIAL_JTAG_BASE&type=code
  uint32_t USB_SERIAL_JTAG_FRAM_NUM_REG = (DR_REG_USB_SERIAL_JTAG_BASE + 0x24);
  uint32_t *aa = (uint32_t*)USB_SERIAL_JTAG_FRAM_NUM_REG;
  return (*aa) != 0;
}
