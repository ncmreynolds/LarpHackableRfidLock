/*
 * Example sketch for the lock prop, using Tap Code to control the lock
 * 
 */

#include <LarpHackableRfidLock.h>

char codeToOpen[] = "Open";
char codeToLock[] = "Lock";
char codeToUnlock[] = "Unlock";

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY); //Note SERIAL_TX_ONLY allows use of RX for a button
  Lock.Debug(Serial);
  Lock.begin(); //Start the lock
  Lock.enableTapCode();	//Enable tap code
}

void loop() {
  Lock.Housekeeping();
  if(Lock.codeEntered()) {  //A code has been tapped out
    if(Lock.codeMatches(codeToOpen)) {  //Code matches the 'open' one
      Lock.allow();
    } else if(Lock.codeMatches(codeToLock)) { //Code matches the 'lock' one, permanently locks the lock until overridden
      Lock.lock();
    } else if(Lock.codeMatches(codeToUnlock)) { //Code matches the 'unlock' one, permanently opens the lock until overridden
      Lock.unlock();
    } else {  //Deny access
      Lock.deny();
    }
    Lock.clearEnteredCode();  //Clear the entered code, otherwise it will keep repeating these checks
  }
}
