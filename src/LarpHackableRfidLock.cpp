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
	rfid_ss_pin_(D8),
	rfid_driver_{rfid_ss_pin_},
	rfid_reader_{rfid_driver_},
	reset_detector(DRD_TIMEOUT, DRD_ADDRESS)	//Calls constructor of class DoubleResetDetector
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
	if(Reset() == true)
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
	//Initialise RFID reader
	rfid_reader_.PCD_Init();
	if(lock_uart != nullptr)
	{
		lock_uart->println(F("Initialising RFID reader"));
		lock_uart->print(F("MFRC522 Self test: "));
		if (rfid_reader_.PCD_PerformSelfTest() == true)	{
			lock_uart->println(F("OK"));
		}
		else	{
			lock_uart->println(F("Fail"));
		}
	}
	for (uint8_t i = 0; i < MFRC522::MIFARE_Misc::MF_KEY_SIZE; i++) {
		key_.keyByte[i] = 0xFF;
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::RedLedOn(uint32_t on_time)	{
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
void ICACHE_FLASH_ATTR LarpHackableRfidLock::GreenLedOn(uint32_t on_time)	{
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
void ICACHE_FLASH_ATTR LarpHackableRfidLock::BuzzerOn(uint16_t frequency, uint32_t on_time)	{
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
void ICACHE_FLASH_ATTR LarpHackableRfidLock::allow()	{
	BuzzerOn(musicalTone[4], 250);
	GreenLedOn(5000);
	RedLedOff();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::deny()	{
	BuzzerOn(musicalTone[7], 500);
	RedLedOn(5000);
	GreenLedOff();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::unlock()	{
	BuzzerOn(musicalTone[4], 250);
	GreenLedOn();
	RedLedOff();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::lock()	{
	BuzzerOn(musicalTone[7], 500);
	RedLedOn();
	GreenLedOff();
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::enableRFID() {
	return(true);
}
uint8_t* ICACHE_FLASH_ATTR LarpHackableRfidLock::CardUID() {
	return(current_uid_);
}
uint8_t ICACHE_FLASH_ATTR LarpHackableRfidLock::CardUIDsize() {
	return(current_uid_size_);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::PollForCard_() {
	if(millis() - rfid_reader_last_polled_ > rfid_reader_polling_interval_)
	{
		rfid_reader_last_polled_ = millis();
		/*if(lock_uart != nullptr)
		{
			lock_uart->print(F("Polling card:"));
		}*/
		if(rfid_reader_.PICC_IsNewCardPresent() == false) {
			if(rfid_read_failures_++ >= rfid_read_failure_threshold_)
			{
				rfid_read_failures_ = rfid_read_failure_threshold_;
				if(card_present_ == true) {
					if(lock_uart != nullptr)
					{
						lock_uart->println(F("Card removed"));
					}
					for(uint8_t i = 0; i < current_uid_size_; i++) {
						current_uid_[i] = 0;
					}
					current_uid_size_ = 0;
					card_present_ = false;
				}
				return(false);
			}
			/*if(lock_uart != nullptr)
			{
				lock_uart->println(F("not present"));
			}*/
			return(false);
		}
		if(rfid_reader_.PICC_ReadCardSerial() == false) {
			if(rfid_read_failures_++ >= rfid_read_failure_threshold_)
			{
				rfid_read_failures_ = rfid_read_failure_threshold_;
				if(card_present_ == true) {
					if(lock_uart != nullptr)
					{
						lock_uart->println(F("Card removed"));
					}
					for(uint8_t i = 0; i < current_uid_size_; i++) {
						current_uid_[i] = 0;
					}
					current_uid_size_ = 0;
					card_present_ = false;
				}
				return(false);
			}
			/*if(lock_uart != nullptr)
			{
				lock_uart->println(F("can't read PICC"));
			}*/
			return(false);
		}
		/*if(lock_uart != nullptr)
		{
			lock_uart->println(F("present"));
		}*/
		card_present_ = true;
		card_changed_ = false;
		rfid_read_failures_ = 0;
		if(current_uid_size_ != rfid_reader_.uid.size)
		{
			current_uid_size_ = rfid_reader_.uid.size;
			card_changed_ = true;
		}
		for(uint8_t i = 0; i < rfid_reader_.uid.size; i++) {
			if(current_uid_[i] != rfid_reader_.uid.uidByte[i])
			{
				current_uid_[i] = rfid_reader_.uid.uidByte[i];
				card_changed_ = true;
			}
		}
		if(card_present_ == true && lock_uart != nullptr)	{
			if(card_changed_)	{
				lock_uart->print(F("New card presented "));
			} else {
				lock_uart->print(F("Card present "));
			}
			switch (current_uid_size_) {
			case 4:
				lock_uart->printf("UID:%02x:%02x:%02x:%02x\r\n",current_uid_[0],current_uid_[1],current_uid_[2],current_uid_[3]);
			break;
			case 7:
				lock_uart->printf("UID:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",current_uid_[0],current_uid_[1],current_uid_[2],current_uid_[3],current_uid_[4],current_uid_[5],current_uid_[6]);
			break;
			case 10:
				lock_uart->printf("UID:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",current_uid_[0],current_uid_[1],current_uid_[2],current_uid_[3],current_uid_[4],current_uid_[5],current_uid_[6],current_uid_[7],current_uid_[8],current_uid_[9]);
			break;
			default:
			break;
			}		
			
		}
		/*if(card_changed_)
		{
			ReadCardFlags_();
			WipeCard();
			for(uint8_t i = 0; i<32 ; i++)
			{
				card_flags_[i] = random(0,255);
			}
			WriteCardFlags_();
		}*/
		return(true);
	}
	return(false);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::WipeCard() {
	for(uint8_t i = 0; i < 32 ; i++)
	{
		card_flags_[0] = 0;
	}
	if(AuthenticateWithCard_() == false)
	{
		return(false);
	}
	if(WriteCardFlags_() == false)
	{
		return(false);
	}
	DeAuthenticateWithCard_();
	return(true);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::AllowCard(uint8_t id) {
	if(AuthenticateWithCard_() == false)
	{
		return(false);
	}
	if(ReadCardFlags_() == false)
	{
		return(false);
	}
	card_flags_[id>>8] = card_flags_[id>>8] | 0x01<<(id%8);	//Set the specific bit
	if(WriteCardFlags_() == false)
	{
		return(false);
	}
	DeAuthenticateWithCard_();
	return(true);
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::DenyCard(uint8_t id) {
	if(AuthenticateWithCard_() == false)
	{
		return(false);
	}
	if(ReadCardFlags_() == false)
	{
		return(false);
	}
	card_flags_[id>>8] = card_flags_[id>>8] ^ 0xFE<<(id%8);	//Unset the specific bit
	if(WriteCardFlags_() == false)
	{
		return(false);
	}
	DeAuthenticateWithCard_();
	return(true);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::CardAuthorised(uint8_t id) {
	if(AuthenticateWithCard_() == false)
	{
		return(false);
	}
	if(ReadCardFlags_() == false)
	{
		return(false);
	}
	DeAuthenticateWithCard_();
	if(card_flags_[id>>8] & 0x01<<(id%8))	//Check the specific bit
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::AuthenticateWithCard_() {
	MFRC522::StatusCode status = (MFRC522::StatusCode) rfid_reader_.PCD_Authenticate(MFRC522Constants::PICC_Command::PICC_CMD_MF_AUTH_KEY_A, trailerBlock_, &key_, &(rfid_reader_.uid));
	if (status != MFRC522::StatusCode::STATUS_OK) {
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("PCD_Authenticate() failed: "));
			lock_uart->println(MFRC522Debug::GetStatusCodeName(status));
		}
		return(false);
	}
	return(true);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::ReadCardFlags_() {
	if(lock_uart != nullptr) {
		lock_uart->println(F("Authenticating PICC using key A to read card data..."));
	}
	MFRC522::MIFARE_Key key_;
	for (uint8_t i = 0; i < MFRC522::MIFARE_Misc::MF_KEY_SIZE; i++) {
		key_.keyByte[i] = 0xFF;
	}
	uint8_t buffer[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Needs two bytes for the CRC
	uint8_t size = sizeof(buffer);
	if(lock_uart != nullptr)
	{
		MFRC522Debug::PICC_DumpMifareClassicSectorToSerial(rfid_reader_, Serial, &(rfid_reader_.uid), &key_, flags_start_sector_);
		lock_uart->println();
		lock_uart->printf("MIFARE_Read() sector %u block %u start: ",flags_start_sector_, flags_start_block_);
	}
	MFRC522::StatusCode status = (MFRC522::StatusCode) rfid_reader_.MIFARE_Read(flags_start_block_, buffer, &size);
	if (status != MFRC522::StatusCode::STATUS_OK) {
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("failed: "));
			lock_uart->println(MFRC522Debug::GetStatusCodeName(status));
		}
		rfid_reader_.PICC_HaltA();
		rfid_reader_.PCD_StopCrypto1();
		return(false);
	}
	if(lock_uart != nullptr)
	{
		lock_uart->print(F("success: "));
	}
	for (uint8_t i = 0; i < 16; i++) {
		card_flags_[i] = buffer[i];
		if(lock_uart != nullptr)
		{
			lock_uart->printf("%02x",buffer[i]);
			if(i<15)
			{
				lock_uart->print(':');
			}
			else
			{
				lock_uart->println();
			}
		}
	}
	if(lock_uart != nullptr)
	{
		lock_uart->printf("MIFARE_Read() sector %u block %u start: ",flags_start_sector_, flags_start_block_+1);
	}
	status = (MFRC522::StatusCode) rfid_reader_.MIFARE_Read(flags_start_block_+1, buffer, &size);
	if (status != MFRC522::StatusCode::STATUS_OK) {
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("failed: "));
			lock_uart->println(MFRC522Debug::GetStatusCodeName(status));
		}
		rfid_reader_.PICC_HaltA();
		rfid_reader_.PCD_StopCrypto1();
		return(false);
	}
	if(lock_uart != nullptr)
	{
		lock_uart->print(F("success: "));
	}
	for (uint8_t i = 0; i < 16; i++) {
		card_flags_[i + 16] = buffer[i];
		if(lock_uart != nullptr)
		{
			lock_uart->printf("%02x",buffer[i]);
			if(i<15)
			{
				lock_uart->print(':');
			}
			else
			{
				lock_uart->println();
			}
		}
	}
	return(true);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::WriteCardFlags_() {
	if(lock_uart != nullptr)
	{
		MFRC522Debug::PICC_DumpMifareClassicSectorToSerial(rfid_reader_, Serial, &(rfid_reader_.uid), &key_, flags_start_sector_);
		lock_uart->println();
		lock_uart->printf("MIFARE_Write() sector %u block %u start: ",flags_start_sector_, flags_start_block_);
	}
	MFRC522::StatusCode status = (MFRC522::StatusCode) rfid_reader_.MIFARE_Write(flags_start_block_, &card_flags_[0], 16);
	if (status != MFRC522::StatusCode::STATUS_OK) {
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("failed: "));
			lock_uart->println(MFRC522Debug::GetStatusCodeName(status));
		}
		rfid_reader_.PICC_HaltA();
		rfid_reader_.PCD_StopCrypto1();
		return(false);
	}
	if(lock_uart != nullptr)
	{
		lock_uart->print(F("success: "));
	}
	for (uint8_t i = 0; i < 16; i++) {
		if(lock_uart != nullptr)
		{
			lock_uart->printf("%02x",card_flags_[i]);
			if(i<15)
			{
				lock_uart->print(':');
			}
			else
			{
				lock_uart->println();
			}
		}
	}
	if(lock_uart != nullptr)
	{
		lock_uart->printf("MIFARE_Write() sector %u block %u start: ",flags_start_sector_, flags_start_block_+1);
	}
	status = (MFRC522::StatusCode) rfid_reader_.MIFARE_Write(flags_start_block_+1, &card_flags_[16], 16);
	if (status != MFRC522::StatusCode::STATUS_OK) {
		if(lock_uart != nullptr)
		{
			lock_uart->print(F("failed: "));
			lock_uart->println(MFRC522Debug::GetStatusCodeName(status));
		}
		rfid_reader_.PICC_HaltA();
		rfid_reader_.PCD_StopCrypto1();
		return(false);
	}
	if(lock_uart != nullptr)
	{
		lock_uart->print(F("success: "));
	}
	for (uint8_t i = 0; i < 16; i++) {
		if(lock_uart != nullptr)
		{
			lock_uart->printf("%02x",card_flags_[i + 16]);
			if(i<15)
			{
				lock_uart->print(':');
			}
			else
			{
				lock_uart->println();
			}
		}
	}
	MFRC522Debug::PICC_DumpMifareClassicSectorToSerial(rfid_reader_, Serial, &(rfid_reader_.uid), &key_, flags_start_sector_);
	return(true);
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::DeAuthenticateWithCard_()
{
	rfid_reader_.PICC_HaltA();
	rfid_reader_.PCD_StopCrypto1();
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::CardChanged() {
	if(card_changed_)
	{
		card_changed_ = false;
		return(true);
	}
	return(false);
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::CardPresent() {
	return(card_present_);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableTapCode() {
	
	tapCode_ = new TapCode(RX);	//Initialise tap code on pin 2
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
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::Reset() {
	if(this->reset_detector.detectDoubleReset() == true)	{
		return(true);
	}
	return(false);
}

void  ICACHE_FLASH_ATTR LarpHackableRfidLock::Housekeeping(){
	this->reset_detector.loop();
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
	PollForCard_();
	if(tapCode_ != nullptr)	//Only run the tap code sections if enabled
	{
		tapCode_->read();
	}
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
