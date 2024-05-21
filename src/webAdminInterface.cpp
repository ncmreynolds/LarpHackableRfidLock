#include "LarpHackableRfidLock.h"
#ifdef WEBUI_IN_LIBRARY
void ICACHE_FLASH_ATTR LarpHackableRfidLock::startWebAdminInterface()	{
	createWebAdminCommonStaticElements();
	createWebAdminStatusPage();
	createWebAdminControlPage();
	createWebAdminGeneralSettingsPage();
	createWebAdminWiFiClientSettingsPage();
	createWebAdminWiFiAPSettingsPage();
	createWebAdminM2mMeshSettingsPage();
	createWebAdminPINSettingsPage();
	createWebAdminTapCodeSettingsPage();
	createWebAdminRFIDPage();
	createWebAdminGameSettingsPage();
	createWebAdminMQTTClientSettingsPage();
	createWebAdminTwoFactorSettingsPage();
	webAdminServer->on("/css/normalize.css", HTTP_GET, [](AsyncWebServerRequest *request) {request->send(LittleFS, "/css/normalize.css", "text/css");});
	webAdminServer->on("/css/skeleton.css", HTTP_GET, [](AsyncWebServerRequest *request) {request->send(LittleFS, "/css/skeleton.css", "text/css");});
	webAdminServer->on("/images/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request) {request->send(LittleFS, "/images/favicon.png", "image/png");});
	webAdminServer->on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {request->send(LittleFS, Lock.configuration_file_, "text/json");});
	AsyncElegantOTA.begin(webAdminServer);
	webAdminServer->begin();
}

void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminCommonStaticElements()	{
	web_admin_toolbar_ = new EmbAJAXStatic(PSTR(
	"<div class=\"row\"><div class=\"two columns\"><button onclick=\"window.location.href='/';\">Status</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/control';\">Control</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/general';\">General</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/wificlient';\">WiFi</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/wifiap';\">AP</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/mesh';\">Mesh</button></div></div>"
	"<div class=\"row\"><div class=\"two columns\"><button onclick=\"window.location.href='/pins';\">PINs</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/rfid';\">RFID</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/tapcodes';\">Tap codes</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/game';\">Game</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/mqtt';\">MQTT</button></div>"
	"<div class=\"two columns\"><button onclick=\"window.location.href='/multifactor';\">Multi-factor</button></div></div>"
	));
	startContainerDiv_ = new EmbAJAXStatic(PSTR("<div class=\"container\">"));
	startRowDiv_ = new EmbAJAXStatic(PSTR("<div class=\"row\">"));
	startOneColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"one column\">"));
	startTwoColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"two columns\">"));
	startThreeColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"three columns\">"));
	startFourColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"four columns\">"));
	startSixColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"six columns\">"));
	startTenColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"ten columns\">"));
	startTwelveColumnDiv_ = new EmbAJAXStatic(PSTR("<div class=\"twelve columns\">"));
	endDiv_ = new EmbAJAXStatic(PSTR("</div>"));
	startLabel_ = new EmbAJAXStatic(PSTR("<label>"));
	endLabel_ = new EmbAJAXStatic(PSTR("</label>"));
	startLabelSpan_ = new EmbAJAXStatic(PSTR("<span class=\"label-body\">"));
	endSpan_ = new EmbAJAXStatic(PSTR("</span>"));
	nbsp_ = new EmbAJAXStatic(PSTR("&nbsp;"));
	startUl_ = new EmbAJAXStatic(PSTR("<ul>"));
	startLi_ = new EmbAJAXStatic(PSTR("<li>"));
	endLi_ = new EmbAJAXStatic(PSTR("</li>"));
	endUl_ = new EmbAJAXStatic(PSTR("</ul>"));
	//save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), web_admin_save_button_pressed);
	//save_button_->setVisible(false);
}
/*
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_save_button_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_save_button_pressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.configuration_changed_ = false;
		Lock.save_button_->setVisible(false);
	}
}
*/
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminStatusPage()	{
	web_admin_status_page_static1_ = new EmbAJAXStatic(PSTR("The lock is operational"));
	web_admin_status_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"button0\">Restart now</label>"));
	web_admin_status_page_static3_ = new EmbAJAXStatic(PSTR("<h2>Configuration file</h2>"));
	web_admin_status_page_static4_ = new EmbAJAXStatic(PSTR("<iframe src=\"/config\" title=\"Lock configuration\" width=100%></iframe>"));
	web_admin_status_page_static5_ = new EmbAJAXStatic(PSTR("WiFi client: "));
	web_admin_status_page_static6_ = new EmbAJAXStatic(PSTR("WiFi AP: "));
	web_admin_status_page_static7_ = new EmbAJAXStatic(PSTR("NTP: "));
	web_admin_status_page_static8_ = new EmbAJAXStatic(PSTR("Mesh: "));
	web_admin_status_page_static9_ = new EmbAJAXStatic(PSTR("PIN entry: "));
	web_admin_status_page_static10_ = new EmbAJAXStatic(PSTR("RFID card entry: "));
	web_admin_status_page_static11_ = new EmbAJAXStatic(PSTR("Tap Code entry: "));
	web_admin_status_page_static12_ = new EmbAJAXStatic(PSTR("Hacking game entry: "));
	web_admin_status_page_static13_ = new EmbAJAXStatic(PSTR("MQTT integration: "));
	web_admin_status_page_static14_ = new EmbAJAXStatic(PSTR("Require multi-factor authorisation: "));
	web_admin_status_page_static15_ = new EmbAJAXStatic(PSTR("WiFi channel: "));
	web_admin_status_page_static16_ = new EmbAJAXStatic(PSTR("Uptime: "));
	web_admin_status_page_button0_ = new EmbAJAXPushButton(PSTR("button0"), PSTR("Restart"), web_admin_status_page_button0_pressed);
	web_admin_status_page_info00_ = new EmbAJAXMutableSpan(PSTR("info00"));	//Name
	web_admin_status_page_info01_ = new EmbAJAXMutableSpan(PSTR("info01"));	//WiFi
	web_admin_status_page_info02_ = new EmbAJAXMutableSpan(PSTR("info02"));	//NTP
	web_admin_status_page_info03_ = new EmbAJAXMutableSpan(PSTR("info03"));	//AP
	web_admin_status_page_info04_ = new EmbAJAXMutableSpan(PSTR("info04"));	//Mesh
	web_admin_status_page_info05_ = new EmbAJAXMutableSpan(PSTR("info05"));	//PIN entry
	web_admin_status_page_info06_ = new EmbAJAXMutableSpan(PSTR("info06"));	//RFID entry
	web_admin_status_page_info07_ = new EmbAJAXMutableSpan(PSTR("info07"));	//Tap code
	web_admin_status_page_info08_ = new EmbAJAXMutableSpan(PSTR("info08"));	//Hacking game
	web_admin_status_page_info09_ = new EmbAJAXMutableSpan(PSTR("info09"));	//MQTT
	web_admin_status_page_info10_ = new EmbAJAXMutableSpan(PSTR("info10"));	//Multi-factor
	web_admin_status_page_info11_ = new EmbAJAXMutableSpan(PSTR("info11"));	//WiFi channel
	web_admin_status_page_info12_ = new EmbAJAXMutableSpan(PSTR("info12"));	//Mesh info
	web_admin_status_page_info13_ = new EmbAJAXMutableSpan(PSTR("info13"));	//Uptime
	uint8_t index = 0;
	web_admin_status_page_elements_[index++] = startContainerDiv_;
		web_admin_status_page_elements_[index++] = web_admin_toolbar_;
		web_admin_status_page_elements_[index++] = web_admin_status_page_info00_;
		web_admin_status_page_elements_[index++] = startRowDiv_;
			web_admin_status_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_status_page_elements_[index++] = startUl_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static16_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info13_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static5_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info01_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static6_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info02_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static7_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info03_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static8_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info04_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info12_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static15_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info11_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static9_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info05_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static10_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info06_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static11_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info07_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static12_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info08_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static13_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info09_;
					web_admin_status_page_elements_[index++] = endLi_;
					web_admin_status_page_elements_[index++] = startLi_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_static14_;
						web_admin_status_page_elements_[index++] = web_admin_status_page_info10_;
					web_admin_status_page_elements_[index++] = endLi_;
				web_admin_status_page_elements_[index++] = endUl_;
			web_admin_status_page_elements_[index++] = endDiv_;
		web_admin_status_page_elements_[index++] = endDiv_;
		web_admin_status_page_elements_[index++] = startRowDiv_;
			web_admin_status_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_status_page_elements_[index++] = web_admin_status_page_static3_;
				web_admin_status_page_elements_[index++] = web_admin_status_page_static4_;
			web_admin_status_page_elements_[index++] = endDiv_;
		web_admin_status_page_elements_[index++] = endDiv_;
		web_admin_status_page_elements_[index++] = startRowDiv_;
			web_admin_status_page_elements_[index++] = startTenColumnDiv_;
				web_admin_status_page_elements_[index++] = nbsp_;
			web_admin_status_page_elements_[index++] = endDiv_;
			web_admin_status_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_status_page_elements_[index++] = web_admin_status_page_static2_;
					web_admin_status_page_elements_[index++] = web_admin_status_page_button0_;
				web_admin_status_page_elements_[index++] = endDiv_;
			web_admin_status_page_elements_[index++] = endDiv_;
		web_admin_status_page_elements_[index++] = endDiv_;
	web_admin_status_page_elements_[index++] = endDiv_;
	lock_name_element_ = new char[lock_name_.length() + 10];
	sprintf_P(lock_name_element_, PSTR("<h1>%s</h1>"), String(lock_name_).c_str());
	web_admin_status_page_info00_->setValue(lock_name_element_, true);
	web_admin_status_page_ = new EmbAJAXPage<75>(web_admin_status_page_elements_, PSTR("Status"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_status_page_, PSTR("/"), webAdminStatusPageCallback);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminStatusPageCallback()	{
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_status_page_button0_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_status_page_button0_pressed restart soon"));
	}
	Lock.restart_soon_ = millis();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminStatusPageUpdate()	{
	web_admin_status_page_info01_->setValue(wifi_client_enabled_ == true ? (WiFi.status() == WL_CONNECTED ? PSTR("Connected") : PSTR("Enabled")) : PSTR("Disabled"));
	web_admin_status_page_info02_->setValue(wifi_ap_enabled_ == true ? PSTR("Enabled") : PSTR("Disabled"));
	web_admin_status_page_info03_->setValue(ntp_client_enabled_ == true ? PSTR("Enabled") : PSTR("Disabled"));
	web_admin_status_page_info04_->setValue(mesh_network_enabled_ == true ? (mesh_network_started_ == true ? (mesh_network_joined_ == true ? (mesh_partner_online_ == true ? PSTR("Partnered") : PSTR("Joined")) : PSTR("No peers")) : PSTR("Not started")) : PSTR("Disabled"));
	web_admin_status_page_info05_->setValue(pin_entry_enabled_ == true ? PSTR("Enabled") : PSTR("Disabled"));
	web_admin_status_page_info06_->setValue(rfid_authorisation_enabled_ == true ? (rfid_reader_intialised_ == true ? PSTR("Enabled & ready") : PSTR("Enabled but not ready")) : PSTR("Disabled"));
	web_admin_status_page_info07_->setValue(tap_code_enabled_ == true ? PSTR("Enabled") : PSTR("Disabled"));
	web_admin_status_page_info08_->setValue(game_enabled_ == true ? PSTR("Enabled") : PSTR("Disabled"));
	web_admin_status_page_info09_->setValue(mqtt_enabled_ == true ? PSTR("Enabled") : PSTR("Disabled"));
	if(multi_factor_enabled_ == false)	{
		web_admin_status_page_info10_->setValue(PSTR("Disabled"));
	} else if(multi_factor_type_ == multiFactorOption::partnerOnly)	{
		web_admin_status_page_info10_->setValue(PSTR("Partner only"));
	} else if(multi_factor_type_ == multiFactorOption::buttonAndPartner)	{
		web_admin_status_page_info10_->setValue(PSTR("Button and partner"));
	} else if(multi_factor_type_ == multiFactorOption::cardAndPIN)	{
		web_admin_status_page_info10_->setValue(PSTR("Card & PIN"));
	} else if(multi_factor_type_ == multiFactorOption::cardAndPartner)	{
		web_admin_status_page_info10_->setValue(PSTR("Card & partner"));
	} else if(multi_factor_type_ == multiFactorOption::PINandPartner)	{
		web_admin_status_page_info10_->setValue(PSTR("PIN & partner"));
	} else if(multi_factor_type_ == multiFactorOption::cardPINandPartner)	{
		web_admin_status_page_info10_->setValue(PSTR("Card, PIN & partner"));
	}
	sprintf_P(wifi_channel_element_, PSTR("%u"), m2mMesh.getChannel());
	web_admin_status_page_info11_->setValue(wifi_channel_element_);
	if(mesh_network_enabled_ == true && mesh_network_joined_ == true)	{
		if(mesh_partner_online_ == true) {
			sprintf_P(mesh_info_element_, PSTR(" with \"%s\""), multi_factor_partner_name_.c_str());
		} else {
			sprintf_P(mesh_info_element_, PSTR(" %u/%u nodes reachable"), m2mMesh.numberOfReachableNodes(), m2mMesh.numberOfNodes());
		}
	} else {
		strlcpy(mesh_info_element_, PSTR(""), 1);
	}
	web_admin_status_page_info12_->setValue(mesh_info_element_);
	sprintf_P(uptime_element_, PSTR("%03u:%02u:%02u"), millis()/3600000, (millis()/60000)%60, (millis()/1000)%60);
	web_admin_status_page_info13_->setValue(uptime_element_);
}
//Control page
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminControlPage()	{
	web_admin_control_page_static0_ = new EmbAJAXStatic(PSTR("<h1>Lock control</h1>"));
	web_admin_control_page_static1_ = new EmbAJAXStatic(PSTR("<label for=\"button0\">Open door</label>"));
	web_admin_control_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"button1\">Unlock door</label>"));
	web_admin_control_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"button2\">Lock door</label>"));
	web_admin_control_page_static4_ = new EmbAJAXStatic(PSTR("<label for=\"button3\">Seal door</label>"));
	web_admin_control_page_button0_ = new EmbAJAXPushButton(PSTR("button0"), PSTR("Open"), web_admin_control_page_button0_pressed);
	web_admin_control_page_button1_ = new EmbAJAXPushButton(PSTR("button1"), PSTR("Unlock"), web_admin_control_page_button1_pressed);
	web_admin_control_page_button2_ = new EmbAJAXPushButton(PSTR("button2"), PSTR("Lock"), web_admin_control_page_button2_pressed);
	web_admin_control_page_button3_ = new EmbAJAXPushButton(PSTR("button3"), PSTR("Seal"), web_admin_control_page_button3_pressed);
	uint8_t index = 0;
	web_admin_control_page_elements_[index++] = startContainerDiv_;
		web_admin_control_page_elements_[index++] = web_admin_toolbar_;
		web_admin_control_page_elements_[index++] = web_admin_control_page_static0_;
		web_admin_control_page_elements_[index++] = startRowDiv_;
			web_admin_control_page_elements_[index++] = startThreeColumnDiv_;
				web_admin_control_page_elements_[index++] = web_admin_control_page_static1_;
					web_admin_control_page_elements_[index++] = web_admin_control_page_button0_;
				web_admin_control_page_elements_[index++] = endDiv_;
			web_admin_control_page_elements_[index++] = startThreeColumnDiv_;
				web_admin_control_page_elements_[index++] = web_admin_control_page_static2_;
					web_admin_control_page_elements_[index++] = web_admin_control_page_button1_;
				web_admin_control_page_elements_[index++] = endDiv_;
			web_admin_control_page_elements_[index++] = startThreeColumnDiv_;
				web_admin_control_page_elements_[index++] = web_admin_control_page_static3_;
					web_admin_control_page_elements_[index++] = web_admin_control_page_button2_;
				web_admin_control_page_elements_[index++] = endDiv_;
			web_admin_control_page_elements_[index++] = startThreeColumnDiv_;
				web_admin_control_page_elements_[index++] = web_admin_control_page_static4_;
					web_admin_control_page_elements_[index++] = web_admin_control_page_button3_;
				web_admin_control_page_elements_[index++] = endDiv_;
		web_admin_control_page_elements_[index++] = endDiv_;
	web_admin_control_page_elements_[index++] = endDiv_;
	web_admin_control_page_ = new EmbAJAXPage<22>(web_admin_control_page_elements_, PSTR("Control"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_control_page_, PSTR("/control"), webAdminControlPageCallback);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_control_page_button0_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_control_page_button0_pressed Open"));
	}
	Lock.allow();
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_control_page_button1_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_control_page_button1_pressed Unlock"));
	}
	if(Lock.lock_state_ == Lock.lock_state_option_::normal)	{
		if(Lock.debugStream_ != nullptr)	{
			Lock.debugStream_->println(F("Fixing lock open"));
		}
		Lock.unseal();
	}	else	{
		if(Lock.debugStream_ != nullptr)	{
			Lock.debugStream_->println(F("Setting lock to normal operation"));
		}
		Lock.normal();
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_control_page_button2_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_control_page_button2_pressed Lock"));
	}
	if(Lock.lock_state_ == Lock.lock_state_option_::normal)	{
		if(Lock.debugStream_ != nullptr)	{
			Lock.debugStream_->println(F("Sealing lock"));
		}
		Lock.seal();
	}	else	{
		if(Lock.debugStream_ != nullptr)	{
			Lock.debugStream_->println(F("Setting lock to normal operation"));
		}
		Lock.normal();
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_control_page_button3_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_control_page_button3_pressed Seal"));
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminControlPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminControlPageCallback"));
	}
}
//General settings
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminGeneralSettingsPage()	{
	web_admin_general_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>General settings</h1>"));
	web_admin_general_settings_page_static1_ = new EmbAJAXStatic(PSTR("<label for=\"name\">Lock name</label>"));
	web_admin_general_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"group\">Authorised group (0-255)</label>"));
	web_admin_general_settings_page_text0_ = new EmbAJAXTextInput<64>(PSTR("name"));
	web_admin_general_settings_page_text1_ = new EmbAJAXTextInput<64>(PSTR("group"));
	web_admin_general_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), webAdminGeneralSettingsSaveButtonPressed);
	web_admin_general_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_general_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_general_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_general_settings_page_elements_[index++] = web_admin_general_settings_page_static0_;
		web_admin_general_settings_page_elements_[index++] = startRowDiv_;
			web_admin_general_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_general_settings_page_elements_[index++] = web_admin_general_settings_page_static1_;
				web_admin_general_settings_page_elements_[index++] = web_admin_general_settings_page_text0_;
			web_admin_general_settings_page_elements_[index++] = endDiv_;
			web_admin_general_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_general_settings_page_elements_[index++] = web_admin_general_settings_page_static2_;
				web_admin_general_settings_page_elements_[index++] = web_admin_general_settings_page_text1_;
			web_admin_general_settings_page_elements_[index++] = endDiv_;
		web_admin_general_settings_page_elements_[index++] = endDiv_;
		web_admin_general_settings_page_elements_[index++] = startRowDiv_;
			web_admin_general_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_general_settings_page_elements_[index++] = nbsp_;
			web_admin_general_settings_page_elements_[index++] = endDiv_;
			web_admin_general_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_general_settings_page_elements_[index++] = web_admin_general_settings_page_save_button_;
			web_admin_general_settings_page_elements_[index++] = endDiv_;
		web_admin_general_settings_page_elements_[index++] = endDiv_;
	web_admin_general_settings_page_elements_[index++] = endDiv_;
	web_admin_general_settings_page_ = new EmbAJAXPage<22>(web_admin_general_settings_page_elements_, PSTR("General"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_general_settings_page_, PSTR("/general"), webAdminGeneralSettingsPageCallback);
	web_admin_general_settings_page_text0_->setValue(lock_name_.c_str());
	web_admin_general_settings_page_text1_->setValue(String(lock_access_group_).c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminGeneralSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminGeneralSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_general_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminGeneralSettingsSaveButtonPressed(EmbAJAXPushButton*)	{
	if(Lock.configuration_changed_ == true)	{
		if(Lock.debugStream_ != nullptr)	{
			Lock.debugStream_->println(F("webAdminGeneralSettingsSaveButtonPressed saving configuration"));
		}
		Lock.lock_name_ = String(Lock.web_admin_general_settings_page_text0_->value());
		Lock.lock_access_group_ = String(Lock.web_admin_general_settings_page_text1_->value()).toInt();
		Lock.save_configuration_soon_ = millis();
	}
}
//Wifi client
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminWiFiClientSettingsPage()	{
	web_admin_wifi_client_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>WiFi client settings</h1>"));
	web_admin_wifi_client_settings_page_static1_ = new EmbAJAXStatic(PSTR("WiFi client enabled"));
	web_admin_wifi_client_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"ssid\">SSID</label>"));
	web_admin_wifi_client_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"psk\">PSK</label>"));
	web_admin_wifi_client_settings_page_static4_ = new EmbAJAXStatic(PSTR("NTP client enabled"));
	web_admin_wifi_client_settings_page_static5_ = new EmbAJAXStatic(PSTR("<label for=\"ntpserver\">NTP server</label>"));
	web_admin_wifi_client_settings_page_static6_ = new EmbAJAXStatic(PSTR("<label for=\"tz\">Timezone</label>"));
	web_admin_wifi_client_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_wifi_client_settings_page_check1_ = new EmbAJAXCheckButton(PSTR("check1"));
	web_admin_wifi_client_settings_page_text0_ = new EmbAJAXTextInput<64>(PSTR("ssid"));
	web_admin_wifi_client_settings_page_text1_ = new EmbAJAXTextInput<64>(PSTR("psk"));
	web_admin_wifi_client_settings_page_text2_ = new EmbAJAXTextInput<256>(PSTR("ntpserver"));
	web_admin_wifi_client_settings_page_text3_ = new EmbAJAXTextInput<256>(PSTR("tz"));
	web_admin_wifi_client_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), web_admin_wifi_client_settings_page_save_button_pressed);
	web_admin_wifi_client_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_wifi_client_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static0_;
		web_admin_wifi_client_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_wifi_client_settings_page_elements_[index++] = startLabel_;
					web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_check0_;
					web_admin_wifi_client_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static1_;
					web_admin_wifi_client_settings_page_elements_[index++] = endSpan_;
				web_admin_wifi_client_settings_page_elements_[index++] = endLabel_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static2_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_text0_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static3_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_text1_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_wifi_client_settings_page_elements_[index++] = startLabel_;
					web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_check1_;
					web_admin_wifi_client_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static4_;
					web_admin_wifi_client_settings_page_elements_[index++] = endSpan_;
				web_admin_wifi_client_settings_page_elements_[index++] = endLabel_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static5_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_text2_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_static6_;
				web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_text3_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startTenColumnDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = nbsp_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = startTwoColumnDiv_;
			web_admin_wifi_client_settings_page_elements_[index++] = web_admin_wifi_client_settings_page_save_button_;
			web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
	web_admin_wifi_client_settings_page_elements_[index++] = endDiv_;
	web_admin_wifi_client_settings_page_ = new EmbAJAXPage<52>(web_admin_wifi_client_settings_page_elements_, PSTR("WiFi client"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_wifi_client_settings_page_, PSTR("/wificlient"), webAdminWiFiClientSettingsPageCallback);
	web_admin_wifi_client_settings_page_check0_->setChecked(wifi_client_enabled_);
	web_admin_wifi_client_settings_page_text0_->setValue(wifi_client_ssid_.c_str());
	web_admin_wifi_client_settings_page_text1_->setValue(wifi_client_psk_.c_str());
	web_admin_wifi_client_settings_page_check1_->setChecked(ntp_client_enabled_);
	web_admin_wifi_client_settings_page_text2_->setValue(ntp_server_.c_str());
	web_admin_wifi_client_settings_page_text3_->setValue(ntp_timezone_.c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminWiFiClientSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminWiFiClientSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_wifi_client_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_wifi_client_settings_page_save_button_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_wifi_client_settings_page_save_button_pressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.wifi_client_enabled_ = Lock.web_admin_wifi_client_settings_page_check0_->isChecked();
		Lock.wifi_client_ssid_ = String(Lock.web_admin_wifi_client_settings_page_text0_->value());
		Lock.wifi_client_psk_ = String(Lock.web_admin_wifi_client_settings_page_text1_->value());
		Lock.ntp_client_enabled_ = Lock.web_admin_wifi_client_settings_page_check1_->isChecked();
		Lock.ntp_server_ = String(Lock.web_admin_wifi_client_settings_page_text2_->value());
		Lock.ntp_timezone_ = String(Lock.web_admin_wifi_client_settings_page_text3_->value());
		Lock.save_configuration_soon_ = millis();
	}
}
//Wifi AP
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminWiFiAPSettingsPage()	{
	web_admin_wifi_ap_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>WiFi AP settings</h1>"));
	web_admin_wifi_ap_settings_page_static1_ = new EmbAJAXStatic(PSTR("WiFi AP enabled"));
	web_admin_wifi_ap_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"ssid\">SSID</label>"));
	web_admin_wifi_ap_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"psk\">PSK</label>"));
	web_admin_wifi_ap_settings_page_static4_ = new EmbAJAXStatic(PSTR("WiFi AP hidden"));
	web_admin_wifi_ap_settings_page_static5_ = new EmbAJAXStatic(PSTR("Captive portal"));
	web_admin_wifi_ap_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_wifi_ap_settings_page_check1_ = new EmbAJAXCheckButton(PSTR("check1"));
	web_admin_wifi_ap_settings_page_check2_ = new EmbAJAXCheckButton(PSTR("check2"));
	web_admin_wifi_ap_settings_page_text0_ = new EmbAJAXTextInput<64>(PSTR("ssid"));
	web_admin_wifi_ap_settings_page_text1_ = new EmbAJAXTextInput<64>(PSTR("psk"));
	web_admin_wifi_ap_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), web_admin_wifi_ap_settings_page_save_button_pressed);
	web_admin_wifi_ap_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_wifi_ap_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_static0_;
		web_admin_wifi_ap_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_wifi_ap_settings_page_elements_[index++] = startLabel_;
					web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_check0_;
					web_admin_wifi_ap_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_static1_;
					web_admin_wifi_ap_settings_page_elements_[index++] = endSpan_;
				web_admin_wifi_ap_settings_page_elements_[index++] = endLabel_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_wifi_ap_settings_page_elements_[index++] = startLabel_;
					web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_check2_;
					web_admin_wifi_ap_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_static5_;
					web_admin_wifi_ap_settings_page_elements_[index++] = endSpan_;
				web_admin_wifi_ap_settings_page_elements_[index++] = endLabel_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_wifi_ap_settings_page_elements_[index++] = startLabel_;
					web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_check1_;
					web_admin_wifi_ap_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_static4_;
					web_admin_wifi_ap_settings_page_elements_[index++] = endSpan_;
				web_admin_wifi_ap_settings_page_elements_[index++] = endLabel_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_static2_;
				web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_text0_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_static3_;
				web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_text1_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = startRowDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startTenColumnDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = nbsp_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = startTwoColumnDiv_;
			web_admin_wifi_ap_settings_page_elements_[index++] = web_admin_wifi_ap_settings_page_save_button_;
			web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
		web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
	web_admin_wifi_ap_settings_page_elements_[index++] = endDiv_;
	web_admin_wifi_ap_settings_page_ = new EmbAJAXPage<50>(web_admin_wifi_ap_settings_page_elements_, PSTR("WiFi AP"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_wifi_ap_settings_page_, PSTR("/wifiap"), webAdminWiFiAPSettingsPageCallback);
	web_admin_wifi_ap_settings_page_check0_->setChecked(wifi_ap_enabled_);
	web_admin_wifi_ap_settings_page_check1_->setChecked(wifi_ap_hidden_);
	web_admin_wifi_ap_settings_page_check2_->setChecked(wifi_ap_captive_portal_);
	web_admin_wifi_ap_settings_page_text0_->setValue(wifi_ap_ssid_.c_str());
	web_admin_wifi_ap_settings_page_text1_->setValue(wifi_ap_psk_.c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminWiFiAPSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminWiFiAPSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_wifi_ap_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_wifi_ap_settings_page_save_button_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_wifi_ap_settings_page_save_button_pressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.wifi_ap_enabled_ = Lock.web_admin_wifi_ap_settings_page_check0_->isChecked();
		Lock.wifi_ap_hidden_ = Lock.web_admin_wifi_ap_settings_page_check1_->isChecked();
		Lock.wifi_ap_captive_portal_ = Lock.web_admin_wifi_ap_settings_page_check2_->isChecked();
		Lock.wifi_ap_ssid_ = String(Lock.web_admin_wifi_ap_settings_page_text0_->value());
		Lock.wifi_ap_psk_ = String(Lock.web_admin_wifi_ap_settings_page_text1_->value());
		Lock.save_configuration_soon_ = millis();
	}
}
//M2mMesh
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminM2mMeshSettingsPage()	{
	web_admin_m2mMesh_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>Mesh network settings</h1>"));
	web_admin_m2mMesh_settings_page_static1_ = new EmbAJAXStatic(PSTR("Mesh network enabled"));
	web_admin_m2mMesh_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"channel\">Mesh network channel</label>"));
	web_admin_m2mMesh_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"meshid\">Mesh network ID (0-255)</label>"));
	web_admin_m2mMesh_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_m2mMesh_settings_page_select0_options_[0] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[0], "1", 2);
	web_admin_m2mMesh_settings_page_select0_options_[1] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[1], "2", 2);
	web_admin_m2mMesh_settings_page_select0_options_[2] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[2], "3", 2);
	web_admin_m2mMesh_settings_page_select0_options_[3] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[3], "4", 2);
	web_admin_m2mMesh_settings_page_select0_options_[4] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[4], "5", 2);
	web_admin_m2mMesh_settings_page_select0_options_[5] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[5], "6", 2);
	web_admin_m2mMesh_settings_page_select0_options_[6] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[6], "7", 2);
	web_admin_m2mMesh_settings_page_select0_options_[7] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[7], "8", 2);
	web_admin_m2mMesh_settings_page_select0_options_[8] = new char[2];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[8], "9", 2);
	web_admin_m2mMesh_settings_page_select0_options_[9] = new char[3];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[9], "10", 3);
	web_admin_m2mMesh_settings_page_select0_options_[10] = new char[3];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[10], "11", 3);
	web_admin_m2mMesh_settings_page_select0_options_[11] = new char[3];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[11], "12", 3);
	web_admin_m2mMesh_settings_page_select0_options_[12] = new char[3];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[12], "13", 3);
	web_admin_m2mMesh_settings_page_select0_options_[13] = new char[3];
	strlcpy(web_admin_m2mMesh_settings_page_select0_options_[13], "14", 3);
	web_admin_m2mMesh_settings_page_select0_ = new EmbAJAXOptionSelect<14>("channel", (const char**)web_admin_m2mMesh_settings_page_select0_options_);
	web_admin_m2mMesh_settings_page_text0_ = new EmbAJAXTextInput<64>(PSTR("meshid"));
	web_admin_m2mMesh_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), web_admin_m2mMesh_settings_page_save_button_pressed);
	web_admin_m2mMesh_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_m2mMesh_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_static0_;
		web_admin_m2mMesh_settings_page_elements_[index++] = startRowDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_m2mMesh_settings_page_elements_[index++] = startLabel_;
					web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_check0_;
					web_admin_m2mMesh_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_static1_;
					web_admin_m2mMesh_settings_page_elements_[index++] = endSpan_;
				web_admin_m2mMesh_settings_page_elements_[index++] = endLabel_;
			web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
		web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
		web_admin_m2mMesh_settings_page_elements_[index++] = startRowDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_static2_;
				web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_select0_;
			web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = startSixColumnDiv_;
				web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_static3_;
				web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_text0_;
			web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
		web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
		web_admin_m2mMesh_settings_page_elements_[index++] = startRowDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = startTenColumnDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = nbsp_;
			web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = startTwoColumnDiv_;
			web_admin_m2mMesh_settings_page_elements_[index++] = web_admin_m2mMesh_settings_page_save_button_;
			web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
		web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
	web_admin_m2mMesh_settings_page_elements_[index++] = endDiv_;
	web_admin_m2mMesh_settings_page_ = new EmbAJAXPage<32>(web_admin_m2mMesh_settings_page_elements_, PSTR("WiFi Mesh"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_m2mMesh_settings_page_, PSTR("/mesh"), webAdminM2mMeshSettingsPageCallback);
	web_admin_m2mMesh_settings_page_check0_->setChecked(mesh_network_enabled_);
	web_admin_m2mMesh_settings_page_select0_->selectOption(mesh_network_channel_-1);
	web_admin_m2mMesh_settings_page_text0_->setValue(String(mesh_network_id_).c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminM2mMeshSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminM2mMeshSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_m2mMesh_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_m2mMesh_settings_page_save_button_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_m2mMesh_settings_page_save_button_pressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.mesh_network_enabled_ = Lock.web_admin_m2mMesh_settings_page_check0_->isChecked();
		Lock.mesh_network_channel_ = Lock.web_admin_m2mMesh_settings_page_select0_->selectedOption() + 1;
		Lock.mesh_network_id_ = String(Lock.web_admin_m2mMesh_settings_page_text0_->value()).toInt();
		Lock.save_configuration_soon_ = millis();
	}
}
//PINs
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminPINSettingsPage()	{
	web_admin_pin_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>PIN settings</h1>"));
	web_admin_pin_settings_page_static1_ = new EmbAJAXStatic(PSTR("PIN entry enabled"));
	web_admin_pin_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"open\">Open (opens the door once)</label>"));
	web_admin_pin_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"seal\">Seal (locks the door so it won't open)</label>"));
	web_admin_pin_settings_page_static4_ = new EmbAJAXStatic(PSTR("<label for=\"unseal\">Unseal (unlocks the door so it stays open)</label>"));
	web_admin_pin_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_pin_settings_page_text0_ = new EmbAJAXTextInput<abs_max_pin_length_>(PSTR("open"));
	web_admin_pin_settings_page_text1_ = new EmbAJAXTextInput<abs_max_pin_length_>(PSTR("seal"));
	web_admin_pin_settings_page_text2_ = new EmbAJAXTextInput<abs_max_pin_length_>(PSTR("unseal"));
	web_admin_pin_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), web_admin_pin_settings_page_save_button_pressed);
	web_admin_pin_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_pin_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_pin_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_static0_;
		web_admin_pin_settings_page_elements_[index++] = startRowDiv_;
			web_admin_pin_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_pin_settings_page_elements_[index++] = startLabel_;
					web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_check0_;
					web_admin_pin_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_static1_;
					web_admin_pin_settings_page_elements_[index++] = endSpan_;
				web_admin_pin_settings_page_elements_[index++] = endLabel_;
			web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = startRowDiv_;
			web_admin_pin_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_static2_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_text0_;
			web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = startRowDiv_;
			web_admin_pin_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_static3_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_text1_;
			web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = startRowDiv_;
			web_admin_pin_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_static4_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_text2_;
			web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = startRowDiv_;
			web_admin_pin_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_pin_settings_page_elements_[index++] = nbsp_;
			web_admin_pin_settings_page_elements_[index++] = endDiv_;
				web_admin_pin_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_pin_settings_page_elements_[index++] = web_admin_pin_settings_page_save_button_;
			web_admin_pin_settings_page_elements_[index++] = endDiv_;
		web_admin_pin_settings_page_elements_[index++] = endDiv_;
	web_admin_pin_settings_page_elements_[index++] = endDiv_;
	web_admin_pin_settings_page_ = new EmbAJAXPage<40>(web_admin_pin_settings_page_elements_, PSTR("PIN settings"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_pin_settings_page_, PSTR("/pins"), webAdminPINSettingsPageCallback);
	web_admin_pin_settings_page_check0_->setChecked(pin_entry_enabled_);
	web_admin_pin_settings_page_text0_->setValue(pin_to_open_.c_str());
	web_admin_pin_settings_page_text1_->setValue(pin_to_seal_.c_str());
	web_admin_pin_settings_page_text2_->setValue(pin_to_unseal_.c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminPINSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminPINSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_pin_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_pin_settings_page_save_button_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_pin_settings_page_save_button_pressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.pin_entry_enabled_ = Lock.web_admin_pin_settings_page_check0_->isChecked();
		Lock.pin_to_open_ = String(Lock.web_admin_pin_settings_page_text0_->value());
		Lock.pin_to_seal_ = String(Lock.web_admin_pin_settings_page_text1_->value());
		Lock.pin_to_unseal_ = String(Lock.web_admin_pin_settings_page_text2_->value());
		Lock.save_configuration_soon_ = millis();
	}
}
//Tap codes
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminTapCodeSettingsPage()	{
	web_admin_tap_code_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>Tap code settings</h1>"));
	web_admin_tap_code_settings_page_static1_ = new EmbAJAXStatic(PSTR("Tap code entry enabled"));
	web_admin_tap_code_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"open\">Open (opens the door once)</label>"));
	web_admin_tap_code_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"seal\">Seal (locks the door so it won't open)</label>"));
	web_admin_tap_code_settings_page_static4_ = new EmbAJAXStatic(PSTR("<label for=\"unseal\">Unseal (unlocks the door so it stays open)</label>"));
	web_admin_tap_code_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_tap_code_settings_page_text0_ = new EmbAJAXTextInput<abs_max_tap_code_length_>(PSTR("open"));
	web_admin_tap_code_settings_page_text1_ = new EmbAJAXTextInput<abs_max_tap_code_length_>(PSTR("seal"));
	web_admin_tap_code_settings_page_text2_ = new EmbAJAXTextInput<abs_max_tap_code_length_>(PSTR("unseal"));
	web_admin_tap_code_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), webAdminTapCodeSettingsPageSaveButtonPressed);
	web_admin_tap_code_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_tap_code_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_static0_;
		web_admin_tap_code_settings_page_elements_[index++] = startRowDiv_;
			web_admin_tap_code_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = startLabel_;
					web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_check0_;
					web_admin_tap_code_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_static1_;
					web_admin_tap_code_settings_page_elements_[index++] = endSpan_;
				web_admin_tap_code_settings_page_elements_[index++] = endLabel_;
			web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = startRowDiv_;
			web_admin_tap_code_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_static2_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_text0_;
			web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = startRowDiv_;
			web_admin_tap_code_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_static3_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_text1_;
			web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = startRowDiv_;
			web_admin_tap_code_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_static4_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_text2_;
			web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = startRowDiv_;
			web_admin_tap_code_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = nbsp_;
			web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_tap_code_settings_page_elements_[index++] = web_admin_tap_code_settings_page_save_button_;
			web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
		web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
	web_admin_tap_code_settings_page_elements_[index++] = endDiv_;
	web_admin_tap_code_settings_page_ = new EmbAJAXPage<40>(web_admin_tap_code_settings_page_elements_, PSTR("Tap code settings"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_tap_code_settings_page_, PSTR("/tapcodes"), webAdminTapCodeSettingsPageCallback);
	web_admin_tap_code_settings_page_check0_->setChecked(tap_code_enabled_);
	web_admin_tap_code_settings_page_text0_->setValue(tap_code_open_.c_str());
	web_admin_tap_code_settings_page_text1_->setValue(tap_code_seal_.c_str());
	web_admin_tap_code_settings_page_text2_->setValue(tap_code_unseal_.c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminTapCodeSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminPINSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_tap_code_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminTapCodeSettingsPageSaveButtonPressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminTapCodeSettingsPageSaveButtonPressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.tap_code_enabled_ = Lock.web_admin_tap_code_settings_page_check0_->isChecked();
		Lock.tap_code_open_ = String(Lock.web_admin_tap_code_settings_page_text0_->value());
		Lock.tap_code_seal_ = String(Lock.web_admin_tap_code_settings_page_text1_->value());
		Lock.tap_code_unseal_ = String(Lock.web_admin_tap_code_settings_page_text2_->value());
		Lock.save_configuration_soon_ = millis();
	}
}
//RFID
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminRFIDPage()	{
	web_admin_rfid_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>RFID provisioning</h1>"));
	web_admin_rfid_settings_page_static1_ = new EmbAJAXStatic(PSTR("<p>These buttons will change the access for the NEXT card presented at the lock. This can be used for provisioning cards for this lock. Note the authorisation applies to all devices in the same group as this lock.</p>"));
	web_admin_rfid_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"button0\">Authorise next presented card</label>"));
	web_admin_rfid_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"button1\">revoke authorisation for next presented card</label>"));
	web_admin_rfid_settings_page_static4_ = new EmbAJAXStatic(PSTR("<label for=\"button2\">Wipe ALL authorisations from next presented card</label>"));
	web_admin_rfid_settings_page_static5_ = new EmbAJAXStatic(PSTR("RFID card entry enabled"));
	web_admin_rfid_settings_page_button0_ = new EmbAJAXPushButton(PSTR("button0"), PSTR("Authorise"), web_admin_rfid_settings_page_button0_pressed);
	web_admin_rfid_settings_page_button1_ = new EmbAJAXPushButton(PSTR("button1"), PSTR("Revoke"), web_admin_rfid_settings_page_button1_pressed);
	web_admin_rfid_settings_page_button2_ = new EmbAJAXPushButton(PSTR("button2"), PSTR("Wipe ALL"), web_admin_rfid_settings_page_button2_pressed);
	web_admin_rfid_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_rfid_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), webAdminRFIDSettingsPageSaveButtonPressed);
	web_admin_rfid_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_rfid_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_rfid_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_static0_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = startLabel_;
					web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_check0_;
					web_admin_rfid_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_static5_;
					web_admin_rfid_settings_page_elements_[index++] = endSpan_;
				web_admin_rfid_settings_page_elements_[index++] = endLabel_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_static1_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_static2_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_button0_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = nbsp_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_static3_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_button1_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = nbsp_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_static4_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_button2_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = nbsp_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = startRowDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = nbsp_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
			web_admin_rfid_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_rfid_settings_page_elements_[index++] = web_admin_rfid_settings_page_save_button_;
			web_admin_rfid_settings_page_elements_[index++] = endDiv_;
		web_admin_rfid_settings_page_elements_[index++] = endDiv_;
	web_admin_rfid_settings_page_elements_[index++] = endDiv_;
	web_admin_rfid_settings_page_ = new EmbAJAXPage<66>(web_admin_rfid_settings_page_elements_, PSTR("RFID"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_rfid_settings_page_, PSTR("/rfid"), webAdminRFIDPageCallback);
	web_admin_rfid_settings_page_check0_->setChecked(rfid_authorisation_enabled_);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_rfid_settings_page_button0_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_rfid_settings_page_button0_pressed authorise"));
	}
	Lock.rfidReaderState = Lock.RFID_READER_ALLOW_NEXT_CARD;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_rfid_settings_page_button1_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_rfid_settings_page_button1_pressed revoke"));
	}
	Lock.rfidReaderState = Lock.RFID_READER_DENY_NEXT_CARD;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::web_admin_rfid_settings_page_button2_pressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("web_admin_rfid_settings_page_button2_pressed wipe"));
	}
	Lock.rfidReaderState = Lock.RFID_READER_WIPE_NEXT_CARD;
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminRFIDPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminRFIDPageCallback"));
	}
	if(Lock.web_admin_rfid_settings_page_check0_->isChecked() != Lock.rfid_authorisation_enabled_)	{
		Lock.configuration_changed_ = true;
		Lock.web_admin_rfid_settings_page_save_button_->setVisible(true);
	}
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminRFIDSettingsPageSaveButtonPressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminRFIDSettingsPageSaveButtonPressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.rfid_authorisation_enabled_ = Lock.web_admin_rfid_settings_page_check0_->isChecked();
		Lock.save_configuration_soon_ = millis();
	}
}
//MQTT client
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminMQTTClientSettingsPage()	{
	web_admin_mqtt_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>MQTT client settings</h1>"));
	web_admin_mqtt_settings_page_static1_ = new EmbAJAXStatic(PSTR("MQTT client enabled"));
	web_admin_mqtt_settings_page_static2_ = new EmbAJAXStatic(PSTR("<label for=\"host\">MQTT host</label>"));
	web_admin_mqtt_settings_page_static3_ = new EmbAJAXStatic(PSTR("<label for=\"port\">MQTT port</label>"));
	web_admin_mqtt_settings_page_static4_ = new EmbAJAXStatic(PSTR("<label for=\"client\">MQTT client ID</label>"));
	web_admin_mqtt_settings_page_static5_ = new EmbAJAXStatic(PSTR("<label for=\"username\">MQTT username</label>"));
	web_admin_mqtt_settings_page_static6_ = new EmbAJAXStatic(PSTR("<label for=\"password\">MQTT password</label>"));
	web_admin_mqtt_settings_page_static7_ = new EmbAJAXStatic(PSTR("<label for=\"topic\">MQTT topic</label>"));
	web_admin_mqtt_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_mqtt_settings_page_text0_ = new EmbAJAXTextInput<256>(PSTR("host"));
	web_admin_mqtt_settings_page_text1_ = new EmbAJAXTextInput<256>(PSTR("port"));
	web_admin_mqtt_settings_page_text2_ = new EmbAJAXTextInput<256>(PSTR("client"));
	web_admin_mqtt_settings_page_text3_ = new EmbAJAXTextInput<256>(PSTR("username"));
	web_admin_mqtt_settings_page_text4_ = new EmbAJAXTextInput<256>(PSTR("password"));
	web_admin_mqtt_settings_page_text5_ = new EmbAJAXTextInput<256>(PSTR("topic"));
	web_admin_mqtt_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), webAdminMQTTSettingsPageSaveButtonPressed);
	web_admin_mqtt_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_mqtt_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static0_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = startLabel_;
					web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_check0_;
					web_admin_mqtt_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static1_;
					web_admin_mqtt_settings_page_elements_[index++] = endSpan_;
				web_admin_mqtt_settings_page_elements_[index++] = endLabel_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static2_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_text0_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static3_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_text1_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static4_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_text2_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static5_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_text3_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static6_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_text4_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_static7_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_text5_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = startRowDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = nbsp_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
			web_admin_mqtt_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_mqtt_settings_page_elements_[index++] = web_admin_mqtt_settings_page_save_button_;
			web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
		web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
	web_admin_mqtt_settings_page_elements_[index++] = endDiv_;
	web_admin_mqtt_settings_page_ = new EmbAJAXPage<58>(web_admin_mqtt_settings_page_elements_, PSTR("MQTT client"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_mqtt_settings_page_, PSTR("/mqtt"), webAdminMQTTSettingsPageCallback);
	web_admin_mqtt_settings_page_check0_->setChecked(mqtt_enabled_);
	web_admin_mqtt_settings_page_text0_->setValue(mqtt_host_.c_str());
	web_admin_mqtt_settings_page_text1_->setValue(mqtt_port_.c_str());
	web_admin_mqtt_settings_page_text2_->setValue(mqtt_client_id_.c_str());
	web_admin_mqtt_settings_page_text3_->setValue(mqtt_username_.c_str());
	web_admin_mqtt_settings_page_text4_->setValue(mqtt_password_.c_str());
	web_admin_mqtt_settings_page_text5_->setValue(mqtt_topic_.c_str());
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminMQTTSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminMQTTSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_mqtt_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminMQTTSettingsPageSaveButtonPressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminMQTTSettingsPageSaveButtonPressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.mqtt_enabled_ = Lock.web_admin_mqtt_settings_page_check0_->isChecked();
		Lock.mqtt_host_ = String(Lock.web_admin_mqtt_settings_page_text0_->value());
		Lock.mqtt_port_ = String(Lock.web_admin_mqtt_settings_page_text1_->value());
		Lock.mqtt_client_id_ = String(Lock.web_admin_mqtt_settings_page_text2_->value());
		Lock.mqtt_username_ = String(Lock.web_admin_mqtt_settings_page_text3_->value());
		Lock.mqtt_password_ = String(Lock.web_admin_mqtt_settings_page_text4_->value());
		Lock.mqtt_topic_ = String(Lock.web_admin_mqtt_settings_page_text5_->value());
		Lock.save_configuration_soon_ = millis();
	}
}
//Hacking games
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminGameSettingsPage()	{
	web_admin_game_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>Hacking game</h1>"));
	web_admin_game_settings_page_static1_ = new EmbAJAXStatic(PSTR("Hacking game enabled"));
	web_admin_game_settings_page_static2_ = new EmbAJAXStatic(PSTR("Game type"));
	web_admin_game_settings_page_static3_ = new EmbAJAXStatic(PSTR("Game length"));
	web_admin_game_settings_page_static4_ = new EmbAJAXStatic(PSTR("Game retries (0=infinite)"));
	//Game enabled
	web_admin_game_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	//Type
	web_admin_game_settings_page_radio0_options_[0] = new char[13];
	strlcpy(web_admin_game_settings_page_radio0_options_[0], "Whack-a-mole", 13);
	web_admin_game_settings_page_radio0_options_[1] = new char[11];
	strlcpy(web_admin_game_settings_page_radio0_options_[1], "Simon says", 11);
	web_admin_game_settings_page_radio0_ = new EmbAJAXRadioGroup<2>(PSTR("radio0"), (const char**)web_admin_game_settings_page_radio0_options_);
	//Game length
	if(game_type_ == 0) {
		web_admin_game_settings_page_slider0_ = new EmbAJAXSlider(PSTR("slider0"),3,50,game_length_);	//Whack a mole
	} else {
		web_admin_game_settings_page_slider0_ = new EmbAJAXSlider(PSTR("slider0"),3,10,game_length_);	//Simon
	}
	//Game retries
	web_admin_game_settings_page_slider1_ = new EmbAJAXSlider(PSTR("slider1"),0,10,game_retries_);
	//Save
	web_admin_game_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), webAdminGameSettingsPageSaveButtonPressed);
	web_admin_game_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_game_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_game_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_static0_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = startLabel_;
					web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_check0_;
					web_admin_game_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_static1_;
					web_admin_game_settings_page_elements_[index++] = endSpan_;
				web_admin_game_settings_page_elements_[index++] = endLabel_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_static2_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_radio0_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_static3_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_slider0_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_static4_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_slider1_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = startRowDiv_;
			web_admin_game_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = nbsp_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
			web_admin_game_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_game_settings_page_elements_[index++] = web_admin_game_settings_page_save_button_;
			web_admin_game_settings_page_elements_[index++] = endDiv_;
		web_admin_game_settings_page_elements_[index++] = endDiv_;
	web_admin_game_settings_page_elements_[index++] = endDiv_;
	web_admin_game_settings_page_ = new EmbAJAXPage<web_admin_game_settings_page_element_count_>(web_admin_game_settings_page_elements_, PSTR("Hacking game"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_game_settings_page_, PSTR("/game"), webAdminGameSettingsPageCallback);
	web_admin_game_settings_page_check0_->setChecked(game_enabled_);
	web_admin_game_settings_page_radio0_->selectOption(game_type_);
	web_admin_game_settings_page_slider0_->setValue(game_length_);
	web_admin_game_settings_page_slider1_->setValue(game_retries_);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminGameSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminGameSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_game_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminGameSettingsPageSaveButtonPressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminGameSettingsPageSaveButtonPressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.game_enabled_ = Lock.web_admin_game_settings_page_check0_->isChecked();
		Lock.game_enabled_ = Lock.web_admin_game_settings_page_check0_->isChecked();
		Lock.game_type_ = Lock.web_admin_game_settings_page_radio0_->selectedOption();
		Lock.game_length_ = Lock.web_admin_game_settings_page_slider0_->intValue ();
		Lock.game_retries_ = Lock.web_admin_game_settings_page_slider1_->intValue ();
		Lock.save_configuration_soon_ = millis();
	}
}
//Multi-factor authorisation
void ICACHE_FLASH_ATTR LarpHackableRfidLock::createWebAdminTwoFactorSettingsPage()	{
	web_admin_multi_factor_settings_page_static0_ = new EmbAJAXStatic(PSTR("<h1>Multi-factor authentication</h1>"));
	web_admin_multi_factor_settings_page_static1_ = new EmbAJAXStatic(PSTR("Multi-factor authentication enabled"));
	web_admin_multi_factor_settings_page_static2_ = new EmbAJAXStatic(PSTR("Authentication type"));
	web_admin_multi_factor_settings_page_static3_ = new EmbAJAXStatic(PSTR("Partner type"));
	web_admin_multi_factor_settings_page_static4_ = new EmbAJAXStatic(PSTR("<label for=\"partnerName\">Partner name</label>"));
	web_admin_multi_factor_settings_page_static5_ = new EmbAJAXStatic(PSTR("<label for=\"timeout\">Multi-factor timeout(s)</label>"));
	web_admin_multi_factor_settings_page_check0_ = new EmbAJAXCheckButton(PSTR("check0"));
	web_admin_multi_factor_settings_page_radio0_options_[0] = new char[13];
	strlcpy(web_admin_multi_factor_settings_page_radio0_options_[0], "Partner only", 13);
	web_admin_multi_factor_settings_page_radio0_options_[1] = new char[19];
	strlcpy(web_admin_multi_factor_settings_page_radio0_options_[1], "Button and partner", 19);
	web_admin_multi_factor_settings_page_radio0_options_[2] = new char[11];
	strlcpy(web_admin_multi_factor_settings_page_radio0_options_[2], "Card & pin", 11);
	web_admin_multi_factor_settings_page_radio0_options_[3] = new char[15];
	strlcpy(web_admin_multi_factor_settings_page_radio0_options_[3], "Card & partner", 15);
	web_admin_multi_factor_settings_page_radio0_options_[4] = new char[14];
	strlcpy(web_admin_multi_factor_settings_page_radio0_options_[4], "PIN & partner", 14);
	web_admin_multi_factor_settings_page_radio0_options_[5] = new char[20];
	strlcpy(web_admin_multi_factor_settings_page_radio0_options_[5], "Card, pin & partner", 20);
	web_admin_multi_factor_settings_page_radio0_ = new EmbAJAXRadioGroup<6>(PSTR("radio0"), (const char**)web_admin_multi_factor_settings_page_radio0_options_);
	web_admin_multi_factor_settings_page_radio1_options_[0] = new char[5];
	strlcpy(web_admin_multi_factor_settings_page_radio1_options_[0], "MQTT", 5);
	web_admin_multi_factor_settings_page_radio1_options_[1] = new char[13];
	strlcpy(web_admin_multi_factor_settings_page_radio1_options_[1], "Mesh network", 13);
	web_admin_multi_factor_settings_page_radio1_ = new EmbAJAXRadioGroup<2>(PSTR("radio1"), (const char**)web_admin_multi_factor_settings_page_radio1_options_);
	web_admin_multi_factor_settings_page_text0_ = new EmbAJAXTextInput<256>(PSTR("partnerName"));
	web_admin_multi_factor_settings_page_text1_ = new EmbAJAXTextInput<256>(PSTR("timeout"));
	web_admin_multi_factor_settings_page_save_button_ = new EmbAJAXPushButton(PSTR("save"), PSTR("Save"), webAdminTwoFactorSettingsPageSaveButtonPressed);
	web_admin_multi_factor_settings_page_save_button_->setVisible(false);
	uint8_t index = 0;
	web_admin_multi_factor_settings_page_elements_[index++] = startContainerDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = web_admin_toolbar_;
		web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_static0_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = startLabel_;
					web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_check0_;
					web_admin_multi_factor_settings_page_elements_[index++] = startLabelSpan_;
						web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_static1_;
					web_admin_multi_factor_settings_page_elements_[index++] = endSpan_;
				web_admin_multi_factor_settings_page_elements_[index++] = endLabel_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_static2_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_radio0_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_static3_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_radio1_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_static4_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_text0_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwelveColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_static5_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_text1_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = startRowDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTenColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = nbsp_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
			web_admin_multi_factor_settings_page_elements_[index++] = startTwoColumnDiv_;
				web_admin_multi_factor_settings_page_elements_[index++] = web_admin_multi_factor_settings_page_save_button_;
			web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
		web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
	web_admin_multi_factor_settings_page_elements_[index++] = endDiv_;
	web_admin_multi_factor_settings_page_ = new EmbAJAXPage<54>(web_admin_multi_factor_settings_page_elements_, PSTR("Multi-factor authorisation"), web_admin_header_includes_);
	webAdminInterface_->installPage(web_admin_multi_factor_settings_page_, PSTR("/multifactor"), webAdminTwoFactorSettingsPageCallback);
	web_admin_multi_factor_settings_page_check0_->setChecked(multi_factor_enabled_);
	web_admin_multi_factor_settings_page_radio0_->selectOption((int)multi_factor_type_);
	web_admin_multi_factor_settings_page_text0_->setValue(String(multi_factor_partner_name_).c_str());
	web_admin_multi_factor_settings_page_text1_->setValue(String(multi_factor_timeout_/1000).c_str());
	web_admin_multi_factor_settings_page_radio1_->selectOption((int)multi_factor_partner_type_);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminTwoFactorSettingsPageCallback()	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminTwoFactorSettingsPageCallback"));
	}
	Lock.configuration_changed_ = true;
	Lock.web_admin_multi_factor_settings_page_save_button_->setVisible(true);
}
void ICACHE_FLASH_ATTR LarpHackableRfidLock::webAdminTwoFactorSettingsPageSaveButtonPressed(EmbAJAXPushButton*)	{
	if(Lock.debugStream_ != nullptr)	{
		Lock.debugStream_->println(F("webAdminTwoFactorSettingsPageSaveButtonPressed saving configuration"));
	}
	if(Lock.configuration_changed_ == true)	{
		Lock.multi_factor_enabled_ = Lock.web_admin_multi_factor_settings_page_check0_->isChecked();
		Lock.multi_factor_enabled_ = Lock.web_admin_multi_factor_settings_page_check0_->isChecked();
		Lock.multi_factor_type_ = (multiFactorOption)Lock.web_admin_multi_factor_settings_page_radio0_->selectedOption();
		Lock.multi_factor_partner_name_ = String(Lock.web_admin_multi_factor_settings_page_text0_->value());
		Lock.multi_factor_partner_type_ = (multiFactorPartnerOption)Lock.web_admin_multi_factor_settings_page_radio1_->selectedOption();
		Lock.multi_factor_timeout_ = String(Lock.web_admin_multi_factor_settings_page_text1_->value()).toInt() * 1000;
		Lock.save_configuration_soon_ = millis();
	}
}
#endif