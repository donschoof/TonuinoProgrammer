# TonuinoProgrammer
Arduino Based NFC Programmer for the Tonuino Project

## New Features
* Updated Code to Support Mifare Ultralight (PICC_TYPE_MIFARE_UL) as the [TonUINO](https://github.com/tonuino/TonUINO-TNG) supports it
* DEBUG Mode can be toggled by sending the `degug` command
* New UI Mode. You can select between `NORMAL` - full UI on Console like orignal Sourcecode and `TOOLBOX`, which has a minimal UI to be used programmatically
* Support for the newer TonUINO Modes 7-9 (from .. to Modes)


## Instructions
1. Clone the project `git clone https://github.com/donschoof/TonuinoProgrammer.git` or download https://github.com/donschoof/TonuinoProgrammer/raw/main/TonuinoProgrammer.ino directly.

2. Open TonuinoProgrammer.ino in Arduino IDE

3. Install the following Library in Arduino IDE
* https://github.com/miguelbalboa/rfid - MFRC522 by GithubCommunity@1.4.10

4. Select your Board (install through Board Manager if needed), check/edit the Pinout and flash it with Arduino IDE

## Example Setup
1. Arduino Nano + MRFC522: https://discourse.voss.earth/t/tonuino-karten-programmer/12137

2. ESP32 + MRFC522
```
3.3V <-> 3.3V
GND <-> GND
D2 <-> RST
D18 <-> SCK
D19 <-> MISO
D21 <-> SDA
D23 <-> MOSI
```

# Original Sourcecode
Thanks to **Real_Kuno** @ https://discourse.voss.earth/t/tonuino-karten-programmer/12137/3
