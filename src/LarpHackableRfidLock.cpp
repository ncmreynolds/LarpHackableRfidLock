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

//Wrapper hack to allow the use of callbacks to the ESPUI library from the C++ class

LarpHackableRfidLock* LarpHackableRfidLockPointer = nullptr;	//A pointer to 'this' eventually

void ICACHE_FLASH_ATTR LarpHackableRfidLockHTTPCallbackWrapper(Control* sender, int type)
{
    if(LarpHackableRfidLockPointer != nullptr)
	{
        LarpHackableRfidLockPointer->HTTPCallback(sender, type);
	}
}

LarpHackableRfidLock::LarpHackableRfidLock()	:	//Constructor function and member constructors
	reset_detector_(DRD_TIMEOUT, DRD_ADDRESS)		//Calls constructor of class DoubleResetDetector
{
	LarpHackableRfidLockPointer = this;	//Set the pointer for the callback wrapper hack
}

LarpHackableRfidLock::~LarpHackableRfidLock()	//Destructor function
{
	LarpHackableRfidLockPointer = nullptr;	//Delete the pointer for the callback wrapper hack
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::begin(uint8_t id)	{
	accessId = id;
	pinMode(red_led_pin_,OUTPUT);
	digitalWrite(red_led_pin_,red_led_pin_off_value_);
	pinMode(green_led_pin_,OUTPUT);
	digitalWrite(green_led_pin_,green_led_pin_off_value_);
	pinMode(buzzer_pin_,OUTPUT);
	noTone(buzzer_pin_);
	if(debugStream_ != nullptr)
	{
		debugStream_->print(F("\r\n\r\nLock software started on "));
		debugStream_->println(ARDUINO_BOARD);
	}
	if(doubleReset() == true)
	{
		if(debugStream_ != nullptr)
		{
			debugStream_->println(F("Default Admin configuration loaded"));
		}
		tone(buzzer_pin_, musicalTone[0], 500);
	}
	else if(debugStream_ != nullptr)
	{
		debugStream_->println(F("Starting normally"));
	}
	if(LittleFS.begin()) {
		if(debugStream_ != nullptr) {
			debugStream_->println(F("LittleFS filesystem mounted"));
		}
		if(LittleFS.exists(configuration_file_)) {
			loadConfiguration(configuration_file_);
		} else {
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Configuration file not found, using defaults"));
			}
			loadDefaultConfiguration();
			saveConfiguration(configuration_file_);
		}
	} else {
		if(debugStream_ != nullptr) {
			debugStream_->println(F("LittleFS filesystem formatted"));
		}
	}
	//Lock name
	if(debugStream_ != nullptr)	{
		debugStream_->print(F("Lock name:"));
		debugStream_->println(lock_name_);
	}
	//Configure WiFi
	WiFi.persistent(false);										//Avoid flash wear
	if(wifi_client_enabled_ == true && wifi_client_ssid_ != nullptr && wifi_client_psk_!=nullptr && wifi_ap_enabled_ == true && wifi_ap_ssid_ != nullptr && wifi_ap_psk_!=nullptr) {
		WiFi.mode(WIFI_AP_STA);
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("WiFi mode:station & AP"));
		}
	} else if(wifi_client_enabled_ == true && wifi_client_ssid_ != nullptr && wifi_client_psk_!=nullptr) {
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("WiFi mode:station"));
		}
		WiFi.mode(WIFI_STA);
	} else if(wifi_ap_enabled_ == true && wifi_ap_ssid_ != nullptr && wifi_ap_psk_!=nullptr) {
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("WiFi mode:AP"));
		}
		WiFi.mode(WIFI_AP);
	} else {
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("WiFi mode:Disabled"));
			WiFi.mode(WIFI_OFF);
			WiFi.forceSleepBegin();
		}
	}
	if(lock_name_ != nullptr && (wifi_client_enabled_ == true || wifi_ap_enabled_ == true)) {
		#if defined(ESP32)
			WiFi.setHostname(lock_name_);
		#else
			WiFi.hostname(lock_name_);
		#endif
	}
	//WiFi client
	if(wifi_client_enabled_ == true && wifi_client_ssid_ != nullptr && wifi_client_psk_!=nullptr) {
		WiFi.setAutoReconnect(true);							//Because why not?
		WiFi.begin(wifi_client_ssid_, wifi_client_psk_);
		if(debugStream_ != nullptr)	{
			debugStream_->print(F("Trying to connect to SSID:\""));
			debugStream_->print(wifi_client_ssid_);
			debugStream_->print(F("\" PSK:\""));
			debugStream_->print(WiFi.psk());
			debugStream_->print("\"\nTimeout:");
			debugStream_->print((connectionRetries*connectionRetryFrequency)/1000);
			debugStream_->print('s');
		}
		uint8_t retries = connectionRetries;
		while((WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_IDLE_STATUS) && retries > 0)	{
			retries--;
			if(debugStream_ != nullptr)
			{
				debugStream_->print('.');
			}
			//debugStream_->print(WiFi.status());
			delay(connectionRetryFrequency);
		}
		if(debugStream_ != nullptr) {
			debugStream_->println();
			printConnectionStatus_();
			if(WiFi.status() == WL_CONNECTED)
			{
				printIpStatus_();
			}
		}
	}
	if(wifi_ap_enabled_ == true && wifi_ap_ssid_ != nullptr && wifi_ap_psk_!=nullptr) {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Lock Access Point starting, SSID:"));
			debugStream_->print(wifi_ap_ssid_);
			debugStream_->print(F(" PSK:"));
			debugStream_->print(wifi_ap_psk_);
		}
		if(WiFi.softAP(wifi_ap_ssid_, wifi_ap_psk_) == true) {
			if(debugStream_ != nullptr) {
				debugStream_->print(F(" - success AP IP:"));
				debugStream_->println(WiFi.softAPIP());
				debugStream_->print(F("Starting captive portal DNS server"));
			}
			captivePortalDnsServer = new DNSServer;
			if(captivePortalDnsServer->start(53, "*", WiFi.softAPIP()))
			{
				if(debugStream_ != nullptr) {
					debugStream_->println(F(" - success"));
				}
				dns_server_started_ = true;
			} else {
				if(debugStream_ != nullptr) {
					debugStream_->println(F(" - failed to start"));
				}
				dns_server_started_ = false;
			}
		} else {
			if(debugStream_ != nullptr) {
				debugStream_->println(F(" - failed to start"));
			}
		}	
	}
	if(wifi_client_enabled_ == true || wifi_ap_enabled_ == true)
	{
		//Tab 0
		tab_[0] = ESPUI.addControl(ControlType::Tab, "Door control", "Lock control");
		control_[0] = ESPUI.addControl(ControlType::Button, "Open", "Press", ControlColor::Peterriver, tab_[0], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[1] = ESPUI.addControl(ControlType::Button, "Unlock", "Press", ControlColor::Peterriver, tab_[0], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[2] = ESPUI.addControl(ControlType::Button, "Lock", "Press", ControlColor::Peterriver, tab_[0], LarpHackableRfidLockHTTPCallbackWrapper);
		//Tab 1
		tab_[1] = ESPUI.addControl(ControlType::Tab, "Settings 2", "Card Admin");
		//Tab 2
		tab_[2] = ESPUI.addControl(ControlType::Tab, "Settings 3", "Lock Admin");
		control_[3] = ESPUI.addControl(ControlType::Switcher, "WiFi client enabled", "", ControlColor::None, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[4] = ESPUI.addControl(ControlType::Text, "WiFi SSID:", "", ControlColor::Alizarin, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[5] = ESPUI.addControl(ControlType::Text, "WiFi PSK:", "", ControlColor::Alizarin, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[6] = ESPUI.addControl(ControlType::Switcher, "WiFi AP enabled", "", ControlColor::None, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[7] = ESPUI.addControl(ControlType::Text, "WiFi SSID:", "", ControlColor::Alizarin, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
		control_[8] = ESPUI.addControl(ControlType::Text, "WiFi PSK:", "", ControlColor::Alizarin, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
		ESPUI.begin("Lock");
		/*if(basic_auth_username_ != nullptr && basic_auth_password_ != nullptr) {
			ESPUI.begin("ESPUI Control", basic_auth_username_, basic_auth_password_, port);
		} else {
			ESPUI.begin("ESPUI Control", nullptr, nullptr, port);
		}*/
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::loadDefaultConfiguration() {
	//Access ID
	lock_access_id_ = lock_access_id_default_;
	//Name
	lock_name_ = new char[strlen(lock_name_default_)+1];
	strlcpy(lock_name_, lock_name_default_, strlen(lock_name_default_)+1);
	//WiFi client
	wifi_client_enabled_ = wifi_client_enabled_default_;
	//WiFi AP
	wifi_ap_enabled_ = wifi_ap_enabled_default_;
	wifi_ap_ssid_ = new char[strlen(wifi_ap_ssid_default_)+1];
	strlcpy(wifi_ap_ssid_, wifi_ap_ssid_default_, strlen(wifi_ap_ssid_default_)+1);
	wifi_ap_psk_ = new char[strlen(wifi_ap_psk_default_)+1];
	strlcpy(wifi_ap_psk_, wifi_ap_psk_default_, strlen(wifi_ap_psk_default_)+1);
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::loadConfiguration(const char* filename) {
	File configFile = LittleFS.open(filename, "r");
	if(configFile) {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Opened file "));
			debugStream_->println(filename);
		}
		StaticJsonDocument<512> doc;	//Make space for the document
		DeserializationError error = deserializeJson(doc, configFile);
		if(error == true) {
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Failed to read configuration file, using defaults"));
			}
			configFile.close();
			return(false);
		} else {
			//Lock name
			lock_name_ = new char[strlen(doc["hostname"] | lock_name_default_)+1];
			strlcpy(lock_name_, doc[lock_name_key_] | lock_name_default_, strlen(doc["hostname"] | lock_name_default_)+1);
			configFile.close();	//Close file
			return(true);
		}
	} else {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Unable to open file "));
			debugStream_->println(filename);
		}
		return(false);
	}
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::saveConfiguration(const char* filename) {
	return(true);
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
	if(WiFi.softAPgetStationNum() != current_number_of_clients_) {
		current_number_of_clients_ = WiFi.softAPgetStationNum();
		if(debugStream_ != nullptr)	{
			debugStream_->print(F("Number of WiFi clients:"));
			debugStream_->print(current_number_of_clients_);
			debugStream_->print('/');
			debugStream_->println(maximum_number_of_clients_);
		}
	}
	if(captivePortalDnsServer != nullptr && dns_server_started_ == true) {
		  captivePortalDnsServer->processNextRequest();
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::debug(Stream &terminalStream)
{
	debugStream_ = &terminalStream;		//Set the stream used for the terminal
	#if defined(ESP8266)
	if(&terminalStream == &Serial)
	{
		  debugStream_->write(17);		//Send an XON to stop the hung terminal after reset on ESP8266
	}
	#endif
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedOn(uint32_t on_time)	{
	if(red_led_state_ == false)	{
		digitalWrite(red_led_pin_,red_led_pin_on_value_);
		red_led_state_ = true;
		red_led_state_last_changed_ = millis();
		red_led_on_time_= on_time;
		if(debugStream_ != nullptr)
		{
			if(on_time == 0) {
				debugStream_->println(F("Lock red LED on"));
			} else {
				debugStream_->printf("Lock red LED on for %ums\r\n", on_time);
			}
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedOff()	{
	if(red_led_state_ == true)	{
		digitalWrite(red_led_pin_,red_led_pin_off_value_);
		red_led_state_ = false;
		red_led_state_last_changed_ = millis();
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("Lock red LED off"));
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::greenLedOn(uint32_t on_time)	{
	if(green_led_state_ == false)	{
		digitalWrite(green_led_pin_,green_led_pin_on_value_);
		green_led_state_ = true;
		green_led_state_last_changed_ = millis();
		green_led_on_time_= on_time;
		if(debugStream_ != nullptr)
		{
			if(on_time == 0) {
				debugStream_->println(F("Lock green LED on"));
			} else {
				debugStream_->printf("Lock green LED on for %ums\r\n", on_time);
			}
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::greenLedOff()	{
	if(green_led_state_ == true)	{
		digitalWrite(green_led_pin_,green_led_pin_off_value_);
		green_led_state_ = false;
		green_led_state_last_changed_ = millis();
		if(debugStream_ != nullptr)
		{
			debugStream_->println(F("Lock green LED off"));
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::buzzerOn(uint16_t frequency, uint32_t on_time)	{
	if(buzzer_state_ == false)	{
		buzzer_on_time_ = on_time;
		tone(buzzer_pin_, frequency, on_time);
		if(debugStream_ != nullptr)
		{
			debugStream_->printf("Lock buzzer on %uhz for %ums\r\n",frequency,on_time);
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
		if(debugStream_ != nullptr)
		{
			debugStream_->println(F("Lock buzzer off"));
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
	if(debugStream_ != nullptr)
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
	if(debugStream_ != nullptr)
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
	if(this->reset_detector_.detectDoubleReset() == true) {
		return(true);
	}
	return(false);
}

/*
 *	Wi-Fi admin interface
 */
 
/*void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableWiFiAdminInterface(uint8_t port) {
	http_admin_ = new HTTPadminInterface;
	if(debugStream_ != nullptr)
	{
		http_admin_->debug(Serial); //Enable debugging
	}
	http_admin_->begin(port);
}*/

void ICACHE_FLASH_ATTR LarpHackableRfidLock::printIpStatus_()
{
	debugStream_->print(F("IP address:"));
	debugStream_->print(WiFi.localIP());
	debugStream_->print(F(" Subnet mask:"));
	debugStream_->println(WiFi.subnetMask());
	debugStream_->print(F("Gateway:"));
	debugStream_->print(WiFi.gatewayIP());
	debugStream_->print(F(" DNS:"));
	debugStream_->print(WiFi.dnsIP());
	debugStream_->print('/');
	debugStream_->println(WiFi.dnsIP(1));
	debugStream_->print(F("Hostname:"));
	#if defined (ESP8266)
	debugStream_->println(WiFi.hostname());
	#elif defined (ESP32)
	debugStream_->println(WiFi.getHostname());
	#endif
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::printConnectionStatus_()
{
	switch (WiFi.status()) {
		case WL_IDLE_STATUS:
		debugStream_->println(F("Idle"));
		break;
		case WL_NO_SSID_AVAIL:
		debugStream_->println(F("SSID unavailable"));
		break;
		case WL_CONNECTED:
		debugStream_->print(F("Connected to SSID:\""));
		debugStream_->print(WiFi.SSID());
		debugStream_->print("\" BSSID:\"");
		debugStream_->print(WiFi.BSSIDstr());
		debugStream_->println('"');
		debugStream_->print(F("RSSI:"));
		debugStream_->print(WiFi.RSSI());
		debugStream_->print(F(" Channel:"));
		debugStream_->print(WiFi.channel());
		debugStream_->print(F(" Phy:"));
		#if defined (ESP8266)
		switch (WiFi.getPhyMode()){
			case WIFI_PHY_MODE_11B:
			debugStream_->println(F("802.11b"));
			break;
			case WIFI_PHY_MODE_11G:
			debugStream_->println(F("802.11g"));
			break;
			case WIFI_PHY_MODE_11N:
			debugStream_->println(F("802.11n"));
			break;
			default:
			debugStream_->println(F("Unknown"));
			break;
		}
		#elif defined (ESP32)
		wifi_ap_record_t *ap_info;
		if(esp_wifi_sta_get_ap_info(ap_info) == ESP_OK)
		{
			if(ap_info->phy_11b)
			{
				debugStream_->println(F("802.11b"));
			}
			if(ap_info->phy_11g)
			{
				debugStream_->println(F("802.11b"));
			}
			if(ap_info->phy_11n)
			{
				debugStream_->println(F("802.11b"));
			}
			if(ap_info->phy_lr)
			{
				debugStream_->println(F("802.11 lr"));
			}
		} else {
			debugStream_->println(F("Unknown"));
		}
		#endif
		break;
		case WL_CONNECT_FAILED:
		debugStream_->println(F("Connection failed"));
		break;
		#if defined(ESP8266)
		case WL_WRONG_PASSWORD:
		debugStream_->println(F("Wrong PSK"));
		break;
		#elif defined(ESP32)
		case WL_SCAN_COMPLETED:
		debugStream_->println(F("Scan completed"));
		break;
		case WL_CONNECTION_LOST:
		debugStream_->println(F("Connection lost"));
		break;
		#endif
		case WL_DISCONNECTED:
		debugStream_->println(F("Disconnected"));
		break;
		default:
		debugStream_->println(F("Unknown"));
		break;
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::HTTPCallback(Control* sender, int type) {
	if(sender->id == control_[0]) {
		if(type == B_DOWN) {
			control_state_[0] = true;
		} else {
			control_state_[0] = false;
		}
	}
	else {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Control ID: "));
			debugStream_->print(sender->id);
			debugStream_->print(' ');
			switch(sender->type) {
				case Title:
					debugStream_->print(F("Title: "));
				break;
				case Pad:
					debugStream_->print(F("Pad: "));
					switch ( type ) {
					case P_LEFT_DOWN:
						debugStream_->println( "LEFT DOWN" );
					break;
					case P_LEFT_UP:
						debugStream_->println(F("LEFT UP"));
					break;
					case P_RIGHT_DOWN:
						debugStream_->println(F("RIGHT DOWN"));
					break;
					case P_RIGHT_UP:
						debugStream_->println(F("RIGHT UP"));
					break;
					case P_FOR_DOWN:
						debugStream_->println(F("FORWARD DOWN"));
					break;
					case P_FOR_UP:
						debugStream_->println(F("FORWARD UP"));
					break;
					case P_BACK_DOWN:
						debugStream_->println(F("BACK DOWN"));
					break;
					case P_BACK_UP:
						debugStream_->println(F("BACK UP"));
					break;
					}
				break;
				case PadWithCenter:
					debugStream_->print(F("Pad with centre: "));
					switch ( type ) {
					case P_LEFT_DOWN:
						debugStream_->println( "LEFT DOWN" );
					break;
					case P_LEFT_UP:
						debugStream_->println(F("LEFT UP"));
					break;
					case P_RIGHT_DOWN:
						debugStream_->println(F("RIGHT DOWN"));
					break;
					case P_RIGHT_UP:
						debugStream_->println(F("RIGHT UP"));
					break;
					case P_FOR_DOWN:
						debugStream_->println(F("FORWARD DOWN"));
					break;
					case P_FOR_UP:
						debugStream_->println(F("FORWARD UP"));
					break;
					case P_BACK_DOWN:
						debugStream_->println(F("BACK DOWN"));
					break;
					case P_BACK_UP:
						debugStream_->println(F("BACK UP"));
					break;
					case P_CENTER_DOWN:
						debugStream_->println(F("CENTER DOWN"));
					break;
					case P_CENTER_UP:
						debugStream_->println(F("CENTER UP"));
					break;
					}
				break;
				case Button:
					debugStream_->print(F("Button: "));
					switch ( type ) {
					case B_DOWN:
						debugStream_->println(F("Button DOWN" ));
					break;
					case B_UP:
						debugStream_->println(F("Button UP" ));
					break;
					}
				break;
				case Label:
					debugStream_->print(F("Label "));
					debugStream_->print(", value: ");
					debugStream_->println( sender->value );
				break;
				case Switcher:
					debugStream_->print(F("Switch: "));
					switch ( type ) {
					case S_ACTIVE:
						if(debugStream_ != nullptr) {
						Serial.println( "ON" );
						}
					break;
					case S_INACTIVE:
						if(debugStream_ != nullptr) {
						Serial.println( "OFF" );
						}
					break;
					}
				break;
				case Slider:
					debugStream_->print(F("Slider: "));
					debugStream_->println( sender->value );
				break;
				case Number:
					debugStream_->print(F("Number: "));
					debugStream_->println( sender->value );
				break;
				case Text:
					debugStream_->print(F("Text: "));
					debugStream_->println( sender->value );
				break;
				case Graph:
					debugStream_->print(F("Graph: "));
					debugStream_->println( sender->value );
				break;
				case GraphPoint:
					debugStream_->print(F("Graph point: "));
					debugStream_->println( sender->value );
				break;
				case Tab:
					debugStream_->print(F("Tab: "));
					debugStream_->println( sender->value );
				break;
				case Select:
					debugStream_->print(F("Select: "));
					debugStream_->println( sender->value );
				break;
				case Option:
					debugStream_->print(F("Option: "));
					debugStream_->println( sender->value );
				break;
				case Min:
					debugStream_->print(F("Min: "));
					debugStream_->println( sender->value );
				break;
				case Max:
					debugStream_->print(F("Max: "));
					debugStream_->println( sender->value );
				break;
				case Step:
					debugStream_->print(F("Step: "));
					debugStream_->println( sender->value );
				break;
				case Gauge:
					debugStream_->print(F("Gauge: "));
					debugStream_->println( sender->value );
				break;
				case Accel:
					debugStream_->print(F("Accel: "));
					debugStream_->println( sender->value );
				break;
			}
		}
	}
}


bool ICACHE_FLASH_ATTR LarpHackableRfidLock::adminOpened() {
	return(control_state_[0]);
}

LarpHackableRfidLock Lock;
#endif
