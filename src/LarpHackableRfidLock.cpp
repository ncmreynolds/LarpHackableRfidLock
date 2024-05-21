/*
 *	Code, plans, diagrams etc. for a LARP prop 'hackable' RFID lock
 *
 *	https://github.com/ncmreynolds/LarpHackableRfidLock
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/LarpHackableRfidLock/LICENSE for full license
 *
 *
 */
#include "LarpHackableRfidLock.h"

#ifdef HOUSEKEEPING_DEBUG
	uint8_t housekeepingdebug = 0;
#endif

#ifdef DRD
	LarpHackableRfidLock::LarpHackableRfidLock()	:	//Constructor function and member constructors
		reset_detector_(DRD_TIMEOUT, DRD_ADDRESS)	{	//Calls constructor of class DoubleResetDetector
	}
#else
	LarpHackableRfidLock::LarpHackableRfidLock(){
		
	}
#endif

LarpHackableRfidLock::~LarpHackableRfidLock()	{//Destructor function
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::begin(uint8_t id)	{
	lock_access_group_ = id;
	// Configure GPIO which can be a bit torturous with potential GPIO pin sharing
	if(debugStream_ != nullptr)	{
		debugStream_->print(F("\r\n\r\nLock software started on "));
		debugStream_->println(ARDUINO_BOARD);
		debugStream_->print(F("Restart reason: "));
		es32printResetReason(0);
	}
	if(LittleFS.begin()) {	//This will fail if not formatted, but ESP8266 will format it for you then fail
		if(debugStream_ != nullptr) {
			debugStream_->println(F("LittleFS filesystem mounted"));
		}
		if(LittleFS.exists(configuration_file_)) {
			if(loadConfiguration(configuration_file_)) {
				if(debugStream_ != nullptr) {
					debugStream_->println(F("Configuration file read OK"));
				}
			} else {
				if(debugStream_ != nullptr) {
					debugStream_->println(F("Loading default configuration"));
				}
				loadDefaultConfiguration();
				saveConfiguration(configuration_file_);
			}
		} else {
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Configuration file not found, using defaults"));
			}
			loadDefaultConfiguration();
			saveConfiguration(configuration_file_);
		}
	} else {
		#if defined(ESP32)
			LittleFS.begin(true);	//ESP32 needs to be told to format LittleFS
		#endif
		if(debugStream_ != nullptr) {
			debugStream_->println(F("LittleFS filesystem formatted"));
		}
		loadDefaultConfiguration();
		saveConfiguration(configuration_file_);
	}
	#ifdef DRD
		if(doubleReset() == true)	{
			wifi_ap_enabled_ = wifi_ap_enabled_default_;
			wifi_ap_ssid_ = wifi_ap_ssid_default_;
			wifi_ap_psk_ = wifi_ap_psk_default_;
			if(debugStream_ != nullptr)
			{
				debugStream_->println(F("Default AP configuration loaded for setup"));
			}
			if(buzzer_fitted_)
			{
				buzzerOn(musicalTone[0], 500);
			}
		}	else if(debugStream_ != nullptr)	{
			debugStream_->println(F("Starting normally"));
		}
	#endif
	if(debugStream_ != nullptr)	{
		displayCurrentConfiguration();
	}
	if(red_led_fitted_)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising red LED"));
		}
		//Only configure the red LED pin if it's NOT shared with a button
		if((button1_fitted_ == false || red_led_pin_ != button1_pin_) && (button2_fitted_ == false || red_led_pin_ != button2_pin_) && (button3_fitted_ == false || red_led_pin_ != button3_pin_))	{
			pinMode(red_led_pin_,OUTPUT);
			digitalWrite(red_led_pin_,red_led_pin_off_value_);
		}
	}
	if(green_led_fitted_)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising green LED"));
		}
		//Only configure the green LED pin if it's NOT shared with a button
		if((button1_fitted_ == false || green_led_pin_ != button1_pin_) && (button2_fitted_ == false || green_led_pin_ != button2_pin_) && (button3_fitted_ == false || green_led_pin_ != button3_pin_))	{
			pinMode(green_led_pin_,OUTPUT);
			digitalWrite(green_led_pin_,green_led_pin_off_value_);
		}
	}
	if(button1_fitted_ == true)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising button 1"));
		}
		if(button1_pin_ != tapCode_pin_)	{
			pinMode(button1_pin_,INPUT_PULLUP);
			button1_ = new EasyButton(button1_pin_);	//Initialise EasyButton
		}
	}
	if(button2_fitted_ == true)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising button 2"));
		}
		if(button2_pin_ != tapCode_pin_)	{
			pinMode(button2_pin_,INPUT_PULLUP);
			button2_ = new EasyButton(button2_pin_);	//Initialise EasyButton
		}
	}
	if(button3_fitted_ == true)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising button 3"));
		}
		if(button3_pin_ != tapCode_pin_)	{
			pinMode(button3_pin_,INPUT_PULLUP);
			button3_ = new EasyButton(button3_pin_);	//Initialise EasyButton
		}
	}
	if(servo_fitted_ == true)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising servo"));
		}
		lockServo_ = new Servo;	//Initialise servo
		//lockServo_->attach(servo_pin_, servo_channel_, 0, 180, 500, 2400);
		lockServo_->attach(servo_pin_, 500, 2400);
		lockServo_->write(servo_closed_postion_);
		servo_attached_ = true;
	}
	if(buzzer_fitted_ == true)	{
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Initialising buzzer"));
		}
		pinMode(buzzer_pin_,OUTPUT);
		#if defined ESP8266
			noTone(buzzer_pin_);
		#elif defined ESP32
			ledcSetup(buzzer_channel_, 440, 8);
			pinMode(buzzer_pin_,OUTPUT);
			digitalWrite(buzzer_pin_, buzzer_pin_off_value_);
		#endif
	}
	configureWifi();
	if(wifi_client_enabled_ == true || wifi_ap_enabled_ == true)	{
		//Start the web admin interface, if enabled
		#ifdef WEBUI_IN_LIBRARY
			if(debugStream_ != nullptr)	{
				debugStream_->print(F("Web admin interface: "));
			}
			if(web_admin_server_enabled_ == true)	{
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("enabled"));
				}
				if(game_enabled_ == false)	{
					webAdminServer = new EmbAJAXOutputDriverWebServerClass(80);
				} else {
					webAdminServer = new EmbAJAXOutputDriverWebServerClass(81);
					setupGame();
				}
				if(webAdminServer)	{
					webAdminInterface_ = new EmbAJAXOutputDriver(webAdminServer);
				}
				if(webAdminInterface_)	{
					startWebAdminInterface();
				}
			} else {
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("disabled"));
				}
			}
		#endif
	}
	if(debugStream_ != nullptr)	{
		debugStream_->print(F("Mesh network: "));
	}
	if(mesh_network_enabled_ == true)	{
		if(debugStream_ != nullptr)	{
			debugStream_->print(F("enabled (as \""));
		}
		if(m2mMesh.setNodeName(lock_name_))	{
			if(debugStream_ != nullptr)	{
				debugStream_->print(lock_name_);
				debugStream_->print("\") ");
			}
		} else	{
			if(debugStream_ != nullptr)	{
				debugStream_->print("<failed to set node name>\") ");
			}
		}
		mesh_network_started_ = m2mMesh.begin();
		if(mesh_network_started_ == true)	{
			if(debugStream_ != nullptr)	{
				debugStream_->print(F("started on channel "));
				debugStream_->println(m2mMesh.getChannel());
			}
		} else	{
			if(debugStream_ != nullptr)	{
				debugStream_->println(F("not started"));
			}
		}
	} else {
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("not enabled"));
		}
	}
	#ifdef WEBUI_IN_LIBRARY
		webAdminStatusPageUpdate();
	#endif
	//Flash the LEDs and sound the buzzer, if fitted
	if(red_led_fitted_) {
		redLedOn(1000);
	}
	if(green_led_fitted_) {
		greenLedOn(1000);
	}
	if(buzzer_fitted_) {
		buzzerOn(musicalTone[0], 500);
	}
}
uint8_t ICACHE_FLASH_ATTR LarpHackableRfidLock::accessId()	{	//Return the current access ID
	return lock_access_group_;
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::setAccessId(uint8_t id)	{	//Set the current access ID
	lock_access_group_ = id;
}
/*
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableWifi()	{
	
}
*/

void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableWiFiClient(char* SSID, char* PSK)	{
	bool changed = false;
	if(wifi_client_enabled_ == false || wifi_client_ssid_.equals(String(SSID)) == false || wifi_client_psk_.equals(String(PSK)) == false)	{
		wifi_client_enabled_ = true;
		wifi_client_ssid_ = String(SSID);
		wifi_client_psk_ = String(PSK);
		save_configuration_soon_ = millis();
		if(debugStream_ != nullptr)	{
			debugStream_->println(F("WiFi configuration changed"));
		}
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::configureWifi() {
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
			#if defined(ESP8266)
				WiFi.forceSleepBegin();
			#endif
		}
	}
	if(lock_name_.length() > 0 && (wifi_client_enabled_ == true || wifi_ap_enabled_ == true)) {
		#if defined(ESP32)
			WiFi.setHostname(lock_name_.c_str());
		#elif defined(ESP8266)
			WiFi.hostname(lock_name_);
		#endif
	}
	//WiFi client
	if(wifi_client_enabled_ == true && wifi_client_ssid_ != nullptr && wifi_client_psk_!=nullptr) {
		WiFi.setAutoReconnect(true);							//Because why not?
		#if defined(ESP8266)
			WiFi.begin(wifi_client_ssid_, wifi_client_psk_);
		#elif defined(ESP32)
			WiFi.begin(wifi_client_ssid_.c_str(), wifi_client_psk_.c_str());
		#endif
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
			delay(connectionRetryFrequency);
		}
		if(debugStream_ != nullptr) {
			debugStream_->println();
			printConnectionStatus_();
			if(WiFi.status() == WL_CONNECTED)
			{
				printIpStatus_();
				if(ntp_client_enabled_ == true && ntp_server_.length() > 0) {
					configureTimeServer();
				}
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
		#if defined(ESP8266)
			if(WiFi.softAP(wifi_ap_ssid_, wifi_ap_psk_, wifi_ap_channel_, wifi_ap_hidden_, wifi_ap_maximum_clients_) == true) {
		#elif defined(ESP32)
			if(WiFi.softAP(wifi_ap_ssid_.c_str(), wifi_ap_psk_.c_str(), wifi_ap_channel_, (wifi_ap_hidden_ == true ? 1 : 0), wifi_ap_maximum_clients_, false) == true) {
		#endif
			if(debugStream_ != nullptr) {
				debugStream_->print(F(" - success AP IP:"));
				debugStream_->println(WiFi.softAPIP());
			}
			if(wifi_ap_captive_portal_)	{
				if(debugStream_ != nullptr) {
					debugStream_->print(F("Starting captive portal DNS server"));
				}
				captivePortalDnsServer = new DNSServer;
				if(captivePortalDnsServer->start(53, "*", WiFi.softAPIP()))
				{
					if(debugStream_ != nullptr) {
						debugStream_->println(F(" - success"));
					}
					dns_server_started_ = true;
					//captivePortalDnsServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
				} else {
					if(debugStream_ != nullptr) {
						debugStream_->println(F(" - failed to start"));
					}
					dns_server_started_ = false;
				}
			}
		} else {
			if(debugStream_ != nullptr) {
				debugStream_->println(F(" - failed to start"));
			}
		}	
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::configureTimeServer() {
	configTime(0, 0, ntp_server_.c_str());	//Set server
	if(ntp_timezone_.length() > 0) {
		setenv("TZ",ntp_timezone_.c_str(),1);	//Set timezone
		tzset();						//Apply timezone
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::loadDefaultConfiguration() {
	//Access ID
	lock_access_group_ = lock_access_group_default_;
	//Name
	lock_name_ = lock_name_default_;
	//Tap Code
	tap_code_enabled_ = tap_code_enabled_default_;
	tap_code_positive_feedback_ = tap_code_positive_feedback_default_;
	tap_code_negative_feedback_ = tap_code_negative_feedback_default_;
	tap_code_open_ = tap_code_open_default_;
	tap_code_unseal_ = tap_code_unseal_default_;
	tap_code_seal_ = tap_code_seal_default_;
	tap_code_authorise_ = tap_code_authorise_default_;
	tap_code_revoke_ = tap_code_revoke_default_;
	tap_code_revoke_all_ = tap_code_revoke_all_default_;
	//Mesh Network
	mesh_network_enabled_ = mesh_network_enabled_default_;
	mesh_network_channel_ = mesh_network_channel_default_;
	mesh_network_id_ = mesh_network_id_default_;
	mesh_network_size_ = mesh_network_size_default_;
	mesh_network_autosize_ = mesh_network_autosize_default_;
	//WiFi client
	wifi_client_enabled_ = wifi_client_enabled_default_;
	wifi_client_ssid_ = wifi_client_ssid_default_;
	wifi_client_psk_ = wifi_client_psk_default_;
	ntp_client_enabled_ = ntp_client_enabled_default;
	ntp_server_ = ntp_server_default_;
	ntp_timezone_ = ntp_timezone_default_;
	//WiFi AP
	wifi_ap_enabled_ = wifi_ap_enabled_default_;
	wifi_ap_hidden_ = wifi_ap_hidden_default_;
	wifi_ap_ssid_ = wifi_ap_ssid_default_;
	wifi_ap_psk_ = wifi_ap_psk_default_;
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::loadConfiguration(const char* filename) {
	//return true;
	File configFile = LittleFS.open(filename, "r");
	if(configFile) {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Opened file "));
			debugStream_->print(filename);
			debugStream_->print(F(" / "));
			debugStream_->print(configFile.size());
			debugStream_->println(F(" bytes"));
		}
		StaticJsonDocument<configDocumentSize> doc;	//Make space for the document
		DeserializationError error = deserializeJson(doc, configFile);
		if(error != DeserializationError::Ok) {
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Failed to parse configuration file: "));
				debugStream_->println(error.c_str());
			}
			configFile.close();
			return false;
		} else {
			//Lock name
			lock_name_ = doc[lock_name_key_] | lock_name_default_;
			//Lock ID
			lock_access_group_ = doc[lock_access_group_key_] | 0;
			//Tap Code
			tap_code_enabled_ = doc[tap_code_enabled_key_] | tap_code_enabled_default_;
			tap_code_positive_feedback_ = doc[tap_code_positive_feedback_key_] | tap_code_positive_feedback_default_;
			tap_code_negative_feedback_ = doc[tap_code_negative_feedback_key_] | tap_code_negative_feedback_default_;
			//Tap Codes
			tap_code_open_ = doc[tap_code_open_key_] | tap_code_open_default_;
			tap_code_unseal_ = doc[tap_code_unseal_key_] | tap_code_unseal_default_;
			tap_code_seal_ = doc[tap_code_seal_key_] | tap_code_seal_default_;
			tap_code_authorise_ = doc[tap_code_authorise_key_] | tap_code_authorise_default_;
			tap_code_revoke_ = doc[tap_code_revoke_key_] | tap_code_revoke_default_;
			tap_code_revoke_all_ = doc[tap_code_revoke_all_key_] | tap_code_revoke_all_default_;
			//PIN codes
			pin_entry_enabled_ == doc[pin_entry_enabled_key_];
			pin_to_open_ = doc[pin_to_open_key_] | pin_to_open_default_;
			pin_to_seal_ = doc[pin_to_seal_key_] | pin_to_seal_default_;
			pin_to_unseal_ = doc[pin_to_unseal_key_] | pin_to_unseal_default_;
			pin_to_allow_next_card_ = doc[pin_to_allow_next_card_key_] | pin_to_allow_next_card_default_;
			pin_to_deny_next_card_ = doc[pin_to_deny_next_card_key_] | pin_to_deny_next_card_default_;
			pin_to_wipe_next_card_ = doc[pin_to_wipe_next_card_key_] | pin_to_wipe_next_card_default_;
			//RFID
			rfid_authorisation_enabled_ = doc[rfid_authorisation_enabled_key_] | rfid_authorisation_enabled_default_;
			//Mesh network
			mesh_network_enabled_ = doc[mesh_network_enabled_key_] | mesh_network_enabled_default_;
			mesh_network_channel_ = doc[mesh_network_channel_key_] | mesh_network_channel_default_;
			mesh_network_id_ = doc[mesh_network_id_key_] | mesh_network_id_default_;
			mesh_network_size_ = doc[mesh_network_size_key_] | mesh_network_size_default_;
			mesh_network_autosize_ = doc[mesh_network_autosize_key_] | mesh_network_autosize_default_;
			//WiFi client
			wifi_client_enabled_ = doc[wifi_client_enabled_key_] | wifi_client_enabled_default_;
			wifi_client_ssid_ = doc[wifi_client_ssid_key_].as<String>();
			wifi_client_psk_ = doc[wifi_client_psk_key_].as<String>();
			//NTP
			ntp_client_enabled_ = doc[ntp_client_enabled_key_] | ntp_client_enabled_default;
			ntp_server_ = String(doc[ntp_server_key_] | ntp_server_default_);
			ntp_timezone_ = String(doc[ntp_timezone_key_] | ntp_timezone_default_);
			//WiFi AP
			wifi_ap_enabled_ = doc[wifi_ap_enabled_key_] | wifi_ap_enabled_default_;
			wifi_ap_captive_portal_ = doc[wifi_ap_captive_portal_key_] | wifi_ap_captive_portal_default_;
			wifi_ap_hidden_ = doc[wifi_ap_hidden_key_] | wifi_ap_hidden_default_;
			wifi_ap_ssid_ = String(doc[wifi_ap_ssid_key_] | wifi_ap_ssid_default_);
			wifi_ap_psk_ = String(doc[wifi_ap_psk_key_] | wifi_ap_psk_default_);
			//MQTT
			mqtt_enabled_ = doc[mqtt_enabled_key_] | mqtt_enabled_default_;
			mqtt_host_ = doc[mqtt_host_key_].as<String>();
			mqtt_port_ = doc[mqtt_port_key_].as<String>();
			mqtt_client_id_ = doc[mqtt_client_id_key_].as<String>();
			mqtt_username_ = doc[mqtt_username_key_].as<String>();
			mqtt_password_ = doc[mqtt_password_key_].as<String>();
			mqtt_topic_ = doc[mqtt_topic_key_].as<String>();
			//Hacking game
			game_enabled_ = doc[game_enabled_key_] | game_enabled_default_;
			game_type_ =  doc[game_type_key_] | game_type_default_;
			game_length_ = doc[game_length_key_] = game_length_default_;
			game_retries_ = doc[game_retries_key_] = game_retries_default_;
			//Multi-factor authentication
			multi_factor_enabled_ = doc[multi_factor_enabled_key_] | multi_factor_enabled_default_;
			uint8_t multifactortype =  doc[multi_factor_type_key_] | 0;
			multi_factor_type_ =  (multiFactorOption)multifactortype;
			multi_factor_partner_name_ = doc[multi_factor_partner_name_key_].as<String>();
			uint8_t multifactorpartnertype = doc[multi_factor_partner_type_key_] | 0;
			multi_factor_partner_type_ =  (multiFactorPartnerOption)multifactorpartnertype;
			multi_factor_timeout_ =  doc[multi_factor_timeout_key_] | multi_factor_timeout_default_;
			//Close the file
			configFile.close();	//Close file
			return true;
		}
	} else {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Unable to open file "));
			debugStream_->println(filename);
		}
		return false;
	}
	return false;
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::displayCurrentConfiguration() {
	//Lock
	debugStream_->print(F("Lock name:"));
	if(lock_name_.length() > 0) {
		debugStream_->println(lock_name_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Access ID:"));
	debugStream_->println(lock_access_group_);
	//Tap Code
	debugStream_->print(F("Tap Code enabled:"));
	debugStream_->println(tap_code_enabled_);
	debugStream_->print(F("Tap Code positive feedback enabled:"));
	debugStream_->println(tap_code_positive_feedback_);
	debugStream_->print(F("Tap Code negative feedback enabled:"));
	debugStream_->println(tap_code_negative_feedback_);
	debugStream_->print(F("Open Tap Code:"));
	if(tap_code_open_.length() > 0) {
		debugStream_->println(tap_code_open_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Unlock Tap Code:"));
	if(tap_code_unseal_.length() > 0) {
		debugStream_->println(tap_code_unseal_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Seal Tap Code:"));
	if(tap_code_seal_.length() > 0) {
		debugStream_->println(tap_code_seal_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Authorise Tap Code:"));
	if(tap_code_authorise_.length() > 0) {
		debugStream_->println(tap_code_authorise_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Revoke Tap Code:"));
	if(tap_code_revoke_.length() > 0) {
		debugStream_->println(tap_code_revoke_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Revoke All Tap Code:"));
	if(tap_code_revoke_all_.length() > 0) {
		debugStream_->println(tap_code_revoke_all_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	//PINs
	debugStream_->print(F("PIN enabled:"));
	debugStream_->println(pin_entry_enabled_);
	debugStream_->print(F("Open PIN Code:"));
	if(pin_to_open_.length() > 0) {
		debugStream_->println(pin_to_open_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Unseal PIN Code:"));
	if(pin_to_unseal_.length() > 0) {
		debugStream_->println(pin_to_unseal_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Seal PIN Code:"));
	if(pin_to_seal_.length() > 0) {
		debugStream_->println(pin_to_seal_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Authorise PIN Code:"));
	if(pin_to_allow_next_card_.length() > 0) {
		debugStream_->println(pin_to_allow_next_card_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Revoke PIN Code:"));
	if(pin_to_deny_next_card_.length() > 0) {
		debugStream_->println(pin_to_deny_next_card_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Revoke All PIN Code:"));
	if(pin_to_wipe_next_card_.length() > 0) {
		debugStream_->println(pin_to_wipe_next_card_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	//RFID
	debugStream_->print(F("RFID authorisation enabled:"));
	debugStream_->println(rfid_authorisation_enabled_);
	//Mesh network
	debugStream_->print(F("Mesh network enabled:"));
	debugStream_->println(mesh_network_enabled_);
	debugStream_->print(F("Mesh network channel:"));
	debugStream_->println(mesh_network_channel_);
	debugStream_->print(F("Mesh network ID:"));
	debugStream_->println(mesh_network_id_);
	debugStream_->print(F("Mesh network starting size:"));
	debugStream_->println(mesh_network_size_);
	debugStream_->print(F("Mesh network starting auto size:"));
	debugStream_->println(mesh_network_autosize_);
	//Wifi Client
	debugStream_->print(F("WiFi client enabled:"));
	debugStream_->println(wifi_client_enabled_);
	debugStream_->print(F("WiFi client SSID:"));
	if(wifi_client_ssid_.length() > 0) {
		debugStream_->println(wifi_client_ssid_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("WiFi client PSK:"));
	if(wifi_client_psk_.length() > 0) {
		debugStream_->println(wifi_client_psk_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	//NTP
	debugStream_->print(F("NTP client enabled:"));
	debugStream_->println(ntp_client_enabled_);
	debugStream_->print(F("NTP server:"));
	if(ntp_server_.length() > 0) {
		debugStream_->println(ntp_server_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("Timezone:"));
	if(ntp_timezone_.length() > 0) {
		debugStream_->println(ntp_timezone_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	//WiFi AP
	debugStream_->print(F("WiFi AP enabled:"));
	debugStream_->println(wifi_ap_enabled_);
	debugStream_->print(F("WiFi AP captive portal enabled:"));
	debugStream_->println(wifi_ap_captive_portal_);
	debugStream_->print(F("WiFi AP hidden:"));
	debugStream_->println(wifi_ap_hidden_);
	debugStream_->print(F("WiFi AP SSID:"));
	if(wifi_ap_ssid_.length() > 0) {
		debugStream_->println(wifi_ap_ssid_);
	} else {
		debugStream_->println(F("<not set>"));
	}
	debugStream_->print(F("WiFi AP PSK:"));
	if(wifi_ap_psk_.length() > 0) {
		debugStream_->println(wifi_ap_psk_);
	} else {
		debugStream_->println(F("<not set>"));
	}
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::saveConfiguration() {
	return(saveConfiguration(configuration_file_));
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::saveConfiguration(const char* filename) {
	File configFile = LittleFS.open(filename, "w");
	if(configFile) {
		StaticJsonDocument<configDocumentSize> doc;
		//Lock setting
		doc[lock_name_key_] = lock_name_;
		doc[lock_access_group_key_] = lock_access_group_;
		//Tap Code
		doc[tap_code_enabled_key_] = tap_code_enabled_;
		doc[tap_code_positive_feedback_key_] = tap_code_positive_feedback_;
		doc[tap_code_negative_feedback_key_] = tap_code_negative_feedback_;
		//Tap Codes
		doc[tap_code_open_key_] = tap_code_open_;
		doc[tap_code_unseal_key_] = tap_code_unseal_;
		doc[tap_code_seal_key_] = tap_code_seal_;
		doc[tap_code_authorise_key_] = tap_code_authorise_;
		doc[tap_code_revoke_key_] = tap_code_revoke_;
		doc[tap_code_revoke_all_key_] = tap_code_revoke_all_;
		//PIN codes
		//doc[pin_entry_enabled_key_] = pin_entry_enabled_;
		doc[pin_to_open_key_] = pin_to_open_;
		doc[pin_to_seal_key_] = pin_to_seal_;
		doc[pin_to_unseal_key_] = pin_to_unseal_;
		doc[pin_to_allow_next_card_key_] = pin_to_allow_next_card_;
		doc[pin_to_deny_next_card_key_] = pin_to_deny_next_card_;
		doc[pin_to_wipe_next_card_key_] = pin_to_wipe_next_card_;
		//RFID
		doc[rfid_authorisation_enabled_key_] = rfid_authorisation_enabled_;
		//WiFi Client
		doc[wifi_client_enabled_key_] = wifi_client_enabled_;
		doc[wifi_client_ssid_key_] = wifi_client_ssid_;
		doc[wifi_client_psk_key_] = wifi_client_psk_;
		//NTP
		doc[ntp_client_enabled_key_] = ntp_client_enabled_;
		doc[ntp_server_key_] = ntp_server_;
		doc[ntp_timezone_key_] = ntp_timezone_;
		//WiFi AP
		doc[wifi_ap_enabled_key_] = wifi_ap_enabled_;
		doc[wifi_ap_captive_portal_key_] = wifi_ap_captive_portal_;
		doc[wifi_ap_hidden_key_] = wifi_ap_hidden_;
		doc[wifi_ap_ssid_key_] = wifi_ap_ssid_;
		doc[wifi_ap_psk_key_] = wifi_ap_psk_;
		//Mesh network
		doc[mesh_network_enabled_key_] = mesh_network_enabled_;
		doc[mesh_network_channel_key_] = mesh_network_channel_;
		doc[mesh_network_id_key_] = mesh_network_id_;
		doc[mesh_network_size_key_] = mesh_network_size_;
		doc[mesh_network_autosize_key_] = mesh_network_autosize_;
		//MQTT
		doc[mqtt_enabled_key_] = mqtt_enabled_;
		doc[mqtt_host_key_] = mqtt_host_;
		doc[mqtt_port_key_] = mqtt_port_;
		doc[mqtt_client_id_key_] = mqtt_client_id_;
		doc[mqtt_username_key_] = mqtt_username_;
		doc[mqtt_password_key_] = mqtt_password_;
		doc[mqtt_topic_key_] = mqtt_topic_;
		//Hacking game
		doc[game_enabled_key_] = game_enabled_;
		doc[game_type_key_] = game_type_;
		doc[game_length_key_] = game_length_;
		doc[game_retries_key_] = game_retries_;
		//Multi-factor authentication
		doc[multi_factor_enabled_key_] = multi_factor_enabled_;
		doc[multi_factor_type_key_] = (int)multi_factor_type_;
		doc[multi_factor_partner_name_key_] = multi_factor_partner_name_;
		doc[multi_factor_partner_type_key_] = (int)multi_factor_partner_type_;
		doc[multi_factor_timeout_key_] = multi_factor_timeout_;
		//if (serializeJson(doc, configFile) == 0) {
		if (serializeJsonPretty(doc, configFile) == 0) {	//Improves readability in the UI/when downloaded
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Unable to write file "));
				debugStream_->println(filename);
			}
			configFile.close();
			return false;
		} else {
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Configuration saved to file "));
				debugStream_->println(filename);
			}
			configFile.close();
			return true;
		}
	} else {
		if(debugStream_ != nullptr) {
			debugStream_->print(F("Unable to write file "));
			debugStream_->println(filename);
		}
		return false;
	}
}


void  ICACHE_FLASH_ATTR LarpHackableRfidLock::housekeeping(){
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//0
	#endif
	#ifdef DRD
		this->reset_detector_.loop();
	#endif
	if(restart_soon_ != 0 && millis() - restart_soon_ > 5000)	{	//Restart from webUI or other interface
		ESP.restart();
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//1
	#endif
	if(save_configuration_soon_ != 0 && millis() - save_configuration_soon_ > 500)	{	//Save config from webUI or other interface
		if(saveConfiguration())	{
			#ifdef WEBUI_IN_LIBRARY
				web_admin_m2mMesh_settings_page_save_button_->setVisible(false);
				web_admin_wifi_client_settings_page_save_button_->setVisible(false);
				web_admin_wifi_ap_settings_page_save_button_->setVisible(false);
				web_admin_wifi_ap_settings_page_save_button_->setVisible(false);
				web_admin_pin_settings_page_save_button_->setVisible(false);
				web_admin_tap_code_settings_page_save_button_->setVisible(false);
				web_admin_rfid_settings_page_save_button_->setVisible(false);
				web_admin_mqtt_settings_page_save_button_->setVisible(false);
				web_admin_game_settings_page_save_button_->setVisible(false);
				web_admin_multi_factor_settings_page_save_button_->setVisible(false);
				webAdminStatusPageUpdate();
			#endif
			Lock.configuration_changed_ = false;
			save_configuration_soon_ = 0;
		}
	}
	#ifdef WEBUI_IN_LIBRARY
		#ifdef HOUSEKEEPING_DEBUG
			Serial.println(housekeepingdebug++);	
		#endif
	webAdminInterface_->loopHook();	//Manage the WebUI
	#endif
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//2
	#endif
	if(mesh_network_enabled_ == true && mesh_network_started_ == true)	{
		m2mMesh.housekeeping();
		if(mesh_network_joined_ == false && m2mMesh.joined() == true)	{
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Joined mesh network"));
			}
			mesh_network_joined_ = true;
			#ifdef WEBUI_IN_LIBRARY
			webAdminStatusPageUpdate();
			#endif
		}
		if(mesh_network_joined_ == true && m2mMesh.joined() == false)	{
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Left mesh network"));
			}
			mesh_network_joined_ = false;
			#ifdef WEBUI_IN_LIBRARY
			webAdminStatusPageUpdate();
			#endif
		}
		if(mesh_network_joined_ == true && multi_factor_enabled_ == true && multi_factor_partner_type_ == multiFactorPartnerOption::m2mMesh)	{	//There is a mesh partner to sync with
			if(millis() - last_mesh_partner_advertisement_send_ > mesh_partner_advertisement_interval_)	{	//Send advertisements
				last_mesh_partner_advertisement_send_ = millis();
				if(multi_factor_partner_name_.equals("") == false)	{
					String partnerStatusUpdate = "Active";
					if(multi_factor_type_ == multiFactorOption::partnerOnly)	{
						if(multi_factor_partner_state_ == true)	{
							partnerStatusUpdate = "Open";
						} else {
							partnerStatusUpdate = "Ready to open";
						}
					}
					else if(multi_factor_type_ == multiFactorOption::buttonAndPartner)	{
						if(multi_factor_button_state_ == true)	{
							if(multi_factor_partner_state_ == true)	{
								partnerStatusUpdate = "Open";
							} else {
								partnerStatusUpdate = "Ready to open";
							}
						}
					}
					else if(multi_factor_type_ == multiFactorOption::cardAndPartner)	{
						if(multi_factor_card_state_ == true)	{
							if(multi_factor_partner_state_ == true)	{
								partnerStatusUpdate = "Open";
							} else {
								partnerStatusUpdate = "Ready to open";
							}
						}
					}
					else if(multi_factor_type_ == multiFactorOption::PINandPartner)	{
						if(multi_factor_pin_state_ == true)	{
							if(multi_factor_partner_state_ == true)	{
								partnerStatusUpdate = "Open";
							} else {
								partnerStatusUpdate = "Ready to open";
							}
						}
					}
					else if(multi_factor_type_ == multiFactorOption::cardPINandPartner)	{
						if(multi_factor_card_state_ == true && multi_factor_pin_state_ == true)	{
							if(multi_factor_partner_state_ == true)	{
								partnerStatusUpdate = "Open";
							} else {
								partnerStatusUpdate = "Ready to open";
							}
						}
					}
					if(debugStream_ != nullptr) {
						debugStream_->print(F("Sharing state \""));
						debugStream_->print(partnerStatusUpdate);
						debugStream_->print(F("\" with mesh partner \""));
						debugStream_->print(multi_factor_partner_name_);
						debugStream_->print(F("\": "));
					}
					if(m2mMesh.destination(multi_factor_partner_name_))	{
						if(m2mMesh.add(partnerStatusUpdate) && m2mMesh.send())	{
							if(debugStream_ != nullptr) {
								debugStream_->println(F("OK"));
							}
						} else	{
							if(debugStream_ != nullptr) {
								debugStream_->println(F("failed"));
							}
							m2mMesh.clearMessage();
						}
					} else	{
						if(debugStream_ != nullptr) {
							debugStream_->println(F("mesh name resolution failed, perhaps offline"));
						}
					}
				}
			}
		}
		if(m2mMesh.messageWaiting())	{
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Mesh message from "));
			}
			if(m2mMesh.sourceId() != m2mMesh.MESH_ORIGINATOR_NOT_FOUND)	{
				if(m2mMesh.nodeNameIsSet(m2mMesh.sourceId()))	{
					if(debugStream_ != nullptr) {
						debugStream_->print('"');
						debugStream_->print(m2mMesh.getNodeName(m2mMesh.sourceId()));
						debugStream_->print('"');
					}
					if(multi_factor_enabled_ == true && multi_factor_partner_type_ == multiFactorPartnerOption::m2mMesh && String(m2mMesh.getNodeName(m2mMesh.sourceId())).equals(multi_factor_partner_name_) == true)	{
						if(debugStream_ != nullptr) {
							debugStream_->print(F(" multi-factor partner"));
						}
						last_mesh_partner_advertisement_receipt_ = millis();
					}
				} else	{
					if(debugStream_ != nullptr) {
						debugStream_->print(F("node:"));
						debugStream_->print(m2mMesh.sourceId());
					}
				}
			} else	{
				if(debugStream_ != nullptr) {
					debugStream_->print(F("unknown node (how?)"));
				}
			}
			String retrievedContent = "UNSUPPORTED CONTENT";
			if(m2mMesh.nextDataType() == m2mMesh.USR_DATA_STRING)	{
				retrievedContent = m2mMesh.retrieveString();
			} else if(m2mMesh.nextDataType() == m2mMesh.USR_DATA_STR)	{
				uint8_t dataLength = m2mMesh.retrieveDataLength();
				char tempStr[dataLength];
				m2mMesh.retrieveStr(tempStr);
				retrievedContent = String(tempStr);
			}
			if(debugStream_ != nullptr) {
				debugStream_->print(F(" content: \""));
				debugStream_->print(retrievedContent);
				debugStream_->println('"');
			}
			if(multi_factor_type_ == multiFactorOption::partnerOnly || multi_factor_type_ == multiFactorOption::buttonAndPartner || multi_factor_type_ == multiFactorOption::cardAndPartner || multi_factor_type_ == multiFactorOption::PINandPartner || multi_factor_type_ == multiFactorOption::cardPINandPartner)	{
				if((retrievedContent.equals("Ready to open") || retrievedContent.equals("Open")) && multi_factor_partner_state_ == false)	{
					multi_factor_partner_state_ = true;
					last_multi_factor_interaction_ = millis();
					mesh_partner_advertisement_interval_ = mesh_partner_advertisement_fast_interval_;
					positiveFeedback();
					redLedFlash();
				}
				else if((retrievedContent.equals("Open") || retrievedContent.equals("Active")) && multi_factor_partner_state_ == true)	{
					resetMultifactorState();
				}
			}
			m2mMesh.markMessageRead();
		}
		if(mesh_partner_online_ == false && last_mesh_partner_advertisement_receipt_ != 0 && millis() - last_mesh_partner_advertisement_receipt_ < mesh_partner_advertisement_timeout_)	{
			mesh_partner_online_ = true;
			#ifdef WEBUI_IN_LIBRARY
				webAdminStatusPageUpdate();
			#endif
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Mesh partner online"));
			}
		}
		if(mesh_partner_online_ == true && millis() - last_mesh_partner_advertisement_receipt_ > mesh_partner_advertisement_timeout_)	{
			mesh_partner_online_ = false;
			#ifdef WEBUI_IN_LIBRARY
				webAdminStatusPageUpdate();
			#endif
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Mesh partner offline"));
			}
		}
	}
	//Manage LEDs
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//3
	#endif
	if(red_led_fitted_) {
		if(red_led_state_ == true && red_led_on_time_ > 0 && millis() - red_led_state_last_changed_ > red_led_on_time_)	{	//Switch off the red LED after red_led_on_time_
			redLedOff(red_led_off_time_, red_led_on_time_);
		}
		if(red_led_state_ == false && red_led_off_time_ > 0 && millis() - red_led_state_last_changed_ > red_led_off_time_)	{	//Switch on the red LED after red_led_off_time_
			redLedOn(red_led_on_time_, red_led_off_time_);
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//4
	#endif
	if(green_led_fitted_) {
		if(green_led_state_ == true && green_led_on_time_ > 0 && millis() - green_led_state_last_changed_ > green_led_on_time_)	{	//Switch off the red LED after green_led_on_time_
			greenLedOff();
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//5
	#endif
	//Manage buzzer
	if(buzzer_fitted_ == true) {
		if(buzzer_state_ == true && buzzer_on_time_ > 0 && millis() - buzzer_state_last_changed_ > buzzer_on_time_)	{	//Track how long the buzzer was on for
			buzzerOff();
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//6
	#endif
	//Manage servo
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//7
	#endif
	if(servo_fitted_ == true) {
		/*
		if(servo_attached_ == true && millis() - servo_state_last_changed_ > 5000)	{
			servo_state_last_changed_ = millis();
			lockServo_->detach();	//Detach the servo when not in use
			servo_attached_ = false;
		}
		*/
		if(servo_open_ == true && millis() - servo_state_last_changed_ > 60000)	{
			if(servo_attached_ == false) {
				//lockServo_->attach(servo_pin_, servo_channel_, 0, 180, 500, 2400);
				lockServo_->attach(servo_pin_, 500, 2400);
				servo_attached_ = true;
			}
			lockServo_->write(servo_closed_postion_);
			servo_state_last_changed_ = millis();
			servo_open_ = false;
		}
	}
	//Manage RFID
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//8
	#endif
	if(rfid_reader_intialised_ == true && rfid_ != nullptr)	{	//Only run the RFID reader code if enabled
		rfid_->pollForCard();
	}
	//Manage TapCode
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//9
	#endif
	if(tap_code_enabled_ == true && tapCode_ != nullptr)	{	//Only run the tap code sections if enabled
		//Only read TapCode if the pin is not in use for something else, red/green buttons are sometimes shared with an LED
		if((red_led_state_ == false || tapCode_pin_ != red_led_pin_) && (green_led_state_ == false || tapCode_pin_ != green_led_pin_))	{
			tapCode_->read();
		}
	}
	//Manage buttons
	if(button1_fitted_ == true && button1_pin_ != tapCode_pin_ && button1_ != nullptr)	{	//Tap code reads the button separately
		//Only read button if the pin is not in use for something else, red/green buttons are sometimes shared with an LED
		if((red_led_state_ == false || button1_pin_ != red_led_pin_) && (green_led_state_ == false || button1_pin_ != green_led_pin_))	{
			this->button1_->read();	//Update EasyButton
		}
		if(this->button1_->isPressed() && button1_pressed_ == false)	{
			button1_pressed_ = true;
			if(multi_factor_enabled_ == true && multi_factor_type_ == multiFactorOption::buttonAndPartner && open_button_pin_ == button1_pin_) {
				multi_factor_button_state_ = true;
				mesh_partner_advertisement_interval_ = mesh_partner_advertisement_fast_interval_;
				last_multi_factor_interaction_ = millis();
				last_mesh_partner_advertisement_send_ = 0;
				positiveFeedback();
				redLedFlash();
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Button 1 pushed for multi-factor authorisation"));
				}
			} else {
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Button 1 pushed"));
				}
			}
		}
		if(this->button1_->isReleased() && button1_pressed_ == true)	{
			button1_pressed_ = false;
			if(debugStream_ != nullptr)	{
				//debugStream_->println(F("Button 1 released"));
			}
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//10
	#endif
	if(button2_fitted_ == true && button2_pin_ != tapCode_pin_&& button2_ != nullptr)	{	//Tap code reads the button separately
		if((red_led_state_ == false || button2_pin_ != red_led_pin_) && (green_led_state_ == false || button2_pin_ != green_led_pin_))	{
			this->button2_->read();	//Update EasyButton
		}
		if(this->button2_->isPressed() && button2_pressed_ == false)	{
			button2_pressed_ = true;
			if(multi_factor_enabled_ == true && multi_factor_type_ == multiFactorOption::buttonAndPartner && open_button_pin_ == button2_pin_) {
				multi_factor_button_state_ = true;
				mesh_partner_advertisement_interval_ = mesh_partner_advertisement_fast_interval_;
				last_multi_factor_interaction_ = millis();
				last_mesh_partner_advertisement_send_ = 0;
				positiveFeedback();
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Button 2 pushed for multi-factor authorisation"));
				}
			} else {
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Button 2 pushed"));
				}
			}
		}
		if(this->button2_->isReleased() && button2_pressed_ == true)	{
			button2_pressed_ = false;
			if(debugStream_ != nullptr)	{
				//debugStream_->println(F("Button 2 released"));
			}
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//11
	#endif
	if(button3_fitted_ == true && button3_pin_ != tapCode_pin_&& button3_ != nullptr)	{	//Tap code reads the button separately
		if((red_led_state_ == false || button3_pin_ != red_led_pin_) && (green_led_state_ == false || button3_pin_ != green_led_pin_))	{
			this->button3_->read();	//Update EasyButton
		}
		if(this->button3_->isPressed() && button3_pressed_ == false)	{
			button3_pressed_ = true;
			if(multi_factor_enabled_ == true && multi_factor_type_ == multiFactorOption::buttonAndPartner && open_button_pin_ == button3_pin_) {
				multi_factor_button_state_ = true;
				mesh_partner_advertisement_interval_ = mesh_partner_advertisement_fast_interval_;
				last_multi_factor_interaction_ = millis();
				last_mesh_partner_advertisement_send_ = 0;
				positiveFeedback();
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Button 3 pushed for multi-factor authorisation"));
				}
			} else {
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Button 3 pushed"));
				}
			}
		}
		if(this->button3_->isReleased() && button3_pressed_ == true)	{
			button3_pressed_ = false;
			if(debugStream_ != nullptr)	{
				//debugStream_->println(F("Button 3 released"));
			}
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//12
	#endif
	//Manage keypad matrix
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//13
	#endif
	if(matrix_fitted_ == true && matrix_ != nullptr)	{
		char matrixKeypress = matrix_->getKey();
		if(matrixKeypress){
			if(debugStream_ != nullptr)	{
				debugStream_->print(F("Keypad: "));
				debugStream_->println(String(matrixKeypress));
			}
			if(pin_length_ < abs_max_pin_length_ && pin_length_ < max_pin_length_)	{
				last_pin_entry_ = millis();
				entered_pin_[pin_length_++] = matrixKeypress;
				entered_pin_[pin_length_] = char(0);	//Null terminate the string
				positiveFeedback();
			}
			if(pin_length_ == abs_max_pin_length_ || pin_length_ == max_pin_length_)	{
				if(debugStream_ != nullptr)	{
					debugStream_->print(F("PIN: "));
					debugStream_->println(String(entered_pin_));
				}
				pin_entered_ = true;
			}
		}	else if(pin_length_ > 0 && millis() - last_pin_entry_ > pin_entry_timeout_)	{
			if(debugStream_ != nullptr)	{
				debugStream_->print(F("PIN: "));
				debugStream_->println(String(entered_pin_));
			}
			pin_entered_ = true;
		}
	}
	//Do lock interactivity
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//14
	#endif
	if(codeEntered()) {  //A code has been tapped out
		if(codeMatches(codeToOpen)) {  //Code matches the 'open' one
			allow();
		} else if(codeMatches(codeToLock)) { //Code matches the 'lock' one, permanently locks the lock until overridden
			seal();
		} else if(codeMatches(codeToUnlock)) { //Code matches the 'unlock' one, permanently opens the lock until overridden
			unseal();
		} else if(codeMatches(codeToWipeNextCard)) { //Code matches the 'card wipe' one, which will wipe the next presented card
			Serial.println(F("Wiping next card"));
			rfidReaderState = RFID_READER_WIPE_NEXT_CARD;
		} else if(codeMatches(codeToAllowNextCard)) { //Code matches the 'card allow' one, which will add this lock to the allow list for the card
			Serial.println(F("Allowing next card"));
			rfidReaderState = RFID_READER_ALLOW_NEXT_CARD;
		} else if(codeMatches(codeToDenyNextCard)) { //Code matches the 'card deny' one, which will remove this lock to the allow list for the card
			Serial.println(F("Denying next card"));
			rfidReaderState = RFID_READER_DENY_NEXT_CARD;
		} else {  //Deny access
			deny();
		}
		clearEnteredCode();  //Clear the entered code, otherwise it will keep repeating these checks
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);	//15
	#endif
	if(pinEntered()) { //Check if a pin has been entered and clear if it has
		if(pin_entry_enabled_ == true) {
			if(pin_to_open_.length() != 0 && pinMatches(pin_to_open_)) {  //PIN matches the 'open' one
				if(multi_factor_enabled_ == true)	{
					if(multi_factor_type_ == multiFactorOption::cardAndPIN || multi_factor_type_ == multiFactorOption::PINandPartner || multi_factor_type_ == multiFactorOption::cardPINandPartner)	{
						if(debugStream_ != nullptr)	{
							debugStream_->println(F("PIN entered for multi-factor authorisation"));
						}
						multi_factor_pin_state_ = true; //Mark that we have a valid PIN
						last_multi_factor_interaction_ = millis();
						last_mesh_partner_advertisement_send_ = 0;
						mesh_partner_advertisement_interval_ = mesh_partner_advertisement_fast_interval_;
						redLedFlash();
					} else	{
						if(debugStream_ != nullptr)	{
							debugStream_->println(F("PIN entered but not part of multi-factor authorisation"));
						}
						deny();
					}
				} else	{
					if(debugStream_ != nullptr)	{
						debugStream_->println(F("PIN authorised"));
					}
					allow();
				}
			} else if(pin_to_seal_.length() != 0 && pinMatches(pin_to_seal_)) { //PIN matches the 'lock' one, permanently locks the lock until overridden
				if(lock_state_ == lock_state_option_::normal)	{
					seal();
				}	else	{
					normal();
				}
			} else if(pin_to_unseal_.length() != 0 && pinMatches(pin_to_unseal_)) { //PIN matches the 'unlock' one, permanently opens the lock until overridden
				if(lock_state_ == lock_state_option_::normal)	{
					unseal();
				}	else	{
					normal();
				}
			} else if(pin_to_wipe_next_card_.length() != 0 && pinMatches(pin_to_wipe_next_card_)) { //PIN matches the 'card wipe' one, which will wipe the next presented card
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Wiping next card"));
				}
				rfidReaderState = RFID_READER_WIPE_NEXT_CARD;
			} else if(pin_to_allow_next_card_.length() != 0 && pinMatches(pin_to_allow_next_card_)) { //PIN matches the 'card allow' one, which will add this lock to the allow list for the card
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Allowing next card"));
				}
				rfidReaderState = RFID_READER_ALLOW_NEXT_CARD;
			} else if(pin_to_deny_next_card_.length() != 0 && pinMatches(pin_to_deny_next_card_)) { //PIN matches the 'card deny' one, which will remove this lock to the allow list for the card
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Denying next card"));
				}
				rfidReaderState = RFID_READER_DENY_NEXT_CARD;
			} else {  //Deny access
				deny();
			}
		}
		clearEnteredPin();
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);
	#endif
	if(rfid_reader_intialised_ == true) {
		if(cardPresent() && cardChanged())	{
			if(rfidReaderState == RFID_READER_NORMAL)	{
				if(lock_state_ == lock_state_option_::normal)	{
					if(cardAuthorised(accessId()) == true)	{
						if(multi_factor_enabled_ == true && multi_factor_card_state_ == false)	{
							if(multi_factor_type_ == multiFactorOption::cardAndPIN || multi_factor_type_ == multiFactorOption::cardAndPartner || multi_factor_type_ == multiFactorOption::cardPINandPartner)	{
								if(debugStream_ != nullptr)	{
									debugStream_->println(F("Card presented for multi-factor authorisation"));
								}
								multi_factor_card_state_ = true; //Mark that we have a valid card
								last_multi_factor_interaction_ = millis();
								last_mesh_partner_advertisement_send_ = 0;
								mesh_partner_advertisement_interval_ = mesh_partner_advertisement_fast_interval_;
								positiveFeedback();
								redLedFlash();
							} else	{
								if(debugStream_ != nullptr)	{
									debugStream_->println(F("Card presented but not part of multi-factor authorisation"));
								}
								deny();
							}
						} else {
							if(debugStream_ != nullptr)	{
								debugStream_->println(F("Card authorised"));
							}
							allow();
						}
					}	else	{
						if(debugStream_ != nullptr)	{
							debugStream_->println(F("Card not authorised"));
						}
						deny();
					}
				}	else if(lock_state_ == lock_state_option_::unsealed)	{
					positiveFeedback();
				}	else	{
					negativeFeedback();
				}
			}	else if(rfidReaderState == RFID_READER_WIPE_NEXT_CARD)	{
				if(wipeCard())	{
					if(debugStream_ != nullptr)	{
						debugStream_->println(F("Card wiped"));
					}
					negativeFeedback();
					rfidReaderState = RFID_READER_NORMAL;
				}
			}	else if(rfidReaderState == RFID_READER_ALLOW_NEXT_CARD)	{
				if(authoriseCard(accessId()))	{
					if(debugStream_ != nullptr)	{
						debugStream_->println(F("Card allowed"));
					}
					positiveFeedback();
					rfidReaderState = RFID_READER_NORMAL;
				}
			}	else if(rfidReaderState == RFID_READER_DENY_NEXT_CARD)	{
				if(revokeCard(accessId()))	{
					if(debugStream_ != nullptr)	{
						debugStream_->println(F("Card denied"));
					}
					negativeFeedback();
					rfidReaderState = RFID_READER_NORMAL;
				}
			}
		}
	}
	//Handle multi-factor authorisation
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);
	#endif
	if(multi_factor_enabled_ == true && multi_factor_succesful_ == false && millis() - last_multi_factor_interaction_ > multi_factor_delay_)	{
		if(multi_factor_type_ == multiFactorOption::cardAndPIN)	{	//Card and PIN
			if(multi_factor_card_state_ == true && multi_factor_pin_state_ == true)	{
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Multi-factor authorisation (card and PIN) succesful"));
				}
				allow();
				multi_factor_succesful_ = true;
			}
		} else if(multi_factor_type_ == multiFactorOption::buttonAndPartner)	{	//Card and partner
			if(multi_factor_button_state_ == true && multi_factor_partner_state_ == true)	{
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Multi-factor authorisation (button and partner) succesful"));
				}
				allow();
				multi_factor_succesful_ = true;
			}
		} else if(multi_factor_type_ == multiFactorOption::cardAndPartner)	{	//Card and partner
			if(multi_factor_card_state_ == true && multi_factor_partner_state_ == true)	{
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Multi-factor authorisation (card and partner) succesful"));
				}
				allow();
				multi_factor_succesful_ = true;
			}
		} else if(multi_factor_type_ == multiFactorOption::PINandPartner)	{	//PIN and partner
			if(multi_factor_pin_state_ == true && multi_factor_partner_state_ == true)	{
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Multi-factor authorisation (PIN and partner) succesful"));
				}
				allow();
				multi_factor_succesful_ = true;
			}
		} else if(multi_factor_type_ == multiFactorOption::cardPINandPartner)	{	//Card, pin & partner
			if(multi_factor_card_state_ == true && multi_factor_pin_state_ == true && multi_factor_partner_state_ == true)	{
				if(debugStream_ != nullptr)	{
					debugStream_->println(F("Multi-factor authorisation (card, pin & partner) succesful"));
				}
				allow();
				multi_factor_succesful_ = true;
			}
		}
		if(last_multi_factor_interaction_ != 0 && millis() - last_multi_factor_interaction_ > multi_factor_timeout_) {
			if(debugStream_ != nullptr)	{
				debugStream_->print(F("Multi-factor authorisation timed out after "));
				debugStream_->print(multi_factor_timeout_);
				debugStream_->println(F("ms"));
			}
			deny();
			resetMultifactorState();
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);
	#endif
	if(WiFi.softAPgetStationNum() != current_number_of_clients_) {
		current_number_of_clients_ = WiFi.softAPgetStationNum();
		if(debugStream_ != nullptr)	{
			debugStream_->print(F("Number of WiFi clients:"));
			debugStream_->print(current_number_of_clients_);
			debugStream_->print('/');
			debugStream_->println(maximum_number_of_clients_);
		}
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);
	#endif
	if(game_enabled_) {
		game.runFsm();  //Run the game finite state machine
	}
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);
	#endif
	if(dns_server_started_ == true) {
		  captivePortalDnsServer->processNextRequest();
	}
	#ifdef WEBUI_IN_LIBRARY
		#ifdef HOUSEKEEPING_DEBUG
			Serial.println(housekeepingdebug++);
		#endif
		if(millis() - last_clock_update_ > 10000) {
			last_clock_update_ = millis();
			webAdminStatusPageUpdate();
		}
	#endif
	#ifdef HOUSEKEEPING_DEBUG
		Serial.println(housekeepingdebug++);
	#endif
	if(last_lock_state_change_ != 0 && next_lock_state_change_ !=0 && millis() - last_lock_state_change_ > next_lock_state_change_) {
		last_lock_state_change_ = 0;
		next_lock_state_change_ = 0;
		lock_state_ = lock_state_option_::normal;
	}
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::resetMultifactorState()
{
	last_multi_factor_interaction_ = 0;
	multi_factor_button_state_ = false;
	multi_factor_card_state_ = false;
	multi_factor_pin_state_ = false;
	multi_factor_partner_state_ = false;
	multi_factor_succesful_ = false;
	mesh_partner_advertisement_interval_ = mesh_partner_advertisement_default_interval_;
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::rtcValid()
{
    #if defined(ESP32)
	struct tm timeinfo;
    if(getLocalTime(&timeinfo))	//Only works if time is set
    {
		return true;
	}
	else
	{
		return false;
	}
    #elif defined(ESP8266)
	time_t now;
	time(&now);
	struct tm * timeinfo;
	timeinfo = localtime(&now);
	if(timeinfo->tm_year > 100)	//If it's post-Y2K the time is probably valid. Better suggestions for checking welcome!
	{
		return true;
	}
	else
	{
		return false;
	}
	#endif
	return false;
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

void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableButton1(uint8_t pin)	{
	button1_pin_ = pin;
	button1_fitted_ = true;
}
uint8_t ICACHE_FLASH_ATTR LarpHackableRfidLock::button1()	{
	return button1_pin_;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableButton2(uint8_t pin)	{
	button2_pin_ = pin;
	button2_fitted_ = true;
}
uint8_t ICACHE_FLASH_ATTR LarpHackableRfidLock::button2()	{
	return button2_pin_;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableButton3(uint8_t pin)	{
	button3_pin_ = pin;
	button3_fitted_ = true;
}
uint8_t ICACHE_FLASH_ATTR LarpHackableRfidLock::button3()	{
	return button3_pin_;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableServo(uint8_t pin)	{
	servo_pin_ = pin;
	servo_fitted_ = true;
}

//Matrix keypad support
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableMatrixKeypad(uint8_t rowPin1, uint8_t rowPin2, uint8_t rowPin3, uint8_t rowPin4, uint8_t colPin1, uint8_t colPin2, uint8_t colPin3)	{
	matrix_row_pins_[0] = rowPin1;
	matrix_row_pins_[1] = rowPin2;
	matrix_row_pins_[2] = rowPin3;
	matrix_row_pins_[3] = rowPin4;
	matrix_col_pins_[0] = colPin1;
	matrix_col_pins_[1] = colPin2;
	matrix_col_pins_[2] = colPin3;
	matrix_ = new Keypad(makeKeymap(matrix_keys_), matrix_row_pins_, matrix_col_pins_, sizeof(matrix_row_pins_), sizeof(matrix_col_pins_));
	matrix_fitted_ = true;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::setPinLength(uint8_t length)	{
	if(length < abs_max_pin_length_)	{
		max_pin_length_ = length;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::setPinTimeout(uint32_t timeout)	{
	pin_entry_timeout_ = timeout;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addPinToOpen(char* pinToAdd)	{
	pin_to_open_ = String(pinToAdd);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addPinToSeal(char* pinToAdd)	{
	pin_to_seal_ = String(pinToAdd);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addPinToUnseal(char* pinToAdd)	{
	pin_to_unseal_ = String(pinToAdd);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addPinToWipeNextCard(char* pinToAdd)	{
	pin_to_wipe_next_card_ = String(pinToAdd);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addPinToAllowNextCard(char* pinToAdd)	{
	pin_to_allow_next_card_ = String(pinToAdd);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addPinToDenyNextCard(char* pinToAdd)	{
	pin_to_deny_next_card_ = String(pinToAdd);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::pinEntered()	{
	return pin_entered_;
}
char* ICACHE_FLASH_ATTR LarpHackableRfidLock::enteredPin()	{
	return entered_pin_;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::clearEnteredPin()	{
	pin_length_ = 0;
	entered_pin_[0] = char(0);	//Null terminate the string
	pin_entered_ = false;
}
//bool ICACHE_FLASH_ATTR LarpHackableRfidLock::pinMatches(char* pin) {
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::pinMatches(String pin) {
	/*
	if(pin_length_ > 0 && pin != nullptr)	{
		if(pin_length_ == strlen(pin))	{
			return strncmp(entered_pin_,pin,pin_length_) == 0;
		}
	}
	*/
	if(pin_length_ > 0)	{
		return String(entered_pin_).equals(pin);
	}
	return false;
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableRedLed(uint8_t pin, uint8_t onLevel)	{
	red_led_pin_ = pin;
	red_led_fitted_ = true;
	red_led_pin_on_value_ = onLevel;
	if(red_led_pin_on_value_ == HIGH)
	{
		red_led_pin_off_value_ = LOW;
	}
	else
	{
		red_led_pin_off_value_ = HIGH;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedOn(uint32_t on_time, uint32_t off_time)	{
	if(red_led_fitted_)	{
		if(red_led_state_ == false)	{
			//A shared pin needs changing to an output
			if((button1_fitted_ && red_led_pin_ == button1_pin_ ) || (button2_fitted_ && red_led_pin_ == button2_pin_ ) || (button3_fitted_ && red_led_pin_ == button3_pin_))	{
				pinMode(red_led_pin_, OUTPUT);
			}
			digitalWrite(red_led_pin_,red_led_pin_on_value_);
			red_led_state_ = true;
			red_led_state_last_changed_ = millis();
			if(debugStream_ != nullptr)
			{
				if(on_time == 0) {
					debugStream_->println(F("Lock red LED on"));
				} else {
					debugStream_->printf("Lock red LED on for %ums\r\n", on_time);
				}
			}
		}
		red_led_on_time_= on_time;
		red_led_off_time_= off_time;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedOff(uint32_t off_time, uint32_t on_time)	{
	if(red_led_fitted_)	{
		if(red_led_state_ == true)	{
			//A shared pin needs changing back to an input
			if((button1_fitted_ && red_led_pin_ == button1_pin_ )||(button2_fitted_ && red_led_pin_ == button2_pin_ )||(button3_fitted_ && red_led_pin_ == button3_pin_))	{
				if(red_led_pin_off_value_ == HIGH)	{
					pinMode(red_led_pin_, INPUT_PULLUP);
				}	else	{
					pinMode(red_led_pin_, INPUT_PULLDOWN);
				}
			}	else	{
				digitalWrite(red_led_pin_,red_led_pin_off_value_);
			}
			red_led_state_ = false;
			red_led_state_last_changed_ = millis();
			if(debugStream_ != nullptr)	{
				if(off_time == 0) {
					debugStream_->println(F("Lock red LED off"));
				} else {
					debugStream_->printf("Lock red LED off for %ums\r\n", off_time);
				}
			}
		}
		red_led_on_time_= on_time;
		red_led_off_time_ = off_time;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::redLedFlash(uint32_t on_time, uint32_t off_time)	{
	if(red_led_fitted_)	{
		if(red_led_state_ == false)	{
			//A shared pin needs changing to an output
			if((button1_fitted_ && red_led_pin_ == button1_pin_ ) || (button2_fitted_ && red_led_pin_ == button2_pin_ ) || (button3_fitted_ && red_led_pin_ == button3_pin_))	{
				pinMode(red_led_pin_, OUTPUT);
			}
			digitalWrite(red_led_pin_,red_led_pin_on_value_);
			red_led_state_ = true;
			red_led_state_last_changed_ = millis();
			red_led_on_time_= on_time;
			red_led_off_time_= off_time;
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
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableGreenLed(uint8_t pin, uint8_t onLevel)	{
	green_led_pin_ = pin;
	green_led_fitted_ = true;
	green_led_pin_on_value_ = onLevel;
	if(green_led_pin_on_value_ == HIGH)
	{
		green_led_pin_off_value_ = LOW;
	}
	else
	{
		green_led_pin_off_value_ = HIGH;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::greenLedOn(uint32_t on_time)	{
	if(green_led_fitted_)	{
		if(green_led_state_ == false)	{
			//A shared pin needs changing to an output
			if((button1_fitted_ && green_led_pin_ == button1_pin_ ) || (button2_fitted_ && green_led_pin_ == button2_pin_ ) || (button3_fitted_ && green_led_pin_ == button3_pin_))	{
				pinMode(green_led_pin_, OUTPUT);
			}
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
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::greenLedOff(uint32_t off_time)	{
	if(green_led_fitted_)	{
		if(green_led_state_ == true)	{
			if((button1_fitted_ && green_led_pin_ == button1_pin_ )||(button2_fitted_ && green_led_pin_ == button2_pin_ )||(button1_fitted_ && green_led_pin_ == button3_pin_))	{
				if(green_led_pin_off_value_ == HIGH)	{
					pinMode(green_led_pin_, INPUT_PULLUP);
				}	else	{
					pinMode(green_led_pin_, INPUT_PULLDOWN);
				}
			}	else	{
				digitalWrite(green_led_pin_,green_led_pin_off_value_);
			}
			green_led_state_ = false;
			green_led_state_last_changed_ = millis();
			green_led_off_time_ = off_time;
			if(debugStream_ != nullptr)	{
				if(off_time == 0) {
					debugStream_->println(F("Lock green LED off"));
				} else {
					debugStream_->printf("Lock green LED off for %ums\r\n", off_time);
				}
			}
		}
	}
}
/*
 *	Buzzer
 */
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableBuzzer(uint8_t pin, uint8_t onLevel)	{
	buzzer_pin_ = pin;
	buzzer_fitted_ = true;
	buzzer_pin_on_value_ = onLevel;
	if(buzzer_pin_on_value_ == HIGH)
	{
		buzzer_pin_off_value_ = LOW;
	}
	else
	{
		buzzer_pin_off_value_ = HIGH;
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::buzzerOn(uint16_t frequency, uint32_t on_time)	{
	if(buzzer_state_ == false)	{
		buzzer_on_time_ = on_time;
		#if defined ESP8266
			tone(buzzer_pin_, frequency, on_time);
		#elif defined ESP32
			ledcAttachPin(buzzer_pin_, 0);
			ledcWriteTone(buzzer_channel_, frequency);
		#endif
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
		#if defined ESP8266
			noTone(buzzer_pin_);
		#elif defined ESP32
			ledcWriteTone(buzzer_channel_, 0);
			ledcDetachPin(buzzer_pin_);
			pinMode(buzzer_pin_, OUTPUT);
			digitalWrite(buzzer_pin_, buzzer_pin_off_value_);
		#endif
		buzzer_state_ = false;
		buzzer_state_last_changed_ = millis();
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Lock buzzer off"));
		}
	}
}
/*
 *	Lock animations
 */
void ICACHE_FLASH_ATTR LarpHackableRfidLock::positiveFeedback()	{
	if(buzzer_fitted_ && buzzer_state_ == false)	{
		buzzerOn(musicalTone[3], feedbackBuzzerOnTime);
	}
	if(green_led_fitted_ && green_led_state_ == false)	{
		greenLedOn(feedbackLedOnTime);
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::negativeFeedback()	{
	if(buzzer_fitted_ && buzzer_state_ == false)	{
		buzzerOn(musicalTone[4], feedbackBuzzerOnTime);
	}
	if(red_led_fitted_ && red_led_state_ == false)	{
		redLedOn(feedbackLedOnTime);
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::allow()	{
	if(lock_state_ != lock_state_option_::allow) {
		lock_state_ = lock_state_option_::allow;
		last_lock_state_change_ = millis();
		next_lock_state_change_ = decisionLedOnTime;
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Allowing access"));
		}
		if(buzzer_fitted_)	{
			buzzerOn(musicalTone[0], decisionBuzzerOnTime);
		}
		if(green_led_fitted_)	{
			greenLedOn(decisionLedOnTime);
		}
		if(red_led_fitted_)	{
			redLedOff();
		}
		if(servo_fitted_ == true) {
			if(servo_attached_ == false) {
				//lockServo_->attach(servo_pin_, servo_channel_, 0, 180, 500, 2400);
				lockServo_->attach(servo_pin_, 500, 2400);
				servo_attached_ = true;
			}
			lockServo_->write(servo_open_postion_);
			servo_state_last_changed_ = millis();
			servo_open_ = true;
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::deny()	{
	if(lock_state_ != lock_state_option_::deny) {
		lock_state_ = lock_state_option_::deny;
		last_lock_state_change_ = millis();
		next_lock_state_change_ = decisionLedOnTime;
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Denying access"));
		}
		if(buzzer_fitted_)	{
			buzzerOn(musicalTone[6], decisionBuzzerOnTime);
		}
		if(red_led_fitted_)	{
			redLedOn(decisionLedOnTime);
		}
		if(green_led_fitted_)	{
			greenLedOff();
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::unseal()	{
	if(lock_state_ != lock_state_option_::unsealed) {
		lock_state_ = lock_state_option_::unsealed;
		last_lock_state_change_ = millis();
		next_lock_state_change_ = 0;	//Stick in this state
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Unsealing lock"));
		}
		if(buzzer_fitted_)	{
			buzzerOn(musicalTone[0], decisionBuzzerOnTime);
		}
		if(green_led_fitted_)	{
			greenLedOn();
		}
		if(red_led_fitted_)	{
			redLedOff();
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::seal()	{
	if(lock_state_ != lock_state_option_::sealed) {
		lock_state_ = lock_state_option_::sealed;
		last_lock_state_change_ = millis();
		next_lock_state_change_ = 0;	//Stick in this state
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Sealing lock"));
		}
		if(buzzer_fitted_)	{
			buzzerOn(musicalTone[4], decisionBuzzerOnTime);
		}
		if(red_led_fitted_)	{
			redLedOn();
		}
		if(green_led_fitted_)	{
			greenLedOff();
		}
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::normal()	{
	if(lock_state_ != lock_state_option_::normal) {
		lock_state_ = lock_state_option_::normal;
		last_lock_state_change_ = millis();
		next_lock_state_change_ = 0;	//Stick in this state
		if(debugStream_ != nullptr) {
			debugStream_->println(F("Setting lock to normal state"));
		}
		if(buzzer_fitted_)	{
			buzzerOff();
		}
		if(red_led_fitted_)	{
			redLedOff();
		}
		if(green_led_fitted_)	{
			greenLedOff();
		}
	}
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
	rfid_reader_intialised_ = rfid_->begin(rfid_authorisation_sector_);
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::authoriseCard(uint8_t id) {
	if(rfid_ != nullptr) {
		return(rfid_->authoriseCard(id, true));
	} else {
		return false;
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::revokeCard(uint8_t id) {
	if(rfid_ != nullptr) {
		return(rfid_->revokeCardAuthorisation(id));
	} else {
		return false;
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::wipeCard() {
	if(rfid_ != nullptr) {
		return(rfid_->revokeCardAuthorisation());
	} else {
		return false;
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::cardPresent() {
	if(rfid_ != nullptr) {
		return(rfid_->cardPresent());
	} else {
		return false;
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::cardChanged() {
	if(rfid_ != nullptr) {
		return(rfid_->cardChanged());
	} else {
		return false;
	}		
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::cardAuthorised(uint8_t id) {
	if(rfid_ != nullptr) {
		return(rfid_->checkCardAuthorisation(id));
	} else {
		return false;
	}		
}
/*
 *	Open button for a button where the security is physical like a key
 */
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableOpenButton(uint8_t pin, bool momentary) {
	open_button_enabled_ = true;
	open_button_momentary_ = momentary;
	open_button_pin_ = pin;
}
/*
 *	Tap code, which mostly passes through to the library methods
 */
void ICACHE_FLASH_ATTR LarpHackableRfidLock::enableTapCode(uint8_t pin) {
	tap_code_enabled_ = true;
	tapCode_pin_ = pin;
	tapCode_ = new TapCode(pin);	//Initialise tap code on pin 2
	if(debugStream_ != nullptr)	{
		tapCode_->debug(Serial); //Enable debugging
	}
	tapCode_->begin();
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::codeEntered() {
	if(tap_code_enabled_ == true && tapCode_ != nullptr)
	{
		return(tapCode_->finished());
	}
	return false;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addCodeToOpen(char* codeToAdd)	{
	if(codeToOpen != nullptr)	{
		return false;
		//delete [] codeToOpen;
	}
	codeToOpen = new char[strlen(codeToAdd) + 1];
	strlcpy(codeToOpen, codeToAdd, strlen(codeToAdd) + 1);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addCodeToLock(char* codeToAdd)	{
	if(codeToLock != nullptr)	{
		return false;
		//delete [] codeToLock;
	}
	codeToLock = new char[strlen(codeToAdd) + 1];
	strlcpy(codeToLock, codeToAdd, strlen(codeToAdd) + 1);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addCodeToUnlock(char* codeToAdd)	{
	if(codeToUnlock != nullptr)	{
		return false;
		//delete [] codeToUnlock;
	}
	codeToUnlock = new char[strlen(codeToAdd) + 1];
	strlcpy(codeToUnlock, codeToAdd, strlen(codeToAdd) + 1);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addCodeToWipeNextCard(char* codeToAdd)	{
	if(codeToWipeNextCard != nullptr)	{
		return false;
		//delete [] codeToWipeNextCard;
	}
	codeToWipeNextCard = new char[strlen(codeToAdd) + 1];
	strlcpy(codeToWipeNextCard, codeToAdd, strlen(codeToAdd) + 1);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addCodeToAllowNextCard(char* codeToAdd)	{
	if(codeToAllowNextCard != nullptr)	{
		return false;
		//delete [] codeToAllowNextCard;
	}
	codeToAllowNextCard = new char[strlen(codeToAdd) + 1];
	strlcpy(codeToAllowNextCard, codeToAdd, strlen(codeToAdd) + 1);
	return true;
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::addCodeToDenyNextCard(char* codeToAdd)	{
	if(codeToDenyNextCard != nullptr)	{
		return false;
		//delete [] codeToDenyNextCard;
	}
	codeToDenyNextCard = new char[strlen(codeToAdd) + 1];
	strlcpy(codeToDenyNextCard, codeToAdd, strlen(codeToAdd) + 1);
	return true;
}

bool ICACHE_FLASH_ATTR LarpHackableRfidLock::codeMatches(char* code) {
	if(tapCode_ != nullptr)
	{
		return(tapCode_->matches(code));
	}
	return false;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::clearEnteredCode() {
	if(tapCode_ != nullptr)
	{
		tapCode_->reset();
	}
}
/*
 *	Game
 */
void LarpHackableRfidLock::setupGame()
{
	if(game_type_ == 0) {
		game.type(ESPUIgames::gameType::whackamole);
	} else {
		game.type(ESPUIgames::gameType::simon);
	}
	game.setLength(game_length_); //Whack 10 moles to win
	game.setMaximumAttempts(game_retries_);
	//Game tab
	game.setTitle(lock_name_.c_str());
	game.setTabTitle(PSTR("Entry Security"));
	game.enableStartSwitch(PSTR("Hack!"));
	game.setWinContent(
		PSTR("Lock breached"),
		PSTR("See the controls in the new tab that has appeared.")); //Setting this makes a widget pop up if you win, not setting it relies on colours alone to show it
	game.setLoseContent(
		PSTR("Hack failed!"),
		PSTR("Flick the switch to try again"),
		PSTR("No further hack attempts possible")); //Setting this makes a widget pop up if you lose, not setting it relies on colours alone to show it
	if(game_type_ == 0) {	//Whack-a-mole
		game.addPlayButton(PSTR("Core Operating system"), PSTR("Hack"), ControlColor::Peterriver);
		game.addPlayButton(PSTR("Secure storage"), PSTR("Hack"), ControlColor::Sunflower);
		game.addPlayButton(PSTR("System bus"), PSTR("Hack"), ControlColor::Turquoise);
		game.addPlayButton(PSTR("Watchdog Daemon"), PSTR("Hack"), ControlColor::Carrot);
	} else {	//Simon
		game.addPlayButton(PSTR("Alpha"), PSTR("Hack"), ControlColor::Peterriver);
		game.addPlayButton(PSTR("Beta"), PSTR("Hack"), ControlColor::Sunflower);
		game.addPlayButton(PSTR("Gamma"), PSTR("Hack"), ControlColor::Turquoise);
		game.addPlayButton(PSTR("Delta"), PSTR("Hack"), ControlColor::Carrot);
	}
	game.addGameTab(); //Builds all the game controls
	game.setHelpTabTitle(PSTR("OC: How to hack"));
	if(game_type_ == 0) {
		game.setHelpContent(
			PSTR("How to play this game"),
			PSTR("Toggle the 'Hack!' switch to start the game.<br /><br />Every time a button lights, press it. If you react fast enough for long enough you will win.<br /><br />When you win all the controls turn green.<br /><br />If you don't hit all the buttons fast enough all the controls turn red and it will tell you what to do next.<br /><br />You can restart the hack by flicking the 'Hack!' switch and it will start from scratch but you only get <b>three attempts</b> and the game will get harder.<br /><br />After you have downloaded the files you want, if you flick the 'Hack' button to the off position, it clears up so it looks like you were never there and disconnects you.<br /><br />Please remember to disconnect after the hack otherwise you won't be able to receive incoming video calls.")
		);
	} else {
		game.setHelpContent(
			PSTR("How to play this game"),
			PSTR("Toggle the 'Hack!' switch to start the game.<br /><br />Repeat the sequence of lit buttons back on the screen just as it happened. If you get this right for long enough, eventually you will win. Hint: nothing is stopping you from writing the sequence down.<br /><br />When you win all the controls turn green and it will tell you what to do next.<br /><br />If you get something wrong, all the controls turn red.<br /><br />You can restart the hack by flicking the 'Hack!' switch and it will start from scratch but you only get <b>three attempts</b> and the game will get harder.<br /><br />After you have done what you want, if you flick the 'Hack' button to the off position, it clears up so it looks like you were never there and disconnects you.<br /><br />Please remember to disconnect after the hack otherwise you won't be able to receive incoming video calls.")
		);
	}
	game.addHelpTab(); //Builds all the help controls
	ESPUI.begin(game.title());  //ESPUI is started from the sketch in case you want to add your own controls and pages as well before starting ESPUI
}
#ifdef DRD
	bool ICACHE_FLASH_ATTR LarpHackableRfidLock::doubleReset() {
		if(this->reset_detector_.detectDoubleReset() == true) {
			return true;
		}
		return false;
	}
#endif
/*
 *	Wi-Fi admin interface
 */
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
		/*
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
		*/
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
void LarpHackableRfidLock::es32printResetReason(uint8_t core)
{
	switch (rtc_get_reset_reason(core))
	{
		case 1 : debugStream_->println ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
		case 3 : debugStream_->println ("SW_RESET");break;               /**<3,  Software reset digital core*/
		case 4 : debugStream_->println ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
		case 5 : debugStream_->println ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
		case 6 : debugStream_->println ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
		case 7 : debugStream_->println ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
		case 8 : debugStream_->println ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
		case 9 : debugStream_->println ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
		case 10 : debugStream_->println ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
		case 11 : debugStream_->println ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
		case 12 : debugStream_->println ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
		case 13 : debugStream_->println ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
		case 14 : debugStream_->println ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
		case 15 : debugStream_->println ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
		case 16 : debugStream_->println ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
		default : debugStream_->println ("NO_MEAN");
	}
}

LarpHackableRfidLock Lock;