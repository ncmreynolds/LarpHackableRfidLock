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


LarpHackableRfidLock::LarpHackableRfidLock()	:	//Constructor function and member constructors
	reset_detector_(DRD_TIMEOUT, DRD_ADDRESS)	//Calls constructor of class DoubleResetDetector
{
}

LarpHackableRfidLock::~LarpHackableRfidLock()	//Destructor function
{
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::begin(uint8_t id)	{
	accessId = id;
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
	if(doubleReset() == true)
	{
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("Lock is ready for configuration on SSID:"));
			lock_uart->print(default_ssid);
			lock_uart->print(F(" PSK:"));
			lock_uart->println(default_psk);
		}
		tone(buzzer_pin_, musicalTone[0], 500);
	}
	else if(lock_uart != nullptr)
	{
		lock_uart->println(F("Starting normally"));
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedOn(uint32_t on_time)	{
	if(red_led_state_ == false)	{
		digitalWrite(red_led_pin_,red_led_pin_on_value_);
		red_led_state_ = true;
		red_led_state_last_changed_ = millis();
		red_led_on_time_= on_time;
		if(lock_uart != nullptr)
		{
			if(on_time == 0) {
				lock_uart->println(F("Lock red LED on"));
			} else {
				lock_uart->printf("Lock red LED on for %ums\r\n", on_time);
			}
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedOff()	{
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
void ICACHE_FLASH_ATTR LarpHackableRfidLock::greenLedOn(uint32_t on_time)	{
	if(green_led_state_ == false)	{
		digitalWrite(green_led_pin_,green_led_pin_on_value_);
		green_led_state_ = true;
		green_led_state_last_changed_ = millis();
		green_led_on_time_= on_time;
		if(lock_uart != nullptr)
		{
			if(on_time == 0) {
				lock_uart->println(F("Lock green LED on"));
			} else {
				lock_uart->printf("Lock green LED on for %ums\r\n", on_time);
			}
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::greenLedOff()	{
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
void ICACHE_FLASH_ATTR LarpHackableRfidLock::buzzerOn(uint16_t frequency, uint32_t on_time)	{
	if(buzzer_state_ == false)	{
		buzzer_on_time_ = on_time;
		tone(buzzer_pin_, frequency, on_time);
		if(lock_uart != nullptr)
		{
			lock_uart->printf("Lock buzzer on %uhz for %ums\r\n",frequency,on_time);
		}
		buzzer_state_last_changed_ = millis();
		buzzer_state_ = true;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::buzzerOff()	{
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
void ICACHE_FLASH_ATTR LarpHackableRfidLock::allow()	{
	buzzerOn(musicalTone[4], 250);
	greenLedOn(5000);
	redLedOff();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::deny()	{
	buzzerOn(musicalTone[0], 50);
	redLedOn(50);
	greenLedOff();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::unlock()	{
	buzzerOn(musicalTone[4], 250);
	greenLedOn();
	redLedOff();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::lock()	{
	buzzerOn(musicalTone[0], 500);
	redLedOn();
	greenLedOff();
}
/*
 *	RFID, which mostly passes through to the library methods
 */
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableRFID(uint8_t pin) {
	
	rfid_ = new TrivialRFIDauthorisation(pin);	//Initialise tap code on pin 2
	if(lock_uart != nullptr)
	{
		rfid_->debug(Serial); //Enable debugging
	}
	rfid_->begin(rfid_authorisation_sector_);
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::authoriseCard(uint8_t id) {
	if(rfid_ != nullptr) {
		return(rfid_->authoriseCard(id));
	} else {
		return(false);
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::revokeCard(uint8_t id) {
	if(rfid_ != nullptr) {
		return(rfid_->revokeCard(id));
	} else {
		return(false);
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::wipeCard() {
	if(rfid_ != nullptr) {
		return(rfid_->revokeCard());
	} else {
		return(false);
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::cardPresent() {
	if(rfid_ != nullptr) {
		return(rfid_->cardPresent());
	} else {
		return(false);
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::cardChanged() {
	if(rfid_ != nullptr) {
		return(rfid_->cardChanged());
	} else {
		return(false);
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::cardAuthorised(uint8_t id) {
	if(rfid_ != nullptr) {
		return(rfid_->checkCardAuthorisation(id));
	} else {
		return(false);
	}		
}
/*
 *	Tap code, which mostly passes through to the library methods
 */
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableTapCode(uint8_t pin) {
	
	tapCode_ = new TapCode(pin);	//Initialise tap code on pin 2
	if(lock_uart != nullptr)
	{
		tapCode_->debug(Serial); //Enable debugging
	}
	tapCode_->begin();
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::codeEntered() {
	if(tapCode_ != nullptr)
	{
		return(tapCode_->finished());
	}
	return(false);
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::codeMatches(char* code) {
	if(tapCode_ != nullptr)
	{
		return(tapCode_->matches(code));
	}
	return(false);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::clearEnteredCode() {
	if(tapCode_ != nullptr)
	{
		tapCode_->reset();
	}
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::doubleReset() {
	if(this->reset_detector_.detectDoubleReset() == true)	{
		return(true);
	}
	return(false);
}

void  ICACHE_FLASH_ATTR LarpHackableRfidLock::housekeeping(){
	this->reset_detector_.loop();
	if(red_led_state_ == true && red_led_on_time_ > 0 && millis() - red_led_state_last_changed_ > red_led_on_time_)	//Switch off the red LED after red_led_on_time_
	{
		digitalWrite(red_led_pin_,red_led_pin_off_value_);
		red_led_state_last_changed_ = millis();
		red_led_on_time_ = 0;
		red_led_state_ = false;
	}
	if(green_led_state_ == true && green_led_on_time_ > 0 && millis() - green_led_state_last_changed_ > green_led_on_time_)	//Switch off the red LED after green_led_on_time_
	{
		digitalWrite(green_led_pin_,green_led_pin_off_value_);
		green_led_state_last_changed_ = millis();
		green_led_on_time_ = 0;
		green_led_state_ = false;
	}
	if(buzzer_state_ == true && millis() - buzzer_state_last_changed_ > buzzer_on_time_)	//Track how long the buzzer was on for
	{
		buzzer_state_last_changed_ = millis();
		buzzer_state_ = false;
	}
	if(rfid_ != nullptr)	//Only run the RFID reader code if enabled
	{
		rfid_->pollForCard();
	}
	if(tapCode_ != nullptr)	//Only run the tap code sections if enabled
	{
		tapCode_->read();
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::debug(Stream &terminalStream)
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
