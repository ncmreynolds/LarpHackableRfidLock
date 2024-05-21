#ifdef ESPUI
//Include ESPUI for the HTTP interface
#define ESPUI
#ifdef ESPUI
	#include <ESPUI.h>
#endif
//Wrapper hack to allow the use of callbacks to the ESPUI library from the C++ class

LarpHackableRfidLock* LarpHackableRfidLockPointer = nullptr;	//A pointer to 'this' eventually

void ICACHE_FLASH_ATTR LarpHackableRfidLockHTTPCallbackWrapper(Control* sender, int type)
{
    if(LarpHackableRfidLockPointer != nullptr)	{
        LarpHackableRfidLockPointer->HTTPCallback(sender, type);
	}
}
LarpHackableRfidLock::LarpHackableRfidLock()	:	//Constructor function and member constructors
	reset_detector_(DRD_TIMEOUT, DRD_ADDRESS)	{	//Calls constructor of class DoubleResetDetector
	#ifdef ESPUI
		LarpHackableRfidLockPointer = this;	//Set the pointer for the callback wrapper hack
	#endif
}

LarpHackableRfidLock::~LarpHackableRfidLock()	{//Destructor function
	#ifdef ESPUI
		LarpHackableRfidLockPointer = nullptr;	//Delete the pointer for the callback wrapper hack
	#endif
}
		#ifdef ESPUI
			bool adminOpened();														//Was the lock opened through the admin interface
			void HTTPCallback(Control* sender, int type);							//Callback function has to be public for the wrapper to work
		#endif
		//ESPUI
		#ifdef ESPUI
			uint16_t tab_[6];
			uint16_t control_[40];
			bool control_state_[40];												//Array of button states
			bool save_button_visible_ = false;
			void populateESPUIadminInterface();
			void hideControl(uint16_t id);
			void showControl(uint16_t id);
			void highlightControl(uint16_t id);
			void normaliseControl(uint16_t id);
		#endif

void ICACHE_FLASH_ATTR LarpHackableRfidLock::hideControl(uint16_t id)
{
	//ESPUI.getControl(id)->Control.visible = false;
	ESPUI.getControl(id)->color = ControlColor::Emerald;
	ESPUI.updateControl(id);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::showControl(uint16_t id)
{
	//ESPUI.getControl(id)->Control.visible = true;
	ESPUI.getControl(id)->color = ControlColor::Carrot;
	ESPUI.updateControl(id);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::normaliseControl(uint16_t id)
{
	//ESPUI.getControl(id)->Control.visible = false;
	ESPUI.getControl(id)->color = ControlColor::Emerald;
	ESPUI.updateControl(id);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::highlightControl(uint16_t id)
{
	//ESPUI.getControl(id)->Control.visible = true;
	ESPUI.getControl(id)->color = ControlColor::Carrot;
	ESPUI.updateControl(id);
}
/*bool ICACHE_FLASH_ATTR LarpHackableRfidLock::controlVisible(uint16_t id)
{
	if(ESPUI.getControl(id)->color == ControlColor::Emerald) {
		return true;
	} else {
		return false;
	}
	//return(ESPUI.getControl(id)->visible = true);
}*/
void ICACHE_FLASH_ATTR LarpHackableRfidLock::populateESPUIadminInterface()	{
	control_[16]= ESPUI.addControl(ControlType::Label, "Time", "??:??", ControlColor::Emerald, Control::noParent);
	control_[39]= ESPUI.addControl(ControlType::Label, "Resources", "", ControlColor::Emerald, Control::noParent);
	control_[9] = ESPUI.addControl(ControlType::Button, "Configuration", "Save", ControlColor::Emerald, Control::noParent, LarpHackableRfidLockHTTPCallbackWrapper);
	//control_[12]= ESPUI.addControl(ControlType::Button, "Reboot", "Reboot now", ControlColor::Emerald, Control::noParent, LarpHackableRfidLockHTTPCallbackWrapper);
	//Tab 0
	tab_[0] = ESPUI.addControl(ControlType::Tab, "Door control", "Lock control");
	control_[0] = ESPUI.addControl(ControlType::Button, "Open", "Press", ControlColor::None, tab_[0], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[1] = ESPUI.addControl(ControlType::Button, "Unlock", "Press", ControlColor::None, tab_[0], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[2] = ESPUI.addControl(ControlType::Button, "Lock", "Press", ControlColor::None, tab_[0], LarpHackableRfidLockHTTPCallbackWrapper);
	//Tab 1 card control
	tab_[1] = ESPUI.addControl(ControlType::Tab, "Settings 2", "Card Admin");
	//control_[33]= ESPUI.addControl(ControlType::Button, ("Authorise access ID:" + String(lock_access_id_)).c_str(), "Next Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	//control_[34]= ESPUI.addControl(ControlType::Button, ("Revoke access ID:" + String(lock_access_id_)).c_str(), "Next Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[33]= ESPUI.addControl(ControlType::Button, "Authorise access", "Next Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[34]= ESPUI.addControl(ControlType::Button, "Revoke access", "Next Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[35]= ESPUI.addControl(ControlType::Button, "Revoke all access IDs", "Next Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	//control_[36]= ESPUI.addControl(ControlType::Button, ("Authorise access ID:" + String(lock_access_id_)).c_str(), "Every Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	//control_[37]= ESPUI.addControl(ControlType::Button, ("Revoke access ID:" + String(lock_access_id_)).c_str(), "Every Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[36]= ESPUI.addControl(ControlType::Button, "Authorise access", "Every Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[37]= ESPUI.addControl(ControlType::Button, "Revoke access", "Every Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[38]= ESPUI.addControl(ControlType::Button, "Revoke All", "Every Card", ControlColor::None, tab_[1], LarpHackableRfidLockHTTPCallbackWrapper);
	//Tab 2 lock setting
	tab_[2] = ESPUI.addControl(ControlType::Tab, "Settings 3", "Lock settings");
	control_[10] = ESPUI.addControl(ControlType::Text, "Lock name:", lock_name_, ControlColor::None, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[11] = ESPUI.addControl(ControlType::Number, "Access ID:", String(lock_access_id_), ControlColor::None, tab_[2], LarpHackableRfidLockHTTPCallbackWrapper);
	ESPUI.addControl(ControlType::Min, "", String(0), ControlColor::None, control_[11]);
	ESPUI.addControl(ControlType::Max, "", String(255), ControlColor::None, control_[11]);
	//Tab 3 tap code
	tab_[4] = ESPUI.addControl(ControlType::Tab, "Settings 3", "Tap Code");
	control_[18] = ESPUI.addControl(ControlType::Switcher, "Tap code enabled", tap_code_enabled_ ? "1":"0", ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[19] = ESPUI.addControl(ControlType::Switcher, "Positive feedback", tap_code_positive_feedback_ ? "1":"0", ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[20] = ESPUI.addControl(ControlType::Switcher, "Negative feedback", tap_code_negative_feedback_ ? "1":"0", ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[21] = ESPUI.addControl(ControlType::Text, "Open (open the door briefly)", tap_code_open_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[22] = ESPUI.addControl(ControlType::Text, "Lock (cards are needed to open the door)", tap_code_lock_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[23] = ESPUI.addControl(ControlType::Text, "Unlock (door always open)", tap_code_unlock_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[24] = ESPUI.addControl(ControlType::Text, "Seal (door CANNOT be opened)", tap_code_seal_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[25] = ESPUI.addControl(ControlType::Text, "Authorise (authorise next card for this door)", tap_code_authorise_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[26] = ESPUI.addControl(ControlType::Text, "Revoke (revoke authorisation of next card for this door)", tap_code_revoke_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[27] = ESPUI.addControl(ControlType::Text, "Revoke All (revoke authorisation of next card for everything)", tap_code_revoke_all_, ControlColor::None, tab_[4], LarpHackableRfidLockHTTPCallbackWrapper);
	//Tab 4 mesh network
	tab_[5] = ESPUI.addControl(ControlType::Tab, "Settings 3", "Mesh Network");
	control_[28] = ESPUI.addControl(ControlType::Switcher, "Mesh network enabled", mesh_network_enabled_ ? "1":"0", ControlColor::None, tab_[5], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[29] = ESPUI.addControl(ControlType::Number, "Mesh network channel", String(mesh_network_channel_), ControlColor::None, tab_[5], LarpHackableRfidLockHTTPCallbackWrapper);
	ESPUI.addControl(ControlType::Min, "", String(1), ControlColor::None, control_[29]);
	ESPUI.addControl(ControlType::Max, "", String(11), ControlColor::None, control_[29]);
	control_[30] = ESPUI.addControl(ControlType::Number, "Mesh network ID", String(mesh_network_id_), ControlColor::None, tab_[5], LarpHackableRfidLockHTTPCallbackWrapper);
	ESPUI.addControl(ControlType::Min, "", String(0), ControlColor::None, control_[30]);
	ESPUI.addControl(ControlType::Max, "", String(255), ControlColor::None, control_[30]);
	control_[31] = ESPUI.addControl(ControlType::Number, "Mesh network starting size", String(mesh_network_size_), ControlColor::None, tab_[5], LarpHackableRfidLockHTTPCallbackWrapper);
	ESPUI.addControl(ControlType::Min, "", String(1), ControlColor::None, control_[31]);
	ESPUI.addControl(ControlType::Max, "", String(255), ControlColor::None, control_[31]);
	control_[32] = ESPUI.addControl(ControlType::Switcher, "Mesh network auto size", mesh_network_autosize_ ? "1":"0", ControlColor::None, tab_[5], LarpHackableRfidLockHTTPCallbackWrapper);
	//Tab 5
	tab_[3] = ESPUI.addControl(ControlType::Tab, "Settings 4", "WiFi settings");
	control_[3] = ESPUI.addControl(ControlType::Switcher, "WiFi client enabled", wifi_client_enabled_ ? "1":"0", ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[4] = ESPUI.addControl(ControlType::Text, "WiFi SSID:", wifi_client_ssid_, ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[5] = ESPUI.addControl(ControlType::Text, "WiFi PSK:", wifi_client_psk_, ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[13]= ESPUI.addControl(ControlType::Switcher, "NTP client enabled", ntp_client_enabled_ ? "1":"0", ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[14]= ESPUI.addControl(ControlType::Text, "NTP server:", ntp_server_, ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[15]= ESPUI.addControl(ControlType::Text, "(https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv) Time zone:", ntp_timezone_, ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[6] = ESPUI.addControl(ControlType::Switcher, "WiFi AP enabled", wifi_ap_enabled_ ? "1":"0", ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[17]= ESPUI.addControl(ControlType::Switcher, "WiFi AP hidden", wifi_ap_hidden_ ? "1":"0", ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[7] = ESPUI.addControl(ControlType::Text, "WiFi SSID:", wifi_ap_ssid_, ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	control_[8] = ESPUI.addControl(ControlType::Text, "WiFi PSK:", wifi_ap_psk_, ControlColor::None, tab_[3], LarpHackableRfidLockHTTPCallbackWrapper);
	ESPUI.begin(lock_name_.c_str());
	/*if(basic_auth_username_ != nullptr && basic_auth_password_ != nullptr) {
		ESPUI.begin("ESPUI Control", basic_auth_username_, basic_auth_password_, port);
	} else {
		ESPUI.begin("ESPUI Control", nullptr, nullptr, port);
	}*/
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::HTTPCallback(Control* sender, int type) {
	if(sender->id == control_[0]) {
		if(type == B_DOWN) {
			control_state_[0] = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Open lock"));
			}
		} else {
			control_state_[0] = false;
		}
	} else if(sender->id == control_[1]) {
		if(type == B_DOWN) {
			control_state_[1] = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Lock"));
			}
		} else {
			control_state_[1] = false;
		}
	} else if(sender->id == control_[2]) {
		if(type == B_DOWN) {
			control_state_[2] = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Unlock"));
			}
		} else {
			control_state_[2] = false;
		}
	} else if(sender->id == control_[3]) {
		switch (type) {
		case S_ACTIVE:
			wifi_client_enabled_ = true;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("WiFi client enabled"));
			}
		break;
		case S_INACTIVE:
			wifi_client_enabled_ = false;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("WiFi client disabled"));
			}
		break;
		}
	} else if(sender->id == control_[4]) {
		if(not wifi_client_ssid_.equals(sender->value)) {
			wifi_client_ssid_ = sender->value;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("WiFi client SSID:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[5]) {
		if(not wifi_client_psk_.equals(sender->value)) {
			wifi_client_psk_ = sender->value;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("WiFi client PSK:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[6]) {
		switch (type) {
		case S_ACTIVE:
			wifi_ap_enabled_ = true;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("WiFi AP enabled"));
			}
		break;
		case S_INACTIVE:
			wifi_ap_enabled_ = false;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("WiFi AP disabled"));
			}
		break;
		}
	} else if(sender->id == control_[7]) {
		if(not wifi_ap_ssid_.equals(sender->value)) {
			wifi_ap_ssid_ = sender->value;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("WiFi AP SSID:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[8]) {
		if(not wifi_ap_psk_.equals(sender->value)) {
			wifi_ap_psk_ = sender->value;
			configuration_changed_ = true;
			wifi_configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("WiFi AP PSK:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[9]) {
		if(type == B_DOWN) {
		} else {
			if(configuration_changed_ == true && control_state_[9] == false) {
				control_state_[9] = true;
				if(debugStream_ != nullptr) {
					debugStream_->println(F("Saving config"));
				}
			}
		}
	} else if(sender->id == control_[10]) {
		if(not lock_name_.equals(sender->value)) {
			lock_name_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Lock name:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[11]) {
		if(lock_access_id_ != (sender->value).toInt()) {
			lock_access_id_ = (sender->value).toInt();
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Lock access ID:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[12]) {
		if(type == B_DOWN) {
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Rebooting"));
			}
		} else {
			delay(500);
			ESP.reset();
		}
	} else if(sender->id == control_[13]) {
		switch (type) {
		case S_ACTIVE:
			ntp_client_enabled_ = true;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("NTP client enabled"));
			}
		break;
		case S_INACTIVE:
			ntp_client_enabled_ = false;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("NTP client disabled"));
			}
		break;
		}
	} else if(sender->id == control_[14]) {
		if(not ntp_server_.equals(sender->value)) {
			ntp_server_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("NTP server:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[15]) {
		if(not ntp_timezone_.equals(sender->value)) {
			ntp_timezone_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("NTP timezone:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[17]) {
		switch (type) {
		case S_ACTIVE:
			wifi_ap_hidden_ = true;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("WiFi AP hidden"));
			}
		break;
		case S_INACTIVE:
			wifi_ap_hidden_ = false;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("WiFi AP visible"));
			}
		break;
		}
	} else if(sender->id == control_[18]) {
		switch (type) {
		case S_ACTIVE:
			tap_code_enabled_ = true;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Tap Codes enabled"));
			}
		break;
		case S_INACTIVE:
			tap_code_enabled_ = false;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Tap Codes disabled"));
			}
		break;
		}
	} else if(sender->id == control_[19]) {
		switch (type) {
		case S_ACTIVE:
			tap_code_positive_feedback_ = true;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Tap Code positive feedback enabled"));
			}
		break;
		case S_INACTIVE:
			tap_code_positive_feedback_ = false;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Tap Code positive feedback disabled"));
			}
		break;
		}
	} else if(sender->id == control_[20]) {
		switch (type) {
		case S_ACTIVE:
			tap_code_negative_feedback_ = true;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Tap Code negative feedback enabled"));
			}
		break;
		case S_INACTIVE:
			tap_code_negative_feedback_ = false;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->println(F("Tap Code negative feedback disabled"));
			}
		break;
		}
	} else if(sender->id == control_[21]) {
		if(not tap_code_open_.equals(sender->value)) {
			tap_code_open_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Open Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[22]) {
		if(not tap_code_lock_.equals(sender->value)) {
			tap_code_lock_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Lock Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[23]) {
		if(not tap_code_unlock_.equals(sender->value)) {
			tap_code_unlock_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Unlock Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[24]) {
		if(not tap_code_seal_.equals(sender->value)) {
			tap_code_seal_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Seal Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[25]) {
		if(not tap_code_authorise_.equals(sender->value)) {
			tap_code_authorise_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Authorise Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[26]) {
		if(not tap_code_revoke_.equals(sender->value)) {
			tap_code_revoke_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Revoke Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else if(sender->id == control_[27]) {
		if(not tap_code_revoke_all_.equals(sender->value)) {
			tap_code_revoke_all_ = sender->value;
			configuration_changed_ = true;
			if(debugStream_ != nullptr) {
				debugStream_->print(F("Revoke All Tap Code:"));
				debugStream_->println(sender->value);
			}
		}
	} else {
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
					switch (type) {
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
					switch (type) {
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
					switch (type) {
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
					debugStream_->println(sender->value);
				break;
				case Switcher:
					debugStream_->print(F("Switch: "));
					switch (type) {
					case S_ACTIVE:
						if(debugStream_ != nullptr) {
							debugStream_->println(F("ON"));
						}
					break;
					case S_INACTIVE:
						if(debugStream_ != nullptr) {
							debugStream_->println(F("OFF"));
						}
					break;
					}
				break;
				case Slider:
					debugStream_->print(F("Slider: "));
					debugStream_->println(sender->value);
				break;
				case Number:
					debugStream_->print(F("Number: "));
					debugStream_->println(sender->value);
				break;
				case Text:
					debugStream_->print(F("Text: "));
					debugStream_->println(sender->value);
				break;
				case Graph:
					debugStream_->print(F("Graph: "));
					debugStream_->println(sender->value);
				break;
				case GraphPoint:
					debugStream_->print(F("Graph point: "));
					debugStream_->println(sender->value);
				break;
				case Tab:
					debugStream_->print(F("Tab: "));
					debugStream_->println(sender->value);
				break;
				case Select:
					debugStream_->print(F("Select: "));
					debugStream_->println(sender->value);
				break;
				case Option:
					debugStream_->print(F("Option: "));
					debugStream_->println(sender->value);
				break;
				case Min:
					debugStream_->print(F("Min: "));
					debugStream_->println(sender->value);
				break;
				case Max:
					debugStream_->print(F("Max: "));
					debugStream_->println(sender->value);
				break;
				case Step:
					debugStream_->print(F("Step: "));
					debugStream_->println(sender->value);
				break;
				case Gauge:
					debugStream_->print(F("Gauge: "));
					debugStream_->println(sender->value);
				break;
				case Accel:
					debugStream_->print(F("Accel: "));
					debugStream_->println(sender->value);
				break;
			}
		}
	}
}
bool ICACHE_FLASH_ATTR LarpHackableRfidLock::adminOpened() {
	return(control_state_[0]);
}
#endif
