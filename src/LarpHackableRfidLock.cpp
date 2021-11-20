/*
 *	Code, plans, diagrams etc. for a LARP prop 'hackable' RFID lock
 *
 *	https://github.com/ncmreynolds/LarpHackableRfidLock
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/LarpHackableRfidLock/LICENSE for full license
 *
 *
 */
#ifndef LarpHackableRfidLock_cpp
#define LarpHackableRfidLock_cpp
#include "LarpHackableRfidLock.h"


LarpHackableRfidLock::LarpHackableRfidLock()	//Constructor function
  : MIFARE_device(D8, D3),						//Calls constructor of class MFRC522, CS D8, RST D3
  reset_detector(DRD_TIMEOUT, DRD_ADDRESS)		//Calls constructor of class DoubleResetDetector
{
	//Use the default key from the factory: FFFFFFFFFFFF (hex) 
	for (int i = 0; i < 6; i ++) {
		this->card_key.keyByte[i] = 0xFF;
	}
}

LarpHackableRfidLock::~LarpHackableRfidLock()	//Destructor function
{
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::begin()	{
	pinMode(red_led_pin_,OUTPUT);
	digitalWrite(red_led_pin_,red_led_pin_off_value_);
	pinMode(green_led_pin_,OUTPUT);
	digitalWrite(green_led_pin_,green_led_pin_off_value_);
	pinMode(buzzer_pin_,OUTPUT);
	noTone(buzzer_pin_);
	if(lock_uart != nullptr)
	{
		lock_uart->print(F("\r\n\r\nLock software started on "));
		lock_uart->println(ARDUINO_BOARD);
	}
	if(Reset() == true)
	{
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("Lock is ready for configuration on SSID:"));
			lock_uart->print(default_ssid);
			lock_uart->print(F(" PSK:"));
			lock_uart->println(default_psk);
		}
		tone(buzzer_pin_, musicalTones[0], 500);
	}
	else if(lock_uart != nullptr)
	{
		lock_uart->println(F("Starting normally"));
	}
	SPI.begin();
	this->MIFARE_device.PCD_Init();
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::RedLedOn()	{
	if(red_led_state_ == false)	{
		digitalWrite(red_led_pin_,red_led_pin_on_value_);
		red_led_state_ = true;
		red_led_state_last_changed_ = millis();
		if(lock_uart != nullptr)
		{
			lock_uart->println(F("Lock red LED on"));
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::RedLedOff()	{
	if(red_led_state_ == true)	{
		digitalWrite(red_led_pin_,red_led_pin_off_value_);
		red_led_state_ = false;
		red_led_state_last_changed_ = millis();
		if(lock_uart != nullptr)
		{
			lock_uart->println(F("Lock red LED off"));
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::GreenLedOn()	{
	if(green_led_state_ == false)	{
		digitalWrite(green_led_pin_,green_led_pin_on_value_);
		green_led_state_ = true;
		green_led_state_last_changed_ = millis();
		if(lock_uart != nullptr)
		{
			lock_uart->println(F("Lock green LED on"));
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::GreenLedOff()	{
	if(green_led_state_ == true)	{
		digitalWrite(green_led_pin_,green_led_pin_off_value_);
		green_led_state_ = false;
		green_led_state_last_changed_ = millis();
		if(lock_uart != nullptr)
		{
			lock_uart->println(F("Lock green LED off"));
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::BuzzerOn(uint16_t frequency)	{
	if(buzzer_state_ == false)	{
		tone(buzzer_pin_, frequency, max_buzzer_on_time_);
		if(lock_uart != nullptr)
		{
			lock_uart->println(F("Lock buzzer on"));
		}
		buzzer_state_last_changed_ = millis();
		buzzer_state_ = true;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::BuzzerOff()	{
	if(buzzer_state_ == true)	{
		noTone(buzzer_pin_);
		buzzer_state_ = false;
		buzzer_state_last_changed_ = millis();
		if(lock_uart != nullptr)
		{
			lock_uart->println(F("Lock buzzer off"));
		}
	}
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::CardPresented() {
	if(this->MIFARE_device.PICC_IsNewCardPresent() == false) {
		return(false);
	}
	if(this->MIFARE_device.PICC_ReadCardSerial() == false) {
		if(card_present == true)
		{
			current_nuid[0] = 0;
			current_nuid[1] = 0;
			current_nuid[2] = 0;
			current_nuid[3] = 0;
			card_present = false;
			if(lock_uart != nullptr)
			{
				lock_uart->print(F("Card removed"));
			}
		}
		return(false);
	}
	MFRC522::PICC_Type piccType = this->MIFARE_device.PICC_GetType(this->MIFARE_device.uid.sak);
	if(lock_uart != nullptr && card_present == false)
	{
		lock_uart->print(F("PICC type: "));
		lock_uart->print(this->MIFARE_device.PICC_GetTypeName(piccType));
	}
	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
		if(lock_uart != nullptr)
		{
			lock_uart->println(F(" tag is not of type MIFARE Classic."));
		}
		card_present = false;
		return(false);
	}
	else if(lock_uart != nullptr && card_present == false)
	{
		lock_uart->println();
	}
	card_present = true;
	if(current_nuid[0] != MIFARE_device.uid.uidByte[0] ||
		current_nuid[1] != MIFARE_device.uid.uidByte[1] ||
		current_nuid[2] != MIFARE_device.uid.uidByte[2] ||
		current_nuid[3] != MIFARE_device.uid.uidByte[3]) {
		for (uint8_t i = 0; i < 4; i++) {
		  current_nuid[i] = this->MIFARE_device.uid.uidByte[i];
		}
		if(lock_uart != nullptr)
		{
			lock_uart->printf("New card NUID:%02x:%02x:%02x:%02x\r\n",current_nuid[0],current_nuid[1],current_nuid[2],current_nuid[3]);
		}
		return(true);
	}
	return(false);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::Reset() {
	if(this->reset_detector.detectDoubleReset() == true)	{
		return(true);
	}
	return(false);
}

void  ICACHE_FLASH_ATTR LarpHackableRfidLock::Housekeeping(){
	this->reset_detector.loop();
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::Debug(Stream &terminalStream)
{
	lock_uart = &terminalStream;		//Set the stream used for the terminal
	#if defined(ESP8266)
	if(&terminalStream == &Serial)
	{
		  lock_uart->write(17);		//Send an XON to stop the hung terminal after reset on ESP8266
	}
	#endif
}
LarpHackableRfidLock Lock;
#endif
