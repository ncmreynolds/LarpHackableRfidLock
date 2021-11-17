# LARP hackable RFID lock
Code, circuits and 3D printable enclosures for a 'hackable' RFID lock prop usable in LARP or similar events.

This is distributed an an Arduino library to make it easy for novice users to install and get all the dependencies included.

## Required hardware

- WeMos D1 Mini (ESP8266 based microcontroller)

- MFRC-522 RC522 RFID RF IC Card Reader
- Two LEDs and resistors
- Piezo sounder (optional)
- Numeric keypad (optional)
- Battery holder with switch (4x AA)

## Design goals

- Easy to build and solder
- Removable batteries, easy to change
- Flexible software options (local card database, MQTT server etc.)
