/*
 * Example sketch for the lock prop, using Tap Code and/or RFID to control the lock
 * 
 */

#if defined(ESP8266)
  #include <ESPAsyncTCP.h>
#elif(defined ESP32)
  #include <AsyncTCP.h>
  #include "LittleFS.h"
#endif
#include <FS.h>
#include <EmbAJAXOutputDriverESPAsync.h>
#include <EmbAJAX.h>
#include <LarpHackableRfidLock.h>

EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);

//Top of page
void saveConfigPressed(EmbAJAXPushButton*) { Lock.saveConfiguration(); }
#if defined(ESP8266)
  void rebootPressed(EmbAJAXPushButton*) { ESP.reset(); }
#elif(defined ESP32)
  void rebootPressed(EmbAJAXPushButton*) { ESP.restart(); }
#endif
EmbAJAXPushButton saveConfig("saveConfig", "Save configuration", saveConfigPressed);
EmbAJAXPushButton reboot("reboot", "Reboot", rebootPressed);

//Tab 1
void button1Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 1")); }
void button2Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 2")); }
void button3Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 3")); }
void button4Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 4")); }
EmbAJAXPushButton button1("button1", "Open", button1Pressed);
EmbAJAXPushButton button2("button2", "Unlock", button2Pressed);
EmbAJAXPushButton button3("button3", "Lock", button3Pressed);
EmbAJAXPushButton button4("button4", "Seal", button4Pressed);
//Tab 2
void button5Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 5")); }
void button6Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 6")); }
void button7Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 7")); }
void button8Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 8")); }
void button9Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 9")); }
void button10Pressed(EmbAJAXPushButton*) { Serial.println(F("Button 10")); }
EmbAJAXCheckButton check5("check5", "");
EmbAJAXPushButton button5("button5", "Authorise", button5Pressed);
EmbAJAXPushButton button6("button6", "Revoke", button6Pressed);
EmbAJAXPushButton button7("button7", "Wipe", button7Pressed);
EmbAJAXPushButton button8("button8", "Authorise ALL", button8Pressed);
EmbAJAXPushButton button9("button9", "Revoke ALL", button9Pressed);
EmbAJAXPushButton button10("button10", "Wipe ALL", button10Pressed);
//Tab 3
EmbAJAXCheckButton check6("check6", "");
EmbAJAXCheckButton check7("check7", "");
EmbAJAXCheckButton check8("check8", "");
EmbAJAXTextInput<64> text7("tapcodeopen");
EmbAJAXTextInput<64> text8("tapcodeunlock");
EmbAJAXTextInput<64> text9("tapcodelock");
EmbAJAXTextInput<64> text10("tapcodeseal");
EmbAJAXTextInput<64> text11("tapcodeauthorise");
EmbAJAXTextInput<64> text12("tapcoderevoke");
EmbAJAXTextInput<64> text13("tapcodewipe");

//Tab 4
EmbAJAXCheckButton check1("check1", "");
EmbAJAXTextInput<64> text1("ssid");
EmbAJAXTextInput<64> text2("psk");
EmbAJAXCheckButton check2("check2", "");
EmbAJAXTextInput<64> text3("ntpserver");
EmbAJAXTextInput<64> text4("tz");
EmbAJAXCheckButton check3("check3", "");
EmbAJAXCheckButton check4("check4", "");
EmbAJAXTextInput<64> text5("apssid");
EmbAJAXTextInput<64> text6("appsk");

EmbAJAXStatic endDiv("</div>");
EmbAJAXStatic startRowDiv("<div class=\"row\">");
EmbAJAXStatic startSixColumnDiv("<div class=\"six columns\">");
EmbAJAXStatic startTwelveColumnDiv("<div class=\"twelve columns\">");
EmbAJAXStatic startFourColumnDiv("<div class=\"four columns\">");
EmbAJAXStatic startThreeColumnDiv("<div class=\"three columns\">");
EmbAJAXStatic startTwoColumnDiv("<div class=\"two columns\">");
EmbAJAXStatic startLabel("<label>");
EmbAJAXStatic endLabel("</label>");
EmbAJAXStatic startLabelSpan("<span class=\"label-body\">");
EmbAJAXStatic endSpan("</span>");

//#define LEDPIN LED_BUILTIN
//const char* modes[] = {"On", "Blink", "Off"};
//EmbAJAXRadioGroup<3> mode("mode", modes);
//EmbAJAXSlider blinkfreq("blfreq", 0, 1000, 100);   // slider, from 0 to 1000, initial value 100

char headerIncludes[] = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link href=\"//fonts.googleapis.com/css?family=Raleway:400,300,600\" rel=\"stylesheet\" type=\"text/css\"><link rel=\"stylesheet\" href=\"css/normalize.css\"><link rel=\"stylesheet\" href=\"css/skeleton.css\"><link rel=\"icon\" type=\"image/png\" href=\"images/favicon.png\">";

MAKE_EmbAJAXPage(page, "Lock configuration", headerIncludes,
  new EmbAJAXStatic("<div class=\"container\">"),
    new EmbAJAXStatic("<h1>Lock</h1>"),
    //Top of page
    &startRowDiv,
      &startSixColumnDiv,&saveConfig,&endDiv,
      &startSixColumnDiv,&reboot,&endDiv,
    &endDiv,
    //Lock controls
    &startRowDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"button1\">Open door</label>"),&button1,&endDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"button2\">Unlock door</label>"),&button2,&endDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"button3\">Lock door</label>"),&button3,&endDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"button4\">Seal door</label>"),&button4,&endDiv,
    &endDiv,
    //RFID control
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check5,&startLabelSpan,new EmbAJAXStatic("RFID enabled"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"button5\">Authorise next card</label>"),&button5,&endDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"button6\">Revoke next card</label>"),&button6,&endDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"button7\">Revoke ALL next card</label>"),&button7,&endDiv,
    &endDiv,
    &startRowDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"button8\">Authorise</label>"),&button8,&endDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"button9\">Revoke</label>"),&button9,&endDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"button10\">Revoke ALL</label>"),&button10,&endDiv,
    &endDiv,
    //Tap code
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check6,&startLabelSpan,new EmbAJAXStatic("Tap Code enabled"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check7,&startLabelSpan,new EmbAJAXStatic("Tap Code positive feedback"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check8,&startLabelSpan,new EmbAJAXStatic("Tap Code negative feedback"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"text7\">Open</label>"),&text7,&endDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"text8\">Unlock</label>"),&text8,&endDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"text9\">Lock</label>"),&text9,&endDiv,
      &startThreeColumnDiv,new EmbAJAXStatic("<label for=\"text10\">Seal</label>"),&text10,&endDiv,
    &endDiv,
    &startRowDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"text11\">Authorise</label>"),&text11,&endDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"text12\">Revoke</label>"),&text12,&endDiv,
      &startFourColumnDiv,new EmbAJAXStatic("<label for=\"text13\">Wipe</label>"),&text13,&endDiv,
    &endDiv,
    //WiFi client
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check1,&startLabelSpan,new EmbAJAXStatic("WiFi client enabled"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startSixColumnDiv,new EmbAJAXStatic("<label for=\"ssid\">SSID</label>"),&text1,&endDiv,
      &startSixColumnDiv,new EmbAJAXStatic("<label for=\"psk\">PSK</label>"),&text2,&endDiv,
    &endDiv,
    //NTP client
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check2,&startLabelSpan,new EmbAJAXStatic("NTP client enabled"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startSixColumnDiv,new EmbAJAXStatic("<label for=\"ntpserver\">NTP server</label>"),&text3,&endDiv,
      &startSixColumnDiv,new EmbAJAXStatic("<label for=\"tz\">Timezone</label>"),&text4,&endDiv,
    &endDiv,
    //WiFi hotspot
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check3,&startLabelSpan,new EmbAJAXStatic("WiFi Hotspot enabled"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startTwelveColumnDiv,&startLabel,&check4,&startLabelSpan,new EmbAJAXStatic("WiFi Hotspot hidden"),&endSpan,&endLabel,&endDiv,
    &endDiv,
    &startRowDiv,
      &startSixColumnDiv,new EmbAJAXStatic("<label for=\"apssid\">Hotspot SSID</label>"),&text5,&endDiv,
      &startSixColumnDiv,new EmbAJAXStatic("<label for=\"appsk\">Hotspot PSK</label>"),&text6,&endDiv,
    &endDiv,
  &endDiv
)

uint8_t accessId = 0;

char codeToOpen[] = "Open";
char codeToLock[] = "Lock";
char codeToUnlock[] = "Unlock";
char codeToWipeCard[] = "Wipe";
char codeToAllowCard[] = "Allow";
char codeToDenyCard[] = "Deny";

const uint8_t LOCK_NORMAL = 0;
const uint8_t LOCK_LOCKED = 1;
const uint8_t LOCK_UNLOCKED = 2;
const uint8_t LOCK_OPEN = 3;
uint8_t lockState = LOCK_NORMAL;

const uint8_t RFID_READER_NORMAL = 0;
const uint8_t RFID_READER_WIPE_NEXT_CARD = 1;
const uint8_t RFID_READER_ALLOW_NEXT_CARD = 2;
const uint8_t RFID_READER_DENY_NEXT_CARD = 3;
uint8_t rfidReaderState = RFID_READER_NORMAL;

char htmlMimeType[] = "text/html";
char cssMimeType[] = "text/css";
char pngMimeType[] = "image/png";
char jsonMimeType[] = "text/json";

void updateUI() {
}

void setup() {
  #if defined(ESP8266)
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); //Note SERIAL_TX_ONLY allows use of RX for a button
  #elif defined(ARDUINO_ESP32C3_DEV)
    #if ARDUINO_USB_CDC_ON_BOOT == 1
      Serial.begin();
    #else
      Serial.begin(115200);
    #endif
  #endif
  delay(200);  //Delay to make sure debug output gets shown after an upload & reset
  Lock.debug(Serial);
  Lock.begin(accessId); //Start the lock with a specific access Id
  #if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    Lock.enableTapCode(RX);  //Enable tap code on RX pin
  #elif defined(ARDUINO_ESP32C3_DEV)
    Lock.enableTapCode(9);  //Enable tap code on Boot mode pin
  #endif
  #if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
    Lock.enableRFID(D8);  //Enable RFID reader authorisation, CS on pin D8, using default card sector
  #elif defined(ARDUINO_ESP32C3_DEV)
    Lock.enableRFID(7);  //Enable RFID reader authorisation, CS on pin 7, using default card sector
  #endif
  driver.installPage(&page, "/", updateUI);
  server.on("/zones.csv", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/zones.csv");
    });
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/lockConfig.json", jsonMimeType);
    });
  server.on("/test.html", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/test.html", htmlMimeType);
    });
  server.on("/css/normalize.css", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/css/normalize.css", cssMimeType);
    });
  server.on("/css/skeleton.css", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/css/skeleton.css", cssMimeType);
    });
  server.on("/images/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/images/favicon.png", pngMimeType);
    });
  server.begin();
}

void loop() {
  driver.loopHook();
  /*if (mode.selectedOption() == 1) { // blink
      digitalWrite(LEDPIN, (millis() / (1100 - blinkfreq.intValue())) % 2);
  } else {  // on or off
      digitalWrite(LEDPIN, mode.selectedOption() != 0);
  }*/
  Lock.housekeeping();
  if(Lock.codeEntered()) {  //A code has been tapped out
    if(Lock.codeMatches(codeToOpen)) {  //Code matches the 'open' one
      Lock.allow();
    } else if(Lock.codeMatches(codeToLock)) { //Code matches the 'lock' one, permanently locks the lock until overridden
      Lock.lock();
      lockState = LOCK_LOCKED;
    } else if(Lock.codeMatches(codeToUnlock)) { //Code matches the 'unlock' one, permanently opens the lock until overridden
      Lock.unlock();
      lockState = LOCK_UNLOCKED;
    } else if(Lock.codeMatches(codeToWipeCard)) { //Code matches the 'card wipe' one, which will wipe the next presented card
      Serial.println(F("Wiping next card"));
      rfidReaderState = RFID_READER_WIPE_NEXT_CARD;
    } else if(Lock.codeMatches(codeToAllowCard)) { //Code matches the 'card allow' one, which will add this lock to the allow list for the card
      Serial.println(F("Allowing next card"));
      rfidReaderState = RFID_READER_ALLOW_NEXT_CARD;
    } else if(Lock.codeMatches(codeToDenyCard)) { //Code matches the 'card deny' one, which will remove this lock to the allow list for the card
      Serial.println(F("Denying next card"));
      rfidReaderState = RFID_READER_DENY_NEXT_CARD;
    } else {  //Deny access
      Lock.deny();
    }
    Lock.clearEnteredCode();  //Clear the entered code, otherwise it will keep repeating these checks
  }
  if(Lock.cardPresent() && Lock.cardChanged())
  {
    if(rfidReaderState == RFID_READER_NORMAL)
    {
      if(Lock.cardAuthorised(accessId) == true)
      {
        Lock.allow();
        Serial.println(F("Card authorised"));
      }
      else
      {
        Lock.deny();
        Serial.println(F("Card not authorised"));
      }
    }
    else if(rfidReaderState == RFID_READER_WIPE_NEXT_CARD)
    {
      if(Lock.wipeCard())
      {
        Lock.buzzerOn(110,500);
        Serial.println(F("Card wiped"));
        rfidReaderState = RFID_READER_NORMAL;
      }
    }
    else if(rfidReaderState == RFID_READER_ALLOW_NEXT_CARD)
    {
      if(Lock.authoriseCard(accessId))
      {
        Lock.buzzerOn(110,500);
        Serial.println(F("Card allowed"));
        rfidReaderState = RFID_READER_NORMAL;
      }
    }
    else if(rfidReaderState == RFID_READER_DENY_NEXT_CARD)
    {
      if(Lock.revokeCard(accessId))
      {
        Lock.buzzerOn(110,500);
        Serial.println(F("Card denied"));
        rfidReaderState = RFID_READER_NORMAL;
      }
    }
  }
  /*if(Lock.adminOpened())
  {
    Lock.allow();
    Serial.println("Opened by admin interface");
    //delay(1000);
  }*/
}
