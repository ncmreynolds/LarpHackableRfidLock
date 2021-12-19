//Include guard
#ifndef HTTPadminInterface_h
#define HTTPadminInterface_h
//Always needed
#include <Arduino.h>
//Get the right Wi-Fi library
#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
//Include ESPUI for the HTTP interface
#include <ESPUI.h>

class HTTPadminInterface {

	public:
		HTTPadminInterface();													//Constructor
		~HTTPadminInterface();													//Destructor
		void begin(uint8_t port = 81);
		void debug(Stream &debugStream);
		bool openButtonPushed();
		void HTTPadminInterfaceButtonCallback(Control* sender, int type);		//Callback function has to be public for the wrapper to work
	protected:
	private:
		Stream *debugStream_ = nullptr;											//The stream used for debugging
		char* hostname_ = nullptr;												//IP hostname, usually the lock's name, munged into an acceptable hostname
		char clientPSK_[13] = "carolinaliar";
		char clientSSID_[10] = "HMS Belly";
		char* basic_auth_username_ = nullptr;
		char* basic_auth_password_ = nullptr;
		uint8_t connectionRetries = 30;											//How many times a connection retries
		uint16_t connectionRetryFrequency = 1000;								//Interval between retries in ms
		uint16_t tab_[3];
		uint16_t control_[9];
		bool control_state_[3];													//Array of button states
		void printIpStatus_();													//Debug info about the IP status of the ESP
		void printConnectionStatus_();											//Debug info about the connection status of the ESP
};

#endif