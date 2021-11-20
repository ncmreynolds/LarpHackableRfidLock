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
#include <SPI.h>
#include <MFRC522.h>
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

class LarpHackableRfidLock	{

	public:
		LarpHackableRfidLock();													//Constructor function
		~LarpHackableRfidLock();												//Destructor function
		void begin();															//Start the lock and configure all the peripherals
		void Housekeeping();													//Do housekeeping for event/state changes
		void RedLedOn();														//Switch on red LED
		void RedLedOff();														//Switch off red LED
		void GreenLedOn();														//Switch on green LED
		void GreenLedOff();														//Switch off green LED
		void BuzzerOn(uint16_t);												//Switch on buzzer, with a selected frequency
		void BuzzerOff();														//Switch off buzzer
		bool CardPresented();													//Has a card just been presented
		bool Reset();															//Has the lock been reset
		void Debug(Stream &);													//Enable debug output on a Serial stream
		//Higher level lock control
		enum class lockEvent : uint8_t {open, unlock, lock, sleep};
		void open();															//Unlocks briefly to allow the door to open and people to pass
		void unlock();															//Unlock, allowing the door to open freely
		void lock();															//Lock, preventing the door from opening even with normally valid access
		void sleep(uint32_t);													//Deep sleep for an amount of time
		//Card database
		bool initialiseCardDatabase(uint8_t size = 64);							//Initialises the card database
		bool eraseCardDatabase();												//Erases the card database
		bool loadCardDatabase();												//Loads the card database from storage
		bool saveCardDatabase(bool force = false);								//Saves the card database to storage if changed (or forced)
		bool cardRegistered(uint8_t* id);										//Returns true if the card is registered in the database
		bool addCard(uint8_t* id);												//Add a card to the database
		bool removeCard(uint8_t* id);											//Remove a card from the database
		bool authoriseCard(uint8_t* id);										//Authorise a card for this lock
		bool revokeCard(uint8_t* id);											//Revoke the authorisation for a card for this lock
		//Tap code
		bool enableTapCode();													//Enables the use of tap code on the lock
		bool setTapCode(char*, uint8_t);										//Set the tap code for an action
	protected:
	private:
		Stream *lock_uart = nullptr;											//The stream used for the terminal UART
		uint8_t red_led_pin_ = D2;												//GPIO pin used for the red LED
		uint8_t red_led_pin_on_value_ = HIGH;									//GPIO pin used for the red LED
		uint8_t red_led_pin_off_value_ = LOW;									//GPIO pin used for the red LED
		uint8_t green_led_pin_ = D4;											//GPIO pin used for the green LED
		uint8_t green_led_pin_on_value_ = HIGH;									//GPIO pin used for the green LED
		uint8_t green_led_pin_off_value_ = LOW;									//GPIO pin used for the green LED
		uint8_t buzzer_pin_ = D1;												//GPIO pin used for the buzzer
		bool red_led_state_ = false;											//State of the red LED
		uint32_t red_led_state_last_changed_ = 0;								//Time when the state of the red LED last changed
		bool green_led_state_ = false;											//State of the green LED
		uint32_t green_led_state_last_changed_ = 0;								//Time when the state of the green LED last changed
		bool buzzer_state_ = false;												//State of the buzzer
		uint32_t buzzer_state_last_changed_ = 0;								//Time when the state of the buzzer last changed
		uint32_t max_buzzer_on_time_ = 60000;									//Maximum buzzer on time, to avoid total irritation
		//RFID related
		MFRC522 MIFARE_device;													//Instance of the MFRC522
		MFRC522::MIFARE_Key card_key;											//Instance of the MFRC522 MIFARE cardkey
		uint8_t current_nuid[4];												//NUID of last presented card
		bool card_present = false;												//Is card present
		//Reset detection
		DoubleResetDetector reset_detector;										//Double reset detector
		//Wi-Fi
		const char default_ssid[5] = "Lock";									//Default SSID when configuring lock
		const char default_psk[8] = "LetMeIn";									//Default PSK when configuring lock
		//Tones
		uint16_t musicalTones[8] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
		//Tap code
		TapCode* tapCode_ = nullptr;											//Pointer to the tap code class, if enabled
		char* tapCodes[4];														//Array of pointers to tap codes for each event
		
};

extern LarpHackableRfidLock Lock;
#endif
