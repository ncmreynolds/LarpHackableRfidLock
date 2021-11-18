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
#if defined(ESP8266)
  #define USE_LITTLEFS            true
  #define ESP_DRD_USE_LITTLEFS    true
#endif
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
	protected:
	private:
		Stream *lock_uart = nullptr;											//The stream used for the terminal UART
		uint8_t red_led_pin_ = D2;												//GPIO pin used for the red LED
		uint8_t red_led_pin_on_value_ = HIGH;									//GPIO pin used for the red LED
		uint8_t red_led_pin_off_value_ = LOW;									//GPIO pin used for the red LED
		uint8_t green_led_pin_ = D4;											//GPIO pin used for the green LED
		uint8_t green_led_pin_on_value_ = HIGH;									//GPIO pin used for the green LED
		uint8_t green_led_pin_off_value_ = LOW;									//GPIO pin used for the green LED
		uint8_t buzzer_pin_ = D3;												//GPIO pin used for the buzzer
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
		//Reset detection
		DoubleResetDetector reset_detector;										//Double reset detector
		int16_t reset_detector_timeout = 5;										//Time in seconds for reset detection
		int16_t reset_detector_address = 0;										//Address of reset detection flag
		//Wi-Fi
		const char default_ssid[5] = "Lock";									//Default SSID when configuring lock
		const char default_psk[8] = "LetMeIn";									//Default PSK when configuring lock
		//Tones
		uint16_t musicalTones[8] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
};

extern LarpHackableRfidLock Lock;
#endif
