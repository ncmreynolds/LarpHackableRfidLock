# LARP hackable RFID lock
Code, circuits and 3D printable enclosures for a 'hackable' RFID lock prop usable in LARP or similar events.

This is distributed an an Arduino library to make it easy for novice users to install and get all the dependencies included.

## Design goals

- Cheap! You need one per door, LARPs often have lots of doors.
- Ubiquitous hobbyist hardware, nothing custom/rare
- Easy to build and solder
- Removable AA batteries, easy to change, because you **will** forget to charge them at some point
- Flexible software options (local card database, MQTT server etc.)
- Easy to reset to defaults before/during/after events, no reprogramming
- Battery life of ~48 hours, this may be hard to manage without using deep sleep during periods of long inactivity.

## Why is this an Arduino Library when it's a really plans for a prop?

Doing this makes it easier for coding novices. They can install the 'library' into an unused copy of the Arduino IDE and it will also install the dependencies and give them a dropdown list of any different versions of the software under 'Examples'.

There are no plans to add it to the main Arduino Library list as it is a bit niche.

## Getting started

Download and install a local copy of the [Arduino IDE](https://www.arduino.cc/en/software) to your computer. I only have a PC so all instructions will be Windows-centric, but iOS should be broadly similar.

Download a copy of the library.

Install the library.

## Bill of materials

Here are links to the specific modules you need for this build. The LEDs can be swapped out for pretty much any old LED or combined into a single RGB one.

[Wemos D1 mini microcontroller](https://uk.banggood.com/Geekcreit-D1-Mini-V3_0_0-WIFI-Internet-Of-Things-Development-Board-Based-ESP8266-4MB-MicroPython-Nodemcu-p-1264245.html?cur_warehouse=CN&ID=522225&rmmds=search) ~£3.00

[MFRC522 SPI RFID reader/writer](https://uk.banggood.com/MFRC-522-RC522-RFID-RF-IC-Card-Reader-Sensor-Module-Solder-8P-Socket-p-1566599.html?cur_warehouse=CN&rmmds=search) ~£2.50

[10mm red LEDs](https://uk.banggood.com/50pcs-10mm-2Pin-620-625nm-Red-Diffused-Round-Through-Hole-2V-20mA-DIP-LED-Diode-Electronic-Component-p-1560367.html?cur_warehouse=CN&rmmds=search) ~£3.50 for 50

[10mm green LEDs](https://uk.banggood.com/50pcs-10mm-2Pin-DIY-Green-Diffused-Round-Through-Hole-3V-20mA-LED-Diode-Electronic-Component-p-1560361.html?cur_warehouse=CN&rmmds=search) £6.50 for 50

100 ohm resistors 

3xAA switched battery box ~£?

[10mm LED bezels](https://uk.banggood.com/3mm-or-5mm-or-8mm-or-10mm-Light-Cup-5Pcs-For-RC-Car-LED-Lights-p-930752.html?cur_warehouse=CN&ID=510359&rmmds=search) ~£4.65 for 5 (optional, for looks)

[Passive buzzer module](https://uk.banggood.com/3_3-5V-Passive-Buzzer-Alarm-Module-p-985131.html?cur_warehouse=CN&rmmds=search) ~£2.20 (optional)

Button (optional)

## Wiring

The WeMos D1 mini has just enough pins free for this prop. Use the following wiring. You will need several connections to GND  of the D1 mini so solder one wire then join the others to that.

- Red LED anode (long lead) to pin D2 of the D1 mini
- Red LED cathode (short lead) to GND
- Green LED anode (long lead)  to pin D4 of the D1 mini
- Green LED cathode (short lead) to GND
- RFID MOSI to pin D7 of the D1 mini
- RFID MISO to pin D6 of the D1 mini
- RFID SCK (CLK) to pin D5 of the D1 mini
- RFID SDA (CS) to pin D8 of the D1 mini
- RFID RST (Reset) to pin D0 of the D1 mini
- Battery holder + (red wire) to 5V of the D1 mini
- Battery holder - (black wire) to GND

#### Optional buzzer

- Buzzer + to 5V of the D1 mini
- Buzzer - to GND
- Buzzer I/O to pin D3 of the D1 mini

#### Optional button

- Connect one terminal to GND
- Connect one terminal to D1 of the D1 mini

Here's a shot of all this being tested on a breadboard.

![](images\breadboard.png)

## Cases

There are STLs for several styles of case

- 3xAA battery holder, USB connector exposed
- 3xAA battery holder, USB connector hidden
- 4xAA battery holder, USB connector exposed
- 4xAA battery holder, USB connector hidden
