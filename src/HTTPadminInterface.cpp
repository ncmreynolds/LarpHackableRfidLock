//Include guard
#ifndef HTTPadminInterface_cpp
#define HTTPadminInterface_cpp
#include "HTTPadminInterface.h"

//Wrapper hack to allow the use of callbacks to the ESPUI library from the C++ class

HTTPadminInterface* HTTPadminInterfacePointer = nullptr;	//A pointer to 'this' eventually

void ICACHE_FLASH_ATTR HTTPadminInterfaceButtonCallbackWrapper(Control* sender, int type)
{
    if(HTTPadminInterfacePointer != nullptr)
	{
        HTTPadminInterfacePointer->HTTPadminInterfaceButtonCallback(sender, type);
	}
}

HTTPadminInterface::HTTPadminInterface()
{
	//Set the pointer for the callback wrapper hack
	HTTPadminInterfacePointer = this;
}
HTTPadminInterface::~HTTPadminInterface()
{
	//delete the pointer for the callback wrapper hack
	HTTPadminInterfacePointer = nullptr;
}

void ICACHE_FLASH_ATTR HTTPadminInterface::begin(uint8_t port) {
	if(hostname_ != nullptr) {
		#if defined(ESP32)
			WiFi.setHostname(hostname_);
		#else
			WiFi.hostname(hostname_);
		#endif
	}
	WiFi.persistent(false);										//Avoid flash wear
	WiFi.setAutoReconnect(true);								//Because why not?
	WiFi.begin(clientSSID_, clientPSK_);
	if(debugStream_ != nullptr)	{
		debugStream_->print(F("Trying to connect to SSID:\""));
		debugStream_->print(clientSSID_);
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
	//Tab 0
	tab_[0] = ESPUI.addControl(ControlType::Tab, "Door control", "Lock control");
	button_[0] = ESPUI.addControl(ControlType::Button, "Open", "Press", ControlColor::Peterriver, tab_[0], HTTPadminInterfaceButtonCallbackWrapper);
	button_[1] = ESPUI.addControl(ControlType::Button, "Unlock", "Press", ControlColor::Peterriver, tab_[0], HTTPadminInterfaceButtonCallbackWrapper);
	button_[2] = ESPUI.addControl(ControlType::Button, "Lock", "Press", ControlColor::Peterriver, tab_[0], HTTPadminInterfaceButtonCallbackWrapper);
	//Tab 1
	tab_[1] = ESPUI.addControl(ControlType::Tab, "Settings 2", "Lock Settings");
	//Tab 2
	tab_[2] = ESPUI.addControl(ControlType::Tab, "Settings 3", "Lock Admin");
	control_[3] = ESPUI.addControl(ControlType::Switcher, "WiFi client enabled", "", ControlColor::None, tab_[2], HTTPadminInterfaceButtonCallbackWrapper);
	control_[4] = ESPUI.addControl(ControlType::Text, "WiFi SSID:", "", ControlColor::Alizarin, tab_[2], HTTPadminInterfaceButtonCallbackWrapper);
	control_[5] = ESPUI.addControl(ControlType::Text, "WiFi PSK:", "", ControlColor::Alizarin, tab_[2], HTTPadminInterfaceButtonCallbackWrapper);
	control_[6] = ESPUI.addControl(ControlType::Switcher, "WiFi AP enabled", "", ControlColor::None, tab_[2], HTTPadminInterfaceButtonCallbackWrapper);
	control_[7] = ESPUI.addControl(ControlType::Text, "WiFi SSID:", "", ControlColor::Alizarin, tab_[2], HTTPadminInterfaceButtonCallbackWrapper);
	control_[8] = ESPUI.addControl(ControlType::Text, "WiFi PSK:", "", ControlColor::Alizarin, tab_[2], HTTPadminInterfaceButtonCallbackWrapper);
	ESPUI.begin("Lock");
	/*if(basic_auth_username_ != nullptr && basic_auth_password_ != nullptr) {
		ESPUI.begin("ESPUI Control", basic_auth_username_, basic_auth_password_, port);
	} else {
		ESPUI.begin("ESPUI Control", nullptr, nullptr, port);
	}*/
}

#if defined(ESP8266) || defined(ESP32)
void ICACHE_FLASH_ATTR HTTPadminInterface::debug(Stream &debugStream)
#else
void HTTPadminInterface::debug(Stream &debugStream)
#endif
{
	debugStream_ = &debugStream;		//Set the stream used for the terminal
	#if defined(ESP8266)
	if(&debugStream == &Serial) {
		  debugStream_->write(17);		//Send an XON to stop the hung terminal after reset on ESP8266
	}
	#endif
}

void HTTPadminInterface::printIpStatus_()
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

void HTTPadminInterface::printConnectionStatus_()
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

bool HTTPadminInterface::openButtonPushed() {
	return(buttonState_[0]);
}

void HTTPadminInterface::HTTPadminInterfaceButtonCallback(Control* sender, int type) {
	if(sender->id == button_[0]) {
		if(type = B_DOWN) {
			buttonState_[0] = true;
		} else {
			buttonState_[0] = false;
		}
	}
	else {
		if(debugStream_ != nullptr) {
			Serial.print("Text: ID: ");
			Serial.print(sender->id);
			Serial.print(", Value: ");
			Serial.println( sender->value );
		}
		switch ( type ) {
		case B_DOWN:
			if(debugStream_ != nullptr) {
			Serial.println( "Button DOWN" );
			}
		break;
		case B_UP:
			if(debugStream_ != nullptr) {
			Serial.println( "Button UP" );
			}
		break;
		}
	}
}


#endif