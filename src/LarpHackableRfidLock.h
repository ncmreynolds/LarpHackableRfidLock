/*
 *	Code, plans, diagrams etc. for a LARP prop 'hackable' RFID lock
 *
 *	https://github.com/ncmreynolds/LarpHackableRfidLock
 *
 *	Released under LGPL-2.1 see https://github.com/ncmreynolds/LarpHackableRfidLock/LICENSE for full license
 *
 *
 */
#pragma once
#include <Arduino.h>

#define WEBUI_IN_LIBRARY
//#define DRD
//#define HOUSEKEEPING_DEBUG
#ifdef HOUSEKEEPING_DEBUG
	extern uint8_t housekeepingdebug;
#endif

#ifdef DRD
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
	#include <ESP_DoubleResetDetector.h>	//Double reset detector to put into factory reset
#endif
//Get the right supporting libraries for the platform
#if defined(ESP32)
	#include <WiFi.h>			//WiFi basics
	#include "LittleFS.h"		//File system
	#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
		#include "esp32/rom/rtc.h"
	#elif CONFIG_IDF_TARGET_ESP32S2
		#include "esp32s2/rom/rtc.h"
	#elif CONFIG_IDF_TARGET_ESP32C3
		#include "esp32c3/rom/rtc.h"
	#else 
		#error Target CONFIG_IDF_TARGET is not supported
	#endif
#else
	#include <ESP8266WiFi.h>	//WiFi basics
#endif
#include <m2mMesh.h>						//Mesh network
#include <FS.h>								//Filesystem admin routines
#ifdef WEBUI_IN_LIBRARY
	//#include <AsyncTCP.h>						//Used for Web UI
	#include <EmbAJAXOutputDriverESPAsync.h>	//Used for Web UI
	#include <EmbAJAX.h>						//Used for Web UI
	#include <AsyncElegantOTA.h>				//Used for OTA update
#endif
#include <DNSServer.h>						//Used for captive portal in Web UI
#include <ArduinoJson.h>					//Used for configuration storage
#include <time.h>							//Sync time from server
#include <TrivialRFIDauthorisation.h>		//RFID authentication offloaded to another library I wrote
#include <EasyButton.h>						//Rely on external library for button debounce/state management
#include <TapCode.h>						//Tap code library
#include <Keypad.h>							//Common Arduino Matrix Keypad library
//#include <Servo.h>							//Support servo based locks
#include <ESP32Servo.h>

#include <ESPUI.h>
#include <ESPUIgames.h>

const char web_admin_header_includes_[348] PROGMEM = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link href=\"//fonts.googleapis.com/css?family=Raleway:400,300,600\" rel=\"stylesheet\" type=\"text/css\"><link rel=\"stylesheet\" href=\"css/normalize.css\"><link rel=\"stylesheet\" href=\"css/skeleton.css\"><link rel=\"icon\" type=\"image/png\" href=\"images/favicon.png\">";

class LarpHackableRfidLock	{

	public:
		LarpHackableRfidLock();													//Constructor function
		~LarpHackableRfidLock();												//Destructor function
		void begin(uint8_t id = 0);												//Start the lock and configure all the peripherals
		void housekeeping();													//Do housekeeping for event/state changes
		//Access IDs
		uint8_t accessId();														//The current access ID
		void setAccessId(uint8_t id = 0);										//Set the current access ID
		//Buttons
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableButton1(uint8_t pin = D4);								//Enable button 1 on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableButton1(uint8_t pin = 18);								//Enable button 1 on the default pin
		#endif
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableButton2(uint8_t pin = D4);								//Enable button 2 on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableButton2(uint8_t pin = 9);								//Enable button 2 on the default pin
		#endif
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableButton3(uint8_t pin = D4);								//Enable button 3 on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableButton3(uint8_t pin = 8);								//Enable button 3 on the default pin
		#endif
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableServo(uint8_t pin = D4);								//Enable button 3 on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableServo(uint8_t pin = 18);								//Enable button 3 on the default pin
		#endif
		uint8_t button1();														//Return the GPIO used for button 1
		uint8_t button2();														//Return the GPIO used for button 2
		uint8_t button3();														//Return the GPIO used for button 3
		//Matrix keypad
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableMatrixKeypad(uint8_t rowPin1 = 10,						//Enable the matrix keypad on default pins
				uint8_t rowPin2 = 20,
				uint8_t rowPin3 = 21,
				uint8_t rowPin4 = 3,
				uint8_t colPin1 = 2,
				uint8_t colPin2 = 1,
				uint8_t colPin3 = 0);
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableMatrixKeypad(uint8_t rowPin1 = 10,						//Enable the matrix keypad on default pins
				uint8_t rowPin2 = 20,
				uint8_t rowPin3 = 21,
				uint8_t rowPin4 = 3,
				uint8_t colPin1 = 2,
				uint8_t colPin2 = 1,
				uint8_t colPin3 = 0);
		#endif
		void setPinLength(uint8_t length);										//Set a PIN length <= max length
		void setPinTimeout(uint32_t timeout);									//Set a timeout (from start of typing) for PIN entry
		bool addPinToOpen(char* pinToAdd);
		bool addPinToSeal(char* pinToAdd);
		bool addPinToUnseal(char* pinToAdd);
		bool addPinToWipeNextCard(char* pinToAdd);
		bool addPinToAllowNextCard(char* pinToAdd);
		bool addPinToDenyNextCard(char* pinToAdd);
		bool pinEntered();														//Has a PIN been entered
		//bool pinMatches(char*);													//Does the entered PIN match
		bool pinMatches(String);													//Does the entered PIN match
		char* enteredPin();														//Pointer to the string that represents the entered PIN
		void clearEnteredPin();													//Clear an entered PIN
		//LEDs
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableRedLed(uint8_t pin = D2, uint8_t onLevel = LOW);			//Enable the red LED on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableRedLed(uint8_t pin = 8, uint8_t onLevel = LOW);			//Enable the red LED on the default pin
		#endif
		void redLedOn(uint32_t on_time = 0, uint32_t off_time = 0);				//Switch on red LED. On time is 0 for permanent or otherwise in ms.
		void redLedOff(uint32_t off_time = 0, uint32_t on_time = 0);			//Switch off red LED. Off time is 0 for permanent or otherwise in ms.
		void redLedFlash(uint32_t on_time = 100, uint32_t off_time = 400);		//Flash red LED.
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableGreenLed(uint8_t pin = D4, uint8_t onLevel = LOW);		//Enable the red LED on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableGreenLed(uint8_t pin = 9, uint8_t onLevel = LOW);		//Enable the red LED on the default pin
		#endif
		void greenLedOn(uint32_t on_time = 0);									//Switch on green LED. On time is 0 for permanent or otherwise in ms.
		void greenLedOff(uint32_t off_time = 0);								//Switch off green LED. Off time is 0 for permanent or otherwise in ms.
		//Buzzer
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableBuzzer(uint8_t pin = D1);								//Enable the buzzer on the default pin
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableBuzzer(uint8_t pin = 19, uint8_t onLevel = LOW);			//Enable the buzzer on the default pin
		#endif
		void buzzerOn(uint16_t frequency = 440, uint32_t on_time = 100);		//Switch on buzzer, with a selected frequency and on time
		void buzzerOff();														//Switch off buzzer
		//void open();															//Change to open state
		#ifdef DRD
			bool doubleReset();														//Has the lock been reset
		#endif
		void debug(Stream &);													//Enable debug output on a Serial stream
		//Higher level lock control
		enum class lockEvent : uint8_t {open, unlock, lock, sleep};
		void allow();															//Unlocks briefly to allow the door to open and people to pass
		void deny();															//Play the error noise
		void unseal();															//Unlock, allowing the door to open freely
		void seal();															//Lock, preventing the door from opening even with normally valid access
		void normal();															//Return to normal behaviour
		void positiveFeedback();												//Provide positive feedback to the user
		void negativeFeedback();												//Provide negative feedback to the user
		void sleep(uint32_t);													//Deep sleep for an amount of time
		//RFID
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
		void enableRFID(uint8_t pin = D8);										//Enable RFID reading with SS on a pin (default sector 0)
		#elif defined(ARDUINO_ESP32C3_DEV)
		void enableRFID(uint8_t pin = 7);										//Enable RFID reading with SS on a pin (default sector 0)
		#endif
		bool authoriseCard(uint8_t);											//Allow this card
		bool revokeCard(uint8_t);												//Deny this card
		bool wipeCard();														//Wipe the flags from a card, setting them all to zero
		bool cardPresent();														//Is a card just present?
		bool cardChanged();														//Has it changed?
		bool cardAuthorised(uint8_t);											//Is this card authorised for this ID?
		//Open button
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableOpenButton(uint8_t pin = D4, bool momentary = true);		//Enables the use of a simple open button on the lock
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableOpenButton(uint8_t pin = 9, bool momentary = true);		//Enables the use of a simple open button on the lock
		#endif
		//Tap code
		#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
			void enableTapCode(uint8_t pin = RX);								//Enables the use of tap code on the lock
		#elif defined(ARDUINO_ESP32C3_DEV)
			void enableTapCode(uint8_t pin = 9);								//Enables the use of tap code on the lock
		#endif
		bool codeEntered();														//Has a code been entered?
		bool addCodeToOpen(char* codeToAdd);
		bool addCodeToLock(char* codeToAdd);
		bool addCodeToUnlock(char* codeToAdd);
		bool addCodeToWipeNextCard(char* codeToAdd);
		bool addCodeToAllowNextCard(char* codeToAdd);
		bool addCodeToDenyNextCard(char* codeToAdd);
		bool codeMatches(char*);												//Does the code match
		void clearEnteredCode();												//Reset the entered code
		bool setTapCode(char*, uint8_t);										//Set the tap code for an action
		//Wi-Fi client
		void enableWiFiClient(char*, char*);
		//Wi-Fi admin interface
		void enableWiFi();														//Enable WiFi
		void enableWiFiAdminInterface(uint8_t port = 81);
		bool saveConfiguration();												//Save configuration to the default file
	protected:
	
		//const uint8_t LOCK_NORMAL = 0;
		//const uint8_t LOCK_LOCKED = 1;
		//const uint8_t LOCK_UNLOCKED = 2;
		//const uint8_t LOCK_OPEN = 3;
		//uint8_t lockState = LOCK_NORMAL;
		const uint8_t RFID_READER_NORMAL = 0;
		const uint8_t RFID_READER_WIPE_NEXT_CARD = 1;
		const uint8_t RFID_READER_ALLOW_NEXT_CARD = 2;
		const uint8_t RFID_READER_DENY_NEXT_CARD = 3;
		uint8_t rfidReaderState = RFID_READER_NORMAL;

	private:
		//Basic stuff
		Stream *debugStream_ = nullptr;											//The stream used for debugging
		uint32_t restart_soon_ = 0;												//Set if a restart should happen soon
		uint32_t save_configuration_soon_ = 0;									//Set if a configuration save should happen soon
		static const uint16_t configDocumentSize = 2048;						//JSON document size
		//States
		enum class lock_state_option_ {
			normal = 0,
			allow = 1,
			deny = 2,
			sealed = 3,
			unsealed = 4
		};
		lock_state_option_ lock_state_ = lock_state_option_::normal;			//Most basic lock state
		uint32_t last_lock_state_change_ = 0;									//Time of last lock state change
		uint32_t next_lock_state_change_ = 0;									//Time to next lock state change, usually when it goes back to 'normal'
		//Buttons
		bool button1_fitted_ = false;											//Is button 1 fitted and enabled
		uint8_t button1_pin_ = 255;												//GPIO pin used for button 1
		EasyButton* button1_ = nullptr;											//EasyButton object for button 1
		bool button1_pressed_ = false;											//Is button 1 pressed
		bool button2_fitted_ = false;											//Is button 2 fitted and enabled
		uint8_t button2_pin_ = 255;												//GPIO pin used for button 2
		EasyButton* button2_ = nullptr;											//EasyButton object for button 2
		bool button2_pressed_ = false;											//Is button 2 pressed
		bool button3_fitted_ = false;											//Is button 3 fitted and enabled
		uint8_t button3_pin_ = 255;												//GPIO pin used for button 3
		EasyButton* button3_ = nullptr;											//EasyButton object for button 3
		bool button3_pressed_ = false;											//Is button 3 pressed
		bool servo_fitted_ = false;												//Is servo 3 fitted and enabled
		uint8_t servo_pin_ = 255;												//GPIO pin used for servo
		Servo* lockServo_;
		//Servo object
		#ifdef ESP32
			uint8_t servo_channel_ = 0;
		#endif
		uint16_t servo_min_pulse_width_ = 600;
		uint16_t servo_max_pulse_width_ = 2400;
		uint16_t servo_open_postion_ = 0;
		uint16_t servo_closed_postion_ = 180;
		bool servo_attached_ = false;
		bool servo_open_ = false;
		uint32_t servo_state_last_changed_ = 0;
		//Matrix keypad
		bool matrix_fitted_ = false;											//Is the keypad matrix fitted and enabled
		static const uint8_t matrix_rows_ = 4;									//Matrix has four rows
		static const uint8_t matrix_cols_ = 3;									//Matrix has three columns
		uint8_t matrix_row_pins_[matrix_rows_] = {10, 20, 21, 3};				//Matrix GPIO pins
		uint8_t matrix_col_pins_[matrix_cols_] = {2, 1, 0};						//Matrix GPIO pins
		char matrix_keys_[matrix_rows_][matrix_cols_] = {						//Matrix layout
			{ '1','2','3' },
			{ '4','5','6' },
			{ '7','8','9' },
			{ '*','0','#' }
		};
		Keypad* matrix_ = nullptr;												//Matrix keypad object
		static const uint8_t abs_max_pin_length_ = 24;							//Maximum length of a typed PIN
		uint8_t max_pin_length_ = 4;											//Length of a typed PIN, set by application
		char entered_pin_[abs_max_pin_length_ + 1];								//Entered PIN, deliberately not a number to allow for leading zeroes, * and # or potentially 4x4 keypads with ABCD
		uint8_t pin_length_ = 0;												//Current PIN length
		uint32_t last_pin_entry_ = 0;											//Time of last PIN entry
		uint32_t pin_entry_timeout_ = 5000;										//Timeout for entering a PIN
		bool pin_entered_ = false;												//Has a pin been entered
		//LEDs
		bool red_led_fitted_ = false;											//Is the red LED fitted and enabled
		uint8_t red_led_pin_ = 255;												//GPIO pin used for the red LED
		uint8_t red_led_pin_on_value_ = HIGH;									//Pin state for when the LED is on
		uint8_t red_led_pin_off_value_ = LOW;									//Pin state for when the LED is off
		bool red_led_state_ = false;											//State of the red LED
		uint32_t red_led_state_last_changed_ = 0;								//Time when the state of the red LED last changed
		uint32_t red_led_on_time_ = 0;
		uint32_t red_led_off_time_ = 0;
		bool green_led_fitted_ = false;											//Is the red LED fitted and enabled
		uint8_t green_led_pin_ = 255;											//GPIO pin used for the green LED
		uint8_t green_led_pin_on_value_ = LOW;									//Pin state for when the LED is on
		uint8_t green_led_pin_off_value_ = HIGH;								//Pin state for when the LED is off
		bool green_led_state_ = false;											//State of the green LED
		uint32_t green_led_state_last_changed_ = 0;								//Time when the state of the green LED last changed
		uint32_t green_led_on_time_ = 0;
		uint32_t green_led_off_time_ = 0;
		//Buzzer
		bool buzzer_fitted_ = false;
		uint8_t buzzer_pin_ = 255;												//GPIO pin used for the buzzer
		uint8_t buzzer_pin_on_value_ = LOW;										//Pin state for when the buzzer is on
		uint8_t buzzer_pin_off_value_ = HIGH;									//Pin state for when the buzzer is off
		#ifdef ESP32
		uint8_t buzzer_channel_ = 0;											//ledc channel used for the buzzer
		#endif
		bool buzzer_state_ = false;												//State of the buzzer
		uint32_t buzzer_state_last_changed_ = 0;								//Time when the state of the buzzer last changed
		uint32_t buzzer_on_time_ = 0;
		uint32_t max_buzzer_on_time_ = 60000;									//Maximum buzzer on time, to avoid total irritation
		//Reset detection
		#ifdef DRD
			DoubleResetDetector reset_detector_;									//Double reset detector
		#endif
		//Tones
		uint16_t musicalTone[8] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
		//Simple open button
		bool open_button_enabled_ = false;										//Is open button enabled
		bool open_button_momentary_ = true;										//Is open button momentary
		uint8_t open_button_pin_ = 255;											//GPIO pin used for open button
		//Tap code
		bool tap_code_enabled_ = false;											//Is TapCode enabled
		uint8_t tapCode_pin_ = 255;												//GPIO pin used for TapCode
		TapCode* tapCode_ = nullptr;											//Pointer to the tap code class, if enabled
		static const uint8_t abs_max_tap_code_length_ = 16;
		
		//RFID related
		TrivialRFIDauthorisation* rfid_ = nullptr;								//Pointer to the RFID wrapper class
		uint8_t rfid_authorisation_sector_ = 0;									//Sector where the RFID authorisation block is stored
		bool rfid_authorisation_enabled_ = false;
		bool rfid_authorisation_enabled_default_ = false;
		bool rfid_reader_intialised_ = false;
		
		//Wi-Fi admin interface
		uint8_t connectionRetries = 30;											//How many times a connection retries
		uint16_t connectionRetryFrequency = 1000;								//Interval between retries in ms
		
		//IP/connection
		void configureWifi();													//Start WiFi
		void printIpStatus_();													//Debug info about the IP status of the ESP
		void printConnectionStatus_();											//Debug info about the connection status of the ESP
		void configureTimeServer();												//Configure the time server
		//Client connection
		uint8_t current_number_of_clients_ = 0;
		uint8_t maximum_number_of_clients_ = 4;
				
		//Web admin UI
		#ifdef WEBUI_IN_LIBRARY
			bool web_admin_server_enabled_ = true;									//Is the web admin server enabled
			EmbAJAXOutputDriverWebServerClass* webAdminServer = nullptr;			//The EmbAJAX needs a 'driver'
			EmbAJAXOutputDriver* webAdminInterface_ = nullptr;						//The interface to most EmbAJAX features
			//Common static elements
			//constexpr static char web_admin_header_includes_[348] PROGMEM = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link href=\"//fonts.googleapis.com/css?family=Raleway:400,300,600\" rel=\"stylesheet\" type=\"text/css\"><link rel=\"stylesheet\" href=\"css/normalize.css\"><link rel=\"stylesheet\" href=\"css/skeleton.css\"><link rel=\"icon\" type=\"image/png\" href=\"images/favicon.png\">";
			void startWebAdminInterface();
			void createWebAdminCommonStaticElements();
			EmbAJAXStatic* web_admin_toolbar_ = nullptr;
			EmbAJAXStatic* startContainerDiv_ = nullptr;
			EmbAJAXStatic* startRowDiv_ = nullptr;
			EmbAJAXStatic* startOneColumnDiv_ = nullptr;
			EmbAJAXStatic* startTwoColumnDiv_ = nullptr;
			EmbAJAXStatic* startThreeColumnDiv_ = nullptr;
			EmbAJAXStatic* startFourColumnDiv_ = nullptr;
			EmbAJAXStatic* startSixColumnDiv_ = nullptr;
			EmbAJAXStatic* startTenColumnDiv_ = nullptr;
			EmbAJAXStatic* startTwelveColumnDiv_ = nullptr;
			EmbAJAXStatic* endDiv_ = nullptr;
			EmbAJAXStatic* startLabel_ = nullptr;
			EmbAJAXStatic* endLabel_ = nullptr;
			EmbAJAXStatic* startLabelSpan_ = nullptr;
			EmbAJAXStatic* endSpan_ = nullptr;
			EmbAJAXStatic* nbsp_ = nullptr;
			EmbAJAXStatic* startUl_ = nullptr;
			EmbAJAXStatic* startLi_ = nullptr;
			EmbAJAXStatic* endLi_ = nullptr;
			EmbAJAXStatic* endUl_ = nullptr;
			//EmbAJAXPushButton* save_button_ = nullptr;
			//static void web_admin_save_button_pressed(EmbAJAXPushButton*);
			//Status page
			EmbAJAXPage<75>* web_admin_status_page_ = nullptr;						//Put this all on heap, as it can't be defined as part of the class
			EmbAJAXBase* web_admin_status_page_elements_[75];
			//EmbAJAXStatic* web_admin_status_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static4_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static5_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static6_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static7_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static8_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static9_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static10_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static11_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static12_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static13_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static14_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static15_ = nullptr;
			EmbAJAXStatic* web_admin_status_page_static16_ = nullptr;
			EmbAJAXPushButton* web_admin_status_page_button0_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info00_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info01_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info02_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info03_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info04_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info05_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info06_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info07_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info08_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info09_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info10_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info11_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info12_ = nullptr;
			EmbAJAXMutableSpan* web_admin_status_page_info13_ = nullptr;
			void createWebAdminStatusPage();										//Create the web admin status page
			static void webAdminStatusPageCallback();										//Callback for status page
			static void web_admin_status_page_button0_pressed(EmbAJAXPushButton*);
			void webAdminStatusPageUpdate();
			char* lock_name_element_ = nullptr;
			char wifi_channel_element_[3];
			char mesh_info_element_[128];
			char uptime_element_[24];
			//Control page
			EmbAJAXPage<22>* web_admin_control_page_ = nullptr;
			EmbAJAXBase* web_admin_control_page_elements_[22];
			EmbAJAXStatic* web_admin_control_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_control_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_control_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_control_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_control_page_static4_ = nullptr;
			EmbAJAXPushButton* web_admin_control_page_button0_ = nullptr;
			EmbAJAXPushButton* web_admin_control_page_button1_ = nullptr;
			EmbAJAXPushButton* web_admin_control_page_button2_ = nullptr;
			EmbAJAXPushButton* web_admin_control_page_button3_ = nullptr;
			void createWebAdminControlPage();												//Create the web admin control page
			static void web_admin_control_page_button0_pressed(EmbAJAXPushButton*);
			static void web_admin_control_page_button1_pressed(EmbAJAXPushButton*);
			static void web_admin_control_page_button2_pressed(EmbAJAXPushButton*);
			static void web_admin_control_page_button3_pressed(EmbAJAXPushButton*);
			static void webAdminControlPageCallback();
			//General settings page
			EmbAJAXPage<22>* web_admin_general_settings_page_ = nullptr;						//Put this all on heap, as it can't be defined as part of the class
			EmbAJAXBase* web_admin_general_settings_page_elements_[22];
			EmbAJAXStatic* web_admin_general_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_general_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_general_settings_page_static2_ = nullptr;
			EmbAJAXTextInput<64>* web_admin_general_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<64>* web_admin_general_settings_page_text1_ = nullptr;
			EmbAJAXPushButton* web_admin_general_settings_page_save_button_ = nullptr;
			void createWebAdminGeneralSettingsPage();										//Create the web admin status page
			static void webAdminGeneralSettingsPageCallback();										//Callback for status page
			static void webAdminGeneralSettingsSaveButtonPressed(EmbAJAXPushButton*);
			//WiFi client settings
			EmbAJAXPage<52>* web_admin_wifi_client_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_wifi_client_settings_page_elements_[52];
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static4_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static5_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_client_settings_page_static6_ = nullptr;
			EmbAJAXCheckButton* web_admin_wifi_client_settings_page_check0_ = nullptr;
			EmbAJAXCheckButton* web_admin_wifi_client_settings_page_check1_ = nullptr;
			EmbAJAXTextInput<64>* web_admin_wifi_client_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<64>* web_admin_wifi_client_settings_page_text1_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_wifi_client_settings_page_text2_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_wifi_client_settings_page_text3_ = nullptr;
			EmbAJAXPushButton* web_admin_wifi_client_settings_page_save_button_ = nullptr;
			void createWebAdminWiFiClientSettingsPage();									//Create the web admin Wifi client settings page
			static void webAdminWiFiClientSettingsPageCallback();							//Callback for Wifi client settings page
			static void web_admin_wifi_client_settings_page_save_button_pressed(EmbAJAXPushButton*);
			//WiFi AP
			EmbAJAXPage<50>* web_admin_wifi_ap_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_wifi_ap_settings_page_elements_[50];
			EmbAJAXStatic* web_admin_wifi_ap_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_ap_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_ap_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_ap_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_ap_settings_page_static4_ = nullptr;
			EmbAJAXStatic* web_admin_wifi_ap_settings_page_static5_ = nullptr;
			EmbAJAXCheckButton* web_admin_wifi_ap_settings_page_check0_ = nullptr;
			EmbAJAXCheckButton* web_admin_wifi_ap_settings_page_check1_ = nullptr;
			EmbAJAXCheckButton* web_admin_wifi_ap_settings_page_check2_ = nullptr;
			EmbAJAXTextInput<64>* web_admin_wifi_ap_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<64>* web_admin_wifi_ap_settings_page_text1_ = nullptr;
			EmbAJAXPushButton* web_admin_wifi_ap_settings_page_save_button_ = nullptr;
			void createWebAdminWiFiAPSettingsPage();									//Create the web admin Wifi AP settings page
			static void webAdminWiFiAPSettingsPageCallback();							//Callback for Wifi AP settings page
			static void web_admin_wifi_ap_settings_page_save_button_pressed(EmbAJAXPushButton*);
			//WiFi Mesh
			EmbAJAXPage<32>* web_admin_m2mMesh_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_m2mMesh_settings_page_elements_[32];
			EmbAJAXStatic* web_admin_m2mMesh_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_m2mMesh_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_m2mMesh_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_m2mMesh_settings_page_static3_ = nullptr;
			EmbAJAXCheckButton* web_admin_m2mMesh_settings_page_check0_ = nullptr;
			EmbAJAXOptionSelect<14>* web_admin_m2mMesh_settings_page_select0_ = nullptr;
			char* web_admin_m2mMesh_settings_page_select0_options_[14];
			EmbAJAXTextInput<64>* web_admin_m2mMesh_settings_page_text0_ = nullptr;
			EmbAJAXPushButton* web_admin_m2mMesh_settings_page_save_button_ = nullptr;
			void createWebAdminM2mMeshSettingsPage();									//Create the web admin m2mMesh settings page
			static void webAdminM2mMeshSettingsPageCallback();							//Callback for m2mMesh settings page
			static void web_admin_m2mMesh_settings_page_save_button_pressed(EmbAJAXPushButton*);
			//PINs
			EmbAJAXPage<40>* web_admin_pin_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_pin_settings_page_elements_[40];
			EmbAJAXStatic* web_admin_pin_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_pin_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_pin_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_pin_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_pin_settings_page_static4_ = nullptr;
			EmbAJAXCheckButton* web_admin_pin_settings_page_check0_ = nullptr;
			EmbAJAXTextInput<abs_max_pin_length_>* web_admin_pin_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<abs_max_pin_length_>* web_admin_pin_settings_page_text1_ = nullptr;
			EmbAJAXTextInput<abs_max_pin_length_>* web_admin_pin_settings_page_text2_ = nullptr;
			EmbAJAXPushButton* web_admin_pin_settings_page_save_button_ = nullptr;
			void createWebAdminPINSettingsPage();
			static void webAdminPINSettingsPageCallback();
			static void web_admin_pin_settings_page_save_button_pressed(EmbAJAXPushButton*);
			//Tap code
			EmbAJAXPage<40>* web_admin_tap_code_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_tap_code_settings_page_elements_[40];
			EmbAJAXStatic* web_admin_tap_code_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_tap_code_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_tap_code_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_tap_code_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_tap_code_settings_page_static4_ = nullptr;
			EmbAJAXCheckButton* web_admin_tap_code_settings_page_check0_ = nullptr;
			EmbAJAXTextInput<abs_max_tap_code_length_>* web_admin_tap_code_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<abs_max_tap_code_length_>* web_admin_tap_code_settings_page_text1_ = nullptr;
			EmbAJAXTextInput<abs_max_tap_code_length_>* web_admin_tap_code_settings_page_text2_ = nullptr;
			EmbAJAXPushButton* web_admin_tap_code_settings_page_save_button_ = nullptr;
			void createWebAdminTapCodeSettingsPage();
			static void webAdminTapCodeSettingsPageCallback();
			static void webAdminTapCodeSettingsPageSaveButtonPressed(EmbAJAXPushButton*);
			//RFID provisioning
			EmbAJAXPage<66>* web_admin_rfid_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_rfid_settings_page_elements_[66];
			EmbAJAXStatic* web_admin_rfid_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_rfid_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_rfid_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_rfid_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_rfid_settings_page_static4_ = nullptr;
			EmbAJAXStatic* web_admin_rfid_settings_page_static5_ = nullptr;
			EmbAJAXPushButton* web_admin_rfid_settings_page_button0_ = nullptr;
			EmbAJAXPushButton* web_admin_rfid_settings_page_button1_ = nullptr;
			EmbAJAXPushButton* web_admin_rfid_settings_page_button2_ = nullptr;
			EmbAJAXCheckButton* web_admin_rfid_settings_page_check0_ = nullptr;
			EmbAJAXPushButton* web_admin_rfid_settings_page_save_button_ = nullptr;
			void createWebAdminRFIDPage();												//Create the web admin control page
			static void web_admin_rfid_settings_page_button0_pressed(EmbAJAXPushButton*);
			static void web_admin_rfid_settings_page_button1_pressed(EmbAJAXPushButton*);
			static void web_admin_rfid_settings_page_button2_pressed(EmbAJAXPushButton*);
			static void webAdminRFIDSettingsPageSaveButtonPressed(EmbAJAXPushButton*);
			static void webAdminRFIDPageCallback();
			//MQTT
			EmbAJAXPage<58>* web_admin_mqtt_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_mqtt_settings_page_elements_[58];
			EmbAJAXStatic* web_admin_mqtt_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static4_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static5_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static6_ = nullptr;
			EmbAJAXStatic* web_admin_mqtt_settings_page_static7_ = nullptr;
			EmbAJAXCheckButton* web_admin_mqtt_settings_page_check0_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_mqtt_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_mqtt_settings_page_text1_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_mqtt_settings_page_text2_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_mqtt_settings_page_text3_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_mqtt_settings_page_text4_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_mqtt_settings_page_text5_ = nullptr;
			EmbAJAXPushButton* web_admin_mqtt_settings_page_save_button_ = nullptr;
			void createWebAdminMQTTClientSettingsPage();
			static void webAdminMQTTSettingsPageSaveButtonPressed(EmbAJAXPushButton*);
			static void webAdminMQTTSettingsPageCallback();
			//Hacking game
			EmbAJAXPage<32>* web_admin_game_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_game_settings_page_elements_[32];
			EmbAJAXStatic* web_admin_game_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_game_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_game_settings_page_static2_ = nullptr;
			EmbAJAXCheckButton* web_admin_game_settings_page_check0_ = nullptr;
			EmbAJAXRadioGroup<2>* web_admin_game_settings_page_radio0_ = nullptr;
			char* web_admin_game_settings_page_radio0_options_[2];
			EmbAJAXPushButton* web_admin_game_settings_page_save_button_ = nullptr;
			void createWebAdminGameSettingsPage();
			static void webAdminGameSettingsPageSaveButtonPressed(EmbAJAXPushButton*);
			static void webAdminGameSettingsPageCallback();
			//Multi-factor
			EmbAJAXPage<54>* web_admin_multi_factor_settings_page_ = nullptr;
			EmbAJAXBase* web_admin_multi_factor_settings_page_elements_[54];
			EmbAJAXStatic* web_admin_multi_factor_settings_page_static0_ = nullptr;
			EmbAJAXStatic* web_admin_multi_factor_settings_page_static1_ = nullptr;
			EmbAJAXStatic* web_admin_multi_factor_settings_page_static2_ = nullptr;
			EmbAJAXStatic* web_admin_multi_factor_settings_page_static3_ = nullptr;
			EmbAJAXStatic* web_admin_multi_factor_settings_page_static4_ = nullptr;
			EmbAJAXStatic* web_admin_multi_factor_settings_page_static5_ = nullptr;
			EmbAJAXCheckButton* web_admin_multi_factor_settings_page_check0_ = nullptr;
			EmbAJAXRadioGroup<6>* web_admin_multi_factor_settings_page_radio0_ = nullptr;
			char* web_admin_multi_factor_settings_page_radio0_options_[2];
			EmbAJAXRadioGroup<2>* web_admin_multi_factor_settings_page_radio1_ = nullptr;
			char* web_admin_multi_factor_settings_page_radio1_options_[2];
			EmbAJAXTextInput<256>* web_admin_multi_factor_settings_page_text0_ = nullptr;
			EmbAJAXTextInput<256>* web_admin_multi_factor_settings_page_text1_ = nullptr;
			EmbAJAXPushButton* web_admin_multi_factor_settings_page_save_button_ = nullptr;
			void createWebAdminTwoFactorSettingsPage();
			static void webAdminTwoFactorSettingsPageSaveButtonPressed(EmbAJAXPushButton*);
			static void webAdminTwoFactorSettingsPageCallback();
		#endif
		//Captive port DNS server
		DNSServer* captivePortalDnsServer;
		bool dns_server_started_ = false;
		//Configuration management
		bool loadConfiguration(const char*);
		bool saveConfiguration(const char*);
		void loadDefaultConfiguration();
		//bool updateCharArray(char *array, String value);						//Compare and if necessary update a char array stored on heap
		void displayCurrentConfiguration();										//Print the current config for debugging
		const char configuration_file_[17] = "/lockConfig.json";				//Configuration file stored on LittleFS as key/value pairs
		bool configuration_changed_ = false;
		bool configuration_change_feedback_ = false;
		bool wifi_configuration_changed_ = false;
		bool mesh_network_configuration_changed_ = false;
		bool mesh_network_started_ = false;
		bool mesh_network_joined_ = false;
		//Lock name
		String lock_name_;														//Configuration value
		char lock_name_key_[5] = "name";											//Key in key/value pair
		String lock_name_default_ = "Lock";										//Default value if key is missing
		//Lock Access ID
		uint8_t lock_access_group_;
		char lock_access_group_key_[9] = "accessId";
		uint8_t lock_access_group_default_ = 0;
		//Is PIN entry enabled
		bool pin_entry_enabled_ = true;
		String pin_entry_enabled_key_  = "pin_entry_enabled_";
		String pin_to_open_ = "";
		String pin_to_open_key_ = "pin_to_open_";
		String pin_to_open_default_ = "2855";
		String pin_to_seal_ = "";
		String pin_to_seal_key_ = "pin_to_seal_";
		String pin_to_seal_default_ = "8415";
		String pin_to_unseal_ = "";
		String pin_to_unseal_key_ = "pin_to_unseal_";
		String pin_to_unseal_default_ = "3957";
		String pin_to_allow_next_card_ = "";
		String pin_to_allow_next_card_key_ = "pin_to_allow_next_card_";
		String pin_to_allow_next_card_default_ = "9147";
		String pin_to_deny_next_card_ = "";
		String pin_to_deny_next_card_key_ = "pin_to_deny_next_card_";
		String pin_to_deny_next_card_default_ = "8184";
		String pin_to_wipe_next_card_ = "";
		String pin_to_wipe_next_card_key_ = "pin_to_wipe_next_card_";
		String pin_to_wipe_next_card_default_ = "5619";
		//Tap code
		char tap_code_enabled_key_[15] = "tapCodeEnabled";
		bool tap_code_enabled_default_ = false;
		//Tap code positive feedback
		bool tap_code_positive_feedback_ = false;
		char tap_code_positive_feedback_key_[24] = "tapCodePositiveFeedback";
		bool tap_code_positive_feedback_default_ = true;
		//Tap code negative feedback
		bool tap_code_negative_feedback_ = false;
		char tap_code_negative_feedback_key_[24] = "tapCodeNegativeFeedback";
		bool tap_code_negative_feedback_default_ = true;
		//Tap codes
		char* codeToOpen = nullptr;
		char* codeToLock = nullptr;
		char* codeToUnlock = nullptr;
		char* codeToWipeNextCard = nullptr;
		char* codeToAllowNextCard = nullptr;
		char* codeToDenyNextCard = nullptr;
		String tap_code_open_;
		String tap_code_open_key_ = PSTR("tapCodeOpen");
		String tap_code_open_default_ = "open";
		String tap_code_unseal_;
		String tap_code_unseal_key_ = PSTR("tapCodeUnseal");
		String tap_code_unseal_default_ = "unseal";
		String tap_code_seal_;
		String tap_code_seal_key_ = PSTR("tapCodeSeal");
		String tap_code_seal_default_ = "seal";
		String tap_code_authorise_;
		String tap_code_authorise_key_ = PSTR("tapCodeAuthorise");
		String tap_code_authorise_default_ = "authorise";
		String tap_code_revoke_;
		String tap_code_revoke_key_ = PSTR("tapCodeRevoke");
		String tap_code_revoke_default_ = "revoke";
		String tap_code_revoke_all_;
		String tap_code_revoke_all_key_ = PSTR("tapCodeRevokeAll");
		String tap_code_revoke_all_default_ = "wipe";
		//RFID
		String rfid_authorisation_enabled_key_ = PSTR("rfidAuthorisationEnabled");
		//Is the Mesh network enabled
		bool mesh_network_enabled_;
		String mesh_network_enabled_key_ = PSTR("meshNetworkEnabled");
		bool mesh_network_enabled_default_ = false;
		uint8_t mesh_network_channel_;
		String mesh_network_channel_key_ = PSTR("meshNetworkChannel");
		uint8_t mesh_network_channel_default_ = 1;
		uint8_t mesh_network_id_;
		String mesh_network_id_key_ = PSTR("meshNetworkId");
		uint8_t mesh_network_id_default_ = 0;
		uint8_t mesh_network_size_;
		String mesh_network_size_key_ = PSTR("meshNetworkSize");
		uint8_t mesh_network_size_default_ = 16;
		bool mesh_network_autosize_;
		String mesh_network_autosize_key_ = PSTR("meshNetworkAutosize");
		bool mesh_network_autosize_default_ = false;
		//Is the WiFi client enabled
		bool wifi_client_enabled_ = false;
		String wifi_client_enabled_key_ = PSTR("wifiClientEnabled");
		bool wifi_client_enabled_default_ = false;
		//WiFi client SSID
		String wifi_client_ssid_;
		String wifi_client_ssid_key_ = PSTR("wifiClientSsid");
		String wifi_client_ssid_default_ = "";
		//WiFi client PSK
		String wifi_client_psk_;
		String wifi_client_psk_key_ = PSTR("wifiClientPsk");
		String wifi_client_psk_default_ = "";
		//Is the WiFi AP enabled
		bool wifi_ap_enabled_ = false;
		String wifi_ap_enabled_key_ = PSTR("wifiApEnabled");
		bool wifi_ap_enabled_default_ = true;
		//Is the WiFi AP SSID hidden
		bool wifi_ap_hidden_ = false;
		String wifi_ap_hidden_key_ = PSTR("wifiApHidden");
		bool wifi_ap_hidden_default_ = false;
		//Is the WiFi AP captive portal
		bool wifi_ap_captive_portal_ = true;
		String wifi_ap_captive_portal_key_ = PSTR("wifiApCaptivePortal");
		bool wifi_ap_captive_portal_default_ = true;
		//WiFi AP channel
		uint8_t wifi_ap_channel_ = 1;
		//WiFi AP max clients
		uint8_t wifi_ap_maximum_clients_ = 4;
		//WiFi AP SSID
		String wifi_ap_ssid_;
		String wifi_ap_ssid_key_ = PSTR("wifiApSsid");
		String wifi_ap_ssid_default_ = "LockConfiguration";
		//WiFi AP PSK
		String wifi_ap_psk_;
		String wifi_ap_psk_key_ = PSTR("wifiApPsk");
		String wifi_ap_psk_default_ = "letmeinnow";
		//Is NTP enabled
		bool ntp_client_enabled_;
		String ntp_client_enabled_key_ = PSTR("ntpClientEnabled");
		bool ntp_client_enabled_default = true;
		//NTP server
		String ntp_server_;
		String ntp_server_key_ = PSTR("ntpServer");
		String ntp_server_default_ = "pool.ntp.org";
		//Timezone
		String ntp_timezone_;
		String ntp_timezone_key_ = PSTR("ntpTimezone");
		String ntp_timezone_default_ = "GMT0BST,M3.5.0/1,M10.5.0";	//From https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
		//Timer for updating clock
		uint32_t last_clock_update_ = 0;
		bool rtcValid();					//Is the RTC valid? Not a perfect test.
		//MQTT
		bool mqtt_enabled_ = false;
		bool mqtt_enabled_default_ = false;
		String mqtt_enabled_key_ = PSTR("mqttEnabled");
		String mqtt_host_;
		String mqtt_host_key_ = PSTR("mqttHost");
		String mqtt_port_;
		String mqtt_port_key_ = PSTR("mqttPort");
		String mqtt_client_id_;
		String mqtt_client_id_key_ = PSTR("mqttClientId");
		String mqtt_username_;
		String mqtt_username_key_ = PSTR("mqttUsername");
		String mqtt_password_;
		String mqtt_password_key_ = PSTR("mqttPassword");
		String mqtt_topic_;
		String mqtt_topic_key_ = PSTR("mqttTopic");
		//Hacking game
		void setupGame();														//Set up the game
		bool game_enabled_ = false;
		bool game_enabled_default_ = false;
		String game_enabled_key_ = PSTR("gameEnabled");
		uint8_t game_type_ = 0;
		uint8_t game_type_default_ = 0;
		String game_type_key_ = PSTR("gameType");
		//Multi-factor authentication
		bool multi_factor_enabled_ = false;
		bool multi_factor_enabled_default_ = false;
		String multi_factor_enabled_key_ = PSTR("multiFactorEnabled");
		enum class multiFactorOption {
			partnerOnly = 0,
			buttonAndPartner = 1,
			cardAndPIN = 2,
			cardAndPartner = 3,
			PINandPartner = 4,
			cardPINandPartner = 5
		};
		multiFactorOption multi_factor_type_ = multiFactorOption::cardAndPIN;
		multiFactorOption multi_factor_type_default_ = multiFactorOption::cardAndPIN;
		String multi_factor_type_key_ = PSTR("multiFactorType");
		String multi_factor_partner_name_;
		String multi_factor_partner_name_key_ = PSTR("multiFactorPartner");
		enum class multiFactorPartnerOption {
			mqtt = 0,
			m2mMesh = 1
		};
		multiFactorPartnerOption multi_factor_partner_type_ = multiFactorPartnerOption::mqtt;
		multiFactorPartnerOption multi_factor_partner_type_default_ = multiFactorPartnerOption::mqtt;
		String multi_factor_partner_type_key_ = PSTR("multiFactorPartnerType");
		uint32_t multi_factor_delay_ = 500;																				//Short pause before doing multi-factor auth, allows positive feedback to happen
		uint32_t multi_factor_timeout_ = 30000;																			//How long to give for each extra factor
		uint32_t multi_factor_timeout_default_ = 30000;
		String multi_factor_timeout_key_ = PSTR("multiFactorTimeout");
		uint32_t last_multi_factor_interaction_ = 0;
		bool multi_factor_button_state_ = false;
		bool multi_factor_card_state_ = false;
		bool multi_factor_pin_state_ = false;
		bool multi_factor_partner_state_ = false;
		bool multi_factor_succesful_ = false;
		void resetMultifactorState();
		uint32_t last_mesh_partner_advertisement_send_ = 0;
		uint32_t last_mesh_partner_advertisement_receipt_ = 0;
		uint32_t mesh_partner_advertisement_interval_ = 5000;
		uint32_t mesh_partner_advertisement_default_interval_ = 5000;
		uint32_t mesh_partner_advertisement_fast_interval_ = 500;
		uint32_t mesh_partner_advertisement_timeout_ = 30000;
		bool mesh_partner_online_ = false;
		uint32_t feedbackLedOnTime = 100;
		uint32_t feedbackBuzzerOnTime = 50;
		uint32_t decisionLedOnTime = 10000;
		uint32_t decisionBuzzerOnTime = 1000;
		//Utility functions
		void es32printResetReason(uint8_t core);	//Print the reset reason
};

extern LarpHackableRfidLock Lock;