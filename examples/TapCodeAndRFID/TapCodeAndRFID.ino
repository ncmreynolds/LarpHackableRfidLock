/*
 * Example sketch for the lock prop, using Tap Code and/or RFID to control the lock
 * 
 */

#include <LarpHackableRfidLock.h>

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

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); //Note SERIAL_TX_ONLY allows use of RX for a button
  Lock.Debug(Serial);
  Lock.begin(accessId); //Start the lock with a specific access Id
  Lock.enableTapCode();  //Enable tap code
  Lock.enableRFID();  //Enable RFID reader
}

void loop() {
  Lock.Housekeeping();
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
  if(Lock.CardPresent() && Lock.CardChanged())
  {
    if(rfidReaderState == RFID_READER_NORMAL)
    {
      if(Lock.CardAuthorised(accessId) == true)
      {
        Lock.allow();
      }
      else
      {
        Serial.println(F("Card not authorised"));
      }
    }
    else if(rfidReaderState == RFID_READER_WIPE_NEXT_CARD)
    {
      if(Lock.WipeCard())
      {
        Lock.BuzzerOn(110,500);
        Serial.println(F("Card wiped"));
        rfidReaderState = RFID_READER_NORMAL;
      }
    }
    else if(rfidReaderState == RFID_READER_ALLOW_NEXT_CARD)
    {
      if(Lock.AllowCard(accessId))
      {
        Lock.BuzzerOn(110,500);
        Serial.println(F("Card allowed"));
        rfidReaderState = RFID_READER_NORMAL;
      }
    }
    else if(rfidReaderState == RFID_READER_DENY_NEXT_CARD)
    {
      if(Lock.DenyCard(accessId))
      {
        Lock.BuzzerOn(110,500);
        Serial.println(F("Card denied"));
        rfidReaderState = RFID_READER_NORMAL;
      }
    }
  }
}
