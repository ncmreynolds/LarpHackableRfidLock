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
//#include <SPI.h>
//#include <MFRC522.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
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
		void Housekeeping();													//Do housekeeping for event/state changes
		void RedLedOn(uint32_t on_time = 0);									//Switch on red LED. On time is 0 for permanent or otherwise in ms.
		void RedLedOff();														//Switch off red LED
		void GreenLedOn(uint32_t on_time = 0);									//Switch on green LED. On time is 0 for permanent or otherwise in ms.
		void GreenLedOff();														//Switch off green LED
		void BuzzerOn(uint16_t frequency = 440, uint32_t on_time = 100);		//Switch on buzzer, with a selected frequency and on time
		void BuzzerOff();														//Switch off buzzer
		//void open();															//Change to open state
		bool Reset();															//Has the lock been reset
		void Debug(Stream &);													//Enable debug output on a Serial stream
		//Higher level lock control
		enum class lockEvent : uint8_t {open, unlock, lock, sleep};
		void allow();															//Unlocks briefly to allow the door to open and people to pass
		void deny();															//Play the error noise
		void unlock();															//Unlock, allowing the door to open freely
		void lock();															//Lock, preventing the door from opening even with normally valid access
		void sleep(uint32_t);													//Deep sleep for an amount of time
		//RFID
		bool enableRFID();														//Enable RFID reading
		bool CardPresent();														//Is a card just present?
		bool CardChanged();														//Has it changed?
		bool CardAuthorised(uint8_t);											//Is this card authorised for this ID?
		bool WipeCard();														//Wipe the flags from a card, setting them all to zero
		bool AllowCard(uint8_t);												//Allow this card
		bool DenyCard(uint8_t);													//Deny this card
		uint8_t* CardUID();														//Retrieve a pointer to the current UID
		uint8_t CardUIDsize();													//Size of the current UID
		//Card database
		//bool initialiseCardDatabase(uint8_t size = 64);							//Initialises the card database
		//bool eraseCardDatabase();												//Erases the card database
		//bool loadCardDatabase();												//Loads the card database from storage
		//bool saveCardDatabase(bool force = false);								//Saves the card database to storage if changed (or forced)
		//bool cardRegistered(uint8_t* id);										//Returns true if the card is registered in the database
		//bool addCard(uint8_t* id);												//Add a card to the database
		//bool removeCard(uint8_t* id);											//Remove a card from the database
		//bool authoriseCard(uint8_t* id);										//Authorise a card for this lock
		//bool revokeCard(uint8_t* id);											//Revoke the authorisation for a card for this lock
		//Tap code
		void enableTapCode();													//Enables the use of tap code on the lock
		bool codeEntered();														//Has a code been entered?
		bool codeMatches(char*);												//Does the code match
		void clearEnteredCode();												//Reset the entered code
		bool setTapCode(char*, uint8_t);										//Set the tap code for an action
	protected:
	private:
		Stream *lock_uart = nullptr;											//The stream used for the terminal UART
		uint8_t accessId = 0;															//ID used for access
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
		MFRC522DriverPinSimple rfid_ss_pin_;
		MFRC522DriverSPI rfid_driver_;
		MFRC522 rfid_reader_;
		MFRC522::MIFARE_Key key_;
		bool rfid_antenna_enabled_ = true;										//Tracks state of the RFID antenna
		uint8_t current_uid_[10];												//UID of last presented card
		uint8_t current_uid_size_ = 4;											//UID size will be 4, 7 or 10
		bool card_present_ = false;												//Is card present
		bool card_changed_ = false;												//Has card changed
		uint8_t rfid_read_failures_ = 0;										//Count up before considering a card removed
		uint8_t rfid_read_failure_threshold_ = 2;								//Threshold to hit for card removal
		uint32_t rfid_reader_last_polled_ = 0;									//Timer for regular polling of RFID
		uint32_t rfid_reader_polling_interval_ = 100;							//Timer for regular polling of RFID
		uint8_t card_flags_[32];												//Take a copy of the flags on the card
		uint8_t flags_start_block_ = 4;											//Block to start flags from
		uint8_t flags_start_sector_ = 1;										//Sector to start flags from
		uint8_t trailerBlock_  = 7;

		bool PollForCard_();													//Poll to check if a card is there
		bool AuthenticateWithCard_();
		bool ReadCardFlags_();													//Read the flags from a card
		bool WriteCardFlags_();													//Write the flags to a card
		void DeAuthenticateWithCard_();											//Deauthenticate after a transaction is done
		//bool ReadBlock_(uint8_t, uint8_t, uint8_t*);							//Write a block
		//bool WriteBlock_(uint8_t, uint8_t, uint8_t*);							//Read a block
		//Reset detection
		DoubleResetDetector reset_detector;										//Double reset detector
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
