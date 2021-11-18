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
  : MIFARE_device(D8, D0),						//Calls constructor of class MFRC522, CS D8, RST D0
  reset_detector(reset_detector_timeout, reset_detector_address)	//Calls constructor of class DoubleResetDetector
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
	Serial.begin(lock_uart_baudrate_);
	Serial.print(F("\r\n\r\nLock software started on "));
	Serial.println(ARDUINO_BOARD);
	if(Reset() == true)
	{
		Serial.print(F("Lock is ready for configuration on SSID:"));
		Serial.print(default_ssid);
		Serial.print(F(" PSK:"));
		Serial.println(default_psk);
		tone(buzzer_pin_, musicalTones[0], 500);
	}
	else
	{
		Serial.print(F("Starting normally"));
	}
	SPI.begin();
	this->MIFARE_device.PCD_Init();
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::RedLedOn()	{
	if(red_led_state_ == false)	{
		digitalWrite(red_led_pin_,red_led_pin_on_value_);
		red_led_state_ = true;
		red_led_state_last_changed_ = millis();
		Serial.println(F("Lock red LED on"));
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::RedLedOff()	{
	if(red_led_state_ == true)	{
		digitalWrite(red_led_pin_,red_led_pin_off_value_);
		red_led_state_ = false;
		red_led_state_last_changed_ = millis();
		Serial.println(F("Lock red LED off"));
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::GreenLedOn()	{
	if(green_led_state_ == false)	{
		digitalWrite(green_led_pin_,green_led_pin_on_value_);
		green_led_state_ = true;
		green_led_state_last_changed_ = millis();
		Serial.println(F("Lock green LED on"));
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::GreenLedOff()	{
	if(green_led_state_ == true)	{
		digitalWrite(green_led_pin_,green_led_pin_off_value_);
		green_led_state_ = false;
		green_led_state_last_changed_ = millis();
		Serial.println(F("Lock green LED off"));
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::BuzzerOn(uint16_t frequency)	{
	if(buzzer_state_ == false)	{
		tone(buzzer_pin_, frequency, max_buzzer_on_time_);
		Serial.println(F("Lock buzzer on"));
		buzzer_state_last_changed_ = millis();
		buzzer_state_ = true;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::BuzzerOff()	{
	if(buzzer_state_ == true)	{
		noTone(buzzer_pin_);
		buzzer_state_ = false;
		buzzer_state_last_changed_ = millis();
		Serial.println(F("Lock buzzer off"));
	}
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::CardPresented() {
	if(this->MIFARE_device.PICC_IsNewCardPresent() == false) {
		return(false);
	}
	if(this->MIFARE_device.PICC_ReadCardSerial() == false) {
		return(false);
	}
	Serial.print(F("PICC type: "));
	MFRC522::PICC_Type piccType = this->MIFARE_device.PICC_GetType(this->MIFARE_device.uid.sak);
	Serial.print(this->MIFARE_device.PICC_GetTypeName(piccType));
	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
		Serial.println(F(" tag is not of type MIFARE Classic."));
		return(false);
	}
	else
	{
		Serial.println();
	}
    for (uint8_t i = 0; i < 4; i++) {
      current_nuid[i] = this->MIFARE_device.uid.uidByte[i];
    }
	Serial.printf("Card NUID:%02x:%02x:%02x:%02x\r\n",current_nuid[0],current_nuid[1],current_nuid[2],current_nuid[3]);
	return(true);
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
LarpHackableRfidLock Lock;
#endif
