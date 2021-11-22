/*
 *	Code, plans, diagrams etc. for a LARP prop 'hackable' RFID lock
 *
 *	https://github.com/ncmreynolds/LarpHackableRfidLock
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/LarpHackableRfidLock/LICENSE for full license
 *
 *
 */
#ifndef LarpHackableRfidLock_h
#define LarpHackableRfidLock_h
#include <Arduino.h>
//RFID reader
#include <TrivialRFIDauthorisation.h>
//Double reset detection
#ifdef ESP8266
	#define ESP8266_DRD_USE_RTC     false   //true
	#define ESP_DRD_USE_LITTLEFS    true    //false
#endif
#define ESP_DRD_USE_EEPROM      false
#define ESP_DRD_USE_SPIFFS      true
//#define DOUBLERESETDETECTOR_DEBUG       true  //false
#define DRD_TIMEOUT 3
#define DRD_ADDRESS 0
#include <ESP_DoubleResetDetector.h>
//Tap code
#include <TapCode.h>

class LarpHackableRfidLock	{

	public:
		LarpHackableRfidLock();													//Constructor function
		~LarpHackableRfidLock();												//Destructor function
		void begin(uint8_t id = 0);												//Start the lock and configure all the peripherals
		void housekeeping();													//Do housekeeping for event/state changes
		void redLedOn(uint32_t on_time = 0);									//Switch on red LED. On time is 0 for permanent or otherwise in ms.
		void redLedOff();														//Switch off red LED
		void greenLedOn(uint32_t on_time = 0);									//Switch on green LED. On time is 0 for permanent or otherwise in ms.
		void greenLedOff();														//Switch off green LED
		void buzzerOn(uint16_t frequency = 440, uint32_t on_time = 100);		//Switch on buzzer, with a selected frequency and on time
		void buzzerOff();														//Switch off buzzer
		//void open();															//Change to open state
		bool doubleReset();															//Has the lock been reset
		void debug(Stream &);													//Enable debug output on a Serial stream
		//Higher level lock control
		enum class lockEvent : uint8_t {open, unlock, lock, sleep};
		void allow();															//Unlocks briefly to allow the door to open and people to pass
		void deny();															//Play the error noise
		void unlock();															//Unlock, allowing the door to open freely
		void lock();															//Lock, preventing the door from opening even with normally valid access
		void sleep(uint32_t);													//Deep sleep for an amount of time
		//RFID
		void enableRFID(uint8_t pin);											//Enable RFID reading with SS on a pin (default sector 0)
		bool authoriseCard(uint8_t);											//Allow this card
		bool revokeCard(uint8_t);												//Deny this card
		bool wipeCard();														//Wipe the flags from a card, setting them all to zero
		bool cardPresent();														//Is a card just present?
		bool cardChanged();														//Has it changed?
		bool cardAuthorised(uint8_t);											//Is this card authorised for this ID?
		//Tap code
		void enableTapCode(uint8_t);											//Enables the use of tap code on the lock
		bool codeEntered();														//Has a code been entered?
		bool codeMatches(char*);												//Does the code match
		void clearEnteredCode();												//Reset the entered code
		bool setTapCode(char*, uint8_t);										//Set the tap code for an action
	protected:
	private:
		Stream *lock_uart = nullptr;											//The stream used for debugging
		uint8_t accessId = 0;													//ID used for access
		uint8_t red_led_pin_ = D2;												//GPIO pin used for the red LED
		uint8_t red_led_pin_on_value_ = HIGH;									//GPIO pin used for the red LED
		uint8_t red_led_pin_off_value_ = LOW;									//GPIO pin used for the red LED
		uint8_t green_led_pin_ = D4;											//GPIO pin used for the green LED
		uint8_t green_led_pin_on_value_ = HIGH;									//GPIO pin used for the green LED
		uint8_t green_led_pin_off_value_ = LOW;									//GPIO pin used for the green LED
		uint8_t buzzer_pin_ = D1;												//GPIO pin used for the buzzer
		bool red_led_state_ = false;											//State of the red LED
		uint32_t red_led_state_last_changed_ = 0;								//Time when the state of the red LED last changed
		uint32_t red_led_on_time_ = 0;
		bool green_led_state_ = false;											//State of the green LED
		uint32_t green_led_state_last_changed_ = 0;								//Time when the state of the green LED last changed
		uint32_t green_led_on_time_ = 0;
		bool buzzer_state_ = false;												//State of the buzzer
		uint32_t buzzer_state_last_changed_ = 0;								//Time when the state of the buzzer last changed
		uint32_t buzzer_on_time_ = 0;
		uint32_t max_buzzer_on_time_ = 60000;									//Maximum buzzer on time, to avoid total irritation
		//RFID related
		TrivialRFIDauthorisation* rfid_ = nullptr;								//Pointer to the RFID wrapper class
		uint8_t rfid_authorisation_sector_ = 0;									//Sector where the RFID authorisation block is stored
		//Reset detection
		DoubleResetDetector reset_detector_;									//Double reset detector
		//Wi-Fi
		const char default_ssid[5] = "Lock";									//Default SSID when configuring lock
		const char default_psk[8] = "LetMeIn";									//Default PSK when configuring lock
		//Tones
		uint16_t musicalTone[8] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
		//Tap code
		TapCode* tapCode_ = nullptr;											//Pointer to the tap code class, if enabled
		char* tapCodes[4];														//Array of pointers to tap codes for each event
		
};

extern LarpHackableRfidLock Lock;
#endif
