// ╔══╗───╔╦╦══╦═╦╦═╗╔═╗
// ╚╗╔╩╦═╦╣║╠║║╣║║║║║║╬╠╦╦═╦═╦╦╦═╗╔══╦══╦═╦╦╗
// ─║║╬║║║║║╠║║╣║║║║║║╔╣╔╣╬║╬║╔╣╬╚╣║║║║║║╩╣╔╝
// ─╚╩═╩╩═╩═╩══╩╩═╩═╝╚╝╚╝╚═╬╗╠╝╚══╩╩╩╩╩╩╩═╩╝
// ────────────────────────╚═╝
String Version = "Version 2.0";

#include <SPI.h>
#include <MFRC522.h>

// Pin Defintion for ESP32 Development Kit
#define RST_PIN 2
#define SS_PIN 21

// Pin Defintion Nano
//#define RST_PIN         9
//#define SS_PIN          10
//#define LED             A5

/*
DEBUG Mode enables more detailed console output for debugging
NORMAl Mode does only talk what it needs to
*/
enum ConsoleOutput { NORMAL,
                     DEBUG };
ConsoleOutput consoleOutput = NORMAL;

/* 
Switch Mode between MANUAL - for the Standard Serial Interface Used by Humans
or TOOLBOX - for the use as a Programmer for the modified Arduino-Toolbox Release
https://github.com/donschoof/tonuino-toolbox
*/
enum Mode { MANUAL,
            TOOLBOX };
Mode mode = MANUAL;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

String inputString = "";
String assign_folder = "";
String assign_mode = "";
String assign_file = "";
String assign_file2 = "";
String special = "";
byte special_2 = 0xEA;

byte trailerBlock = 7;
byte blockAddr = 4;
byte magicCookie[] = { 0x13, 0x37, 0xB3, 0x47 };
byte Card_Version = 0x02;
byte cardCookie[5];
byte dataBlock[25];
byte buffer[25];
byte size = sizeof(buffer);
byte len = 16;


void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.setTimeout(2000);
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  pinMode(A5, OUTPUT);
  digitalWrite(A5, HIGH);
}

void loop() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');
    inputString.toLowerCase();
  }
  if (inputString == "debug") {
    if (consoleOutput == NORMAL) {
      Serial.println("DEBUG Mode enabled");
      consoleOutput = DEBUG;
    } else {
      Serial.println("DEBUG Mode disabled");
      consoleOutput = NORMAL;
    }

    inputString = "";
  }
  if (inputString == "info") {
    show_info();
    inputString = "";
  }
  if (inputString == "help") {
    show_help();
    inputString = "";
  }

  if (inputString == "read") {
    read_RFID();
    inputString = "";
  }

  if (inputString == "clear") {
    for (int i = 0; i < len; i++) {
      dataBlock[i] = 0x00;
    }
    write_RFID();
    inputString = "";
  }

  if (inputString == "admin") {
    for (int i = 0; i < 4; i++) {
      dataBlock[i] = magicCookie[i];
    }
    dataBlock[4] = Card_Version;
    dataBlock[5] = 0x00;
    dataBlock[6] = 0xFF;
    dataBlock[7] = 0x09;
    dataBlock[8] = special_2;
    write_RFID();
    delay(250);
    inputString = "";
  }

  if (inputString == "toddler") {
    for (int i = 0; i < 4; i++) {
      dataBlock[i] = magicCookie[i];
    }
    dataBlock[4] = Card_Version;
    dataBlock[5] = 0x00;
    dataBlock[6] = 0x04;
    dataBlock[7] = 0x00;
    dataBlock[8] = 0x00;
    write_RFID();
    delay(250);
    inputString = "";
  }

  if (inputString.substring(0, 11) == "schlummern ") {
    special = inputString.substring(11, 13);
    if (special == "5" || special == "15" || special == "30" || special == "60") {
      for (int i = 0; i < 4; i++) {
        dataBlock[i] = magicCookie[i];
      }
      dataBlock[4] = Card_Version;
      dataBlock[5] = 0x00;
      dataBlock[6] = 0x01;
      dataBlock[7] = special.toInt();
      dataBlock[8] = 0x00;
      write_RFID();
      delay(250);
      inputString = "";
    }
  }

  if (inputString == "kita") {
    for (int i = 0; i < 4; i++) {
      dataBlock[i] = magicCookie[i];
    }
    dataBlock[4] = Card_Version;
    dataBlock[5] = 0x00;
    dataBlock[6] = 0x05;
    dataBlock[7] = 0x00;
    dataBlock[8] = 0x00;
    write_RFID();
    delay(250);
    inputString = "";
  }

  if (inputString == "sperren") {
    for (int i = 0; i < 4; i++) {
      dataBlock[i] = magicCookie[i];
    }
    dataBlock[4] = Card_Version;
    dataBlock[5] = 0x00;
    dataBlock[6] = 0x03;
    dataBlock[7] = 0x00;
    dataBlock[8] = 0x00;
    write_RFID();
    delay(250);
    inputString = "";
  }

  if (inputString == "stoptanz") {
    for (int i = 0; i < 4; i++) {
      dataBlock[i] = magicCookie[i];
    }
    dataBlock[4] = Card_Version;
    dataBlock[5] = 0x00;
    dataBlock[6] = 0x02;
    dataBlock[7] = 0x00;
    dataBlock[8] = 0x00;
    write_RFID();
    delay(250);
    inputString = "";
  }

  if (inputString.substring(0, 6) == "write ") {
    assign_folder = inputString.substring(6, 8);
    assign_mode = inputString.substring(9, 10);
    assign_file = inputString.substring(11, 14);
    assign_file2 = inputString.substring(15, 18);
    if (assign_folder.toInt() != 0 && assign_mode.toInt() > 0 && assign_mode.toInt() < 9 && assign_mode.toInt() != 6 || assign_mode.toInt() == 9) {
      // Party von bis
      if (assign_file.toInt() > 255 || assign_file.toInt() == 0 || assign_file2.toInt() > 255 || assign_file2.toInt() == 0) {
        Serial.println(F("Du kannst nur eine Datei zwischen 1-255 eingeben"));
        Serial.println();
        inputString = "";
      } else if (assign_folder.toInt() == 0 || assign_folder.toInt() > 99) {
        Serial.println(F("Du kannst nur einen Ordner zwischen 1-99 eingeben"));
        Serial.println();
        inputString = "";
      } else {
        for (int i = 0; i < 4; i++) {
          dataBlock[i] = magicCookie[i];
        }
        dataBlock[4] = Card_Version;
        dataBlock[5] = assign_folder.toInt();
        dataBlock[6] = assign_mode.toInt();
        dataBlock[7] = assign_file.toInt();
        dataBlock[8] = assign_file2.toInt();
        write_RFID();
        delay(250);
        inputString = "";
      }
    } else if (assign_mode.toInt() == 8) {
      // Album von bis
      if (assign_file.toInt() > 255 || assign_file.toInt() == 0 || assign_file2.toInt() > 255 || assign_file2.toInt() == 0) {
        Serial.println(F("Du kannst nur eine Datei zwischen 1-255 eingeben"));
        Serial.println();
        inputString = "";
      } else if (assign_folder.toInt() == 0 || assign_folder.toInt() > 99) {
        Serial.println(F("Du kannst nur einen Ordner zwischen 1-99 eingeben"));
        Serial.println();
        inputString = "";
      } else {
        for (int i = 0; i < 4; i++) {
          dataBlock[i] = magicCookie[i];
        }
        dataBlock[4] = Card_Version;
        dataBlock[5] = assign_folder.toInt();
        dataBlock[6] = assign_mode.toInt();
        dataBlock[7] = assign_file.toInt();
        dataBlock[8] = assign_file2.toInt();
        write_RFID();
        delay(250);
        inputString = "";
      }
    } else if (assign_mode.toInt() == 7) {
      // Hörspiel von bis
      if (assign_file.toInt() > 255 || assign_file.toInt() == 0 || assign_file2.toInt() > 255 || assign_file2.toInt() == 0) {
        Serial.println(F("Du kannst nur eine Datei zwischen 1-255 eingeben"));
        Serial.println();
        inputString = "";
      } else if (assign_folder.toInt() == 0 || assign_folder.toInt() > 99) {
        Serial.println(F("Du kannst nur einen Ordner zwischen 1-99 eingeben"));
        Serial.println();
        inputString = "";
      } else {
        for (int i = 0; i < 4; i++) {
          dataBlock[i] = magicCookie[i];
        }
        dataBlock[4] = Card_Version;
        dataBlock[5] = assign_folder.toInt();
        dataBlock[6] = assign_mode.toInt();
        dataBlock[7] = assign_file.toInt();
        dataBlock[8] = assign_file2.toInt();
        write_RFID();
        delay(250);
        inputString = "";
      }
    } else if (assign_mode.toInt() == 5) {
      for (int i = 0; i < 4; i++) {
        dataBlock[i] = magicCookie[i];
      }
      dataBlock[4] = Card_Version;
      dataBlock[5] = assign_folder.toInt();
      dataBlock[6] = assign_mode.toInt();
      dataBlock[7] = 0x00;
      dataBlock[8] = 0x00;
      write_RFID();
      delay(250);
      inputString = "";
    } else if (assign_mode.toInt() == 4) {
      if (assign_file.toInt() > 255 || assign_file.toInt() == 0) {
        Serial.println(F("Du kannst nur eine Datei zwischen 1-255 eingeben"));
        Serial.println();
        inputString = "";
      } else if (assign_folder.toInt() == 0 || assign_folder.toInt() > 99) {
        Serial.println(F("Du kannst nur einen Ordner zwischen 1-99 eingeben"));
        Serial.println();
        inputString = "";
      } else {
        for (int i = 0; i < 4; i++) {
          dataBlock[i] = magicCookie[i];
        }
        dataBlock[4] = Card_Version;
        dataBlock[5] = assign_folder.toInt();
        dataBlock[6] = assign_mode.toInt();
        dataBlock[7] = assign_file.toInt();
        dataBlock[8] = 0x00;
        write_RFID();
        delay(250);
        inputString = "";
      }
    } else if (assign_mode.toInt() > 9) {
      Serial.println(F("Diese Funktion gibt es noch nicht!"));
      Serial.println();
      inputString = "";
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void read_RFID() {

  echoCommand();
  waitForCardReady();

  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  if (consoleOutput == DEBUG) {
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));

    Serial.println(mfrc522.PICC_GetTypeName(piccType));
  }

  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) || (piccType == MFRC522::PICC_TYPE_MIFARE_1K) || (piccType == MFRC522::PICC_TYPE_MIFARE_4K)) {
    if (consoleOutput == DEBUG) {
      Serial.println(F("Authenticating Classic using key A..."));
    }
    status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  } else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
    byte pACK[] = { 0, 0 };  //16 bit PassWord ACK returned by the tempCard

    // Authenticate using key A
    if (consoleOutput == DEBUG) {
      Serial.println(F("Authenticating MIFARE UL..."));
    }
    status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);
  }

  if (status != MFRC522::STATUS_OK) {
    if (consoleOutput == DEBUG) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
    return;
  }

  // Read data from the block
  if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) || (piccType == MFRC522::PICC_TYPE_MIFARE_1K) || (piccType == MFRC522::PICC_TYPE_MIFARE_4K)) {
    if (consoleOutput == DEBUG) {
      Serial.print(F("Reading data from block "));
      Serial.print(blockAddr);
      Serial.println(F(" ..."));
    }
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
      if (consoleOutput == DEBUG) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
      }
      return;
    }
  } else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
    byte buffer2[18];
    byte size2 = sizeof(buffer2);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(8, buffer2, &size2);
    if (status != MFRC522::STATUS_OK) {
      if (consoleOutput == DEBUG) {
        Serial.print(F("MIFARE_Read_1() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
      }
      return;
    }
    memcpy(buffer, buffer2, 4);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(9, buffer2, &size2);
    if (status != MFRC522::STATUS_OK) {
      if (consoleOutput == DEBUG) {
        Serial.print(F("MIFARE_Read_2() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
      }
      return;
    }
    memcpy(buffer + 4, buffer2, 4);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(10, buffer2, &size2);
    if (status != MFRC522::STATUS_OK) {
      if (consoleOutput == DEBUG) {
        Serial.print(F("MIFARE_Read_3() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
      }
      return;
    }
    memcpy(buffer + 8, buffer2, 4);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(11, buffer2, &size2);
    if (status != MFRC522::STATUS_OK) {
      if (consoleOutput == DEBUG) {
        Serial.print(F("MIFARE_Read_4() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
      }
      return;
    }
    memcpy(buffer + 12, buffer2, 4);
  }
  if (consoleOutput == DEBUG) {
    Serial.print(F("Data on Card "));
    Serial.println(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
    Serial.println();
  }

  // Check if we have an Tonuino card or not
  bool isTonuinoCard = false;
  for (int i = 0; i < 4; i++) {
    cardCookie[i] = buffer[i];
  }
  // Does it have the Magic cookie?
  if (checkTwo(magicCookie, cardCookie) == true) {
    isTonuinoCard = true;
  }

  switch (mode) {
    case TOOLBOX: showOK(); break;
    case MANUAL:
      {
        Serial.print("Karte gelesen: ");
        if (isTonuinoCard) {
          Serial.println("Die Karte ist eine Tonuino Karte.");
          assigned_mode(buffer);
        } else {
          Serial.println("Die Karte ist leer, oder nicht für den Tonuino konfiguriert.");
          Serial.println();
        }
      }
      break;
    default: break;
  }
}

void write_RFID() {
  echoCommand();
  // Wait for the Tag
  waitForCardReady();

  // Unlock the Tag
  MFRC522::PICC_Type mifareType = mfrc522.PICC_GetType(mfrc522.uid.sak);

  if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) || (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) || (mifareType == MFRC522::PICC_TYPE_MIFARE_4K)) {
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  } else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL) {
    byte pACK[] = { 0, 0 };                                //16 bit PassWord ACK returned by the NFCtag
    status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);  // Authenticate using key A
  }

  if (status != MFRC522::STATUS_OK) {
    if (consoleOutput == DEBUG) {
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
    showError();
    delay(500);
    return;
  }

  // Write to the tag
  if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) || (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) || (mifareType == MFRC522::PICC_TYPE_MIFARE_4K)) {
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, dataBlock, len);
  } else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL) {
    byte buffer2[16];
    byte size2 = sizeof(buffer2);

    memset(buffer2, 0, size2);
    memcpy(buffer2, dataBlock, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(8, buffer2, 16);

    memset(buffer2, 0, size2);
    memcpy(buffer2, dataBlock + 4, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(9, buffer2, 16);

    memset(buffer2, 0, size2);
    memcpy(buffer2, dataBlock + 8, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(10, buffer2, 16);

    memset(buffer2, 0, size2);
    memcpy(buffer2, dataBlock + 12, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(11, buffer2, 16);
  }

  if (status != MFRC522::STATUS_OK) {
    if (consoleOutput == DEBUG) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
    showError();
    delay(500);
  } else {
    if (consoleOutput == DEBUG) {
      Serial.print(F("WRITE\tBlock 4 = "));
      for (byte i = 0; i < len; i++) {
        Serial.print(dataBlock[i] < 0x10 ? "0" : "");
        Serial.print(dataBlock[i], HEX);
        if (i == 15) {
        } else {
          Serial.print(F("|"));
        }
      }
      Serial.println(F("-> HEX"));
      assigned_mode(dataBlock);
    }
    showOK();
    delay(1000);
  }
}

void assigned_mode(byte buffer[]) {
  // Wiedergabemodus:
  // Hörspiel Modus = 1
  if (buffer[5] != 0x00 && buffer[6] == 0x01) {
    Serial.print(F("Hörspiel Modus 1 -> eine zufällige Datei aus dem Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" abspielen"));
    Serial.println();
  }
  // Album Modus = 2
  if (buffer[5] != 0x00 && buffer[6] == 0x02) {
    Serial.print(F("Album Modus 2 -> den kompletten Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" abspielen"));
    Serial.println();
  }
  // Party Modus = 3
  if (buffer[5] != 0x00 && buffer[6] == 0x03) {
    Serial.print(F("Party Modus 3 -> Dateien des Ordners "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" in zufälliger Reihenfolge abspielen"));
    Serial.println();
  }
  // Einzel Modus = 4
  if (buffer[5] != 0x00 && buffer[6] == 0x04) {
    Serial.print(F("Einzel Modus 4 -> die Datei "));
    Serial.print(buffer[7], DEC);
    Serial.print(F(" aus dem Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" abspielen"));
    Serial.println();
  }
  // Hörbuch Modus = 5
  if (buffer[5] != 0x00 && buffer[6] == 0x05) {
    Serial.print(F("Hörbuch Modus 5 -> kompletten Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" spielen und Fortschritt merken"));
    Serial.println();
  }
  // Hörspiel von bis = 7
  if (buffer[5] != 0x00 && buffer[6] == 0x07) {
    Serial.print(F("Hörspiel Modus 7 -> die Dateien "));
    Serial.print(buffer[7], DEC);
    Serial.print(F(" bis "));
    Serial.print(buffer[8], DEC);
    Serial.print(F(" aus dem Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" abspielen"));
    Serial.println();
  }
  // Album von bis = 8
  if (buffer[5] != 0x00 && buffer[6] == 0x08) {
    Serial.print(F("Album Modus 8 -> die Dateien "));
    Serial.print(buffer[7], DEC);
    Serial.print(F(" bis "));
    Serial.print(buffer[8], DEC);
    Serial.print(F(" aus dem Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" abspielen"));
    Serial.println();
  }
  // Party von bis = 9
  if (buffer[5] != 0x00 && buffer[6] == 0x09) {
    Serial.print(F("Party Modus 9 -> die Dateien "));
    Serial.print(buffer[7], DEC);
    Serial.print(F(" bis "));
    Serial.print(buffer[8], DEC);
    Serial.print(F(" aus dem Ordner "));
    Serial.print(buffer[5], DEC);
    Serial.println(F(" abspielen"));
    Serial.println();
  }
  // Modifikationskarten:
  // Admin Karte
  if (buffer[6] == 0xFF && buffer[7] == 0x09 && buffer[8] == 0xEA) {
    Serial.println(F("Admin Karte"));
    Serial.println();
  }
  // Schlummer Modus
  if (buffer[5] == 0x00 && buffer[6] == 0x01) {
    Serial.print(F("Schlummer-Modus -> "));
    Serial.print(buffer[7], DEC);
    Serial.println(F(" min"));
    Serial.println();
  }
  // Stoptanz Karte
  if (buffer[5] == 0x00 && buffer[6] == 0x02) {
    Serial.println(F("Stoptanz Karte"));
    Serial.println();
  }
  // TonUINO sperren
  if (buffer[5] == 0x00 && buffer[6] == 0x03) {
    Serial.println(F("TonUINO sperren"));
    Serial.println();
  }
  // Toddler Modus
  if (buffer[5] == 0x00 && buffer[6] == 0x04) {
    Serial.println(F("Toddler-Modus"));
    Serial.println();
  }
  // KiTa Karte
  if (buffer[5] == 0x00 && buffer[6] == 0x05) {
    Serial.println(F("KiTa Karte"));
    Serial.println();
  }
  // Wiederholen
  if (buffer[5] == 0x00 && buffer[6] == 0x06) {
    Serial.println(F("Widerholen"));
    Serial.println();
  }
}

void show_info() {
  Serial.println(F(" ╔══╗───╔╦╦══╦═╦╦═╗╔═╗"));
  Serial.println(F(" ╚╗╔╩╦═╦╣║╠║║╣║║║║║║╬╠╦╦═╦═╦╦╦═╗╔══╦══╦═╦╦╗"));
  Serial.println(F(" ─║║╬║║║║║╠║║╣║║║║║║╔╣╔╣╬║╬║╔╣╬╚╣║║║║║║╩╣╔╝"));
  Serial.println(F(" ─╚╩═╩╩═╩═╩══╩╩═╩═╝╚╝╚╝╚═╬╗╠╝╚══╩╩╩╩╩╩╩═╩╝"));
  Serial.println(F(" ────────────────────────╚═╝"));
  Serial.println(F("Designed and created by Christian Kühner 10/2022"));
  Serial.println(F("Mifare Ultralight Update & different modes by David Schäfer 04/2023"));
  Serial.println(Version);
  Serial.print("MFRC522 ");
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println();
  Serial.println(F("Benutze 'help' um die Hilfe anzuzeigen"));
  Serial.println();
}

void show_help() {
  Serial.println("Um eine Karte einzulesen oder zu programmieren tippe erst das auszuführende Kommando ein und lege dann die Karte auf den Leser. Die Karte muss für jedes Kommando neu aufgelegt werden.");
  Serial.println();
  Serial.println(F("Befehle:"));
  Serial.println(F("  clear\t\t\t-> Sektor 1 / Block 4 der RFID Karte wird auf 0x00 geschrieben,"));
  Serial.println(F("\t\t\t   durch fehlen des magick cookie wird Sie vom TonUINO als neue Karte erkannt."));
  Serial.println(F("  read\t\t\t-> NFC Tag lesen"));
  Serial.println(F("  write {xx} {n}\t-> Erstellt TonUINO-Karte aus Ordnernummer {xx} und Wiedergabemodus {n}"));
  Serial.println(F("\t\t\t   und zeigt diese an. z.B. write 01 2 -> spielt den Ordner 1 im Album Modus ab."));
  Serial.println(F("  admin\t\t\t-> Erstellt eine Admin Karte."));
  Serial.println(F("  toddler\t\t-> Erstellt eine Toddler Karte."));
  Serial.println(F("  schlummern {nn}\t-> Erstellt eine Schlummer Karte und zeigt die Minuten {nn} an."));
  Serial.println(F("  stoptanz\t\t-> Erstellt eine Stoptanz Karte."));
  Serial.println(F("  kita\t\t\t-> Erstellt eine KiTa Karte."));
  Serial.println(F("  sperren\t\t-> Erstellt eine TonUINO Sperr-Karte."));
  Serial.println(F("  debug\t\t\t-> Wechselt den Ausgabelevel zw. DEBUG und NORMAL."));
  Serial.println(F("  info\t\t\t-> zeigt den Infoscreen mit Logo an."));
  Serial.println(F("  help\t\t\t-> zeigt diese Hilfe an."));
  Serial.println(F("\n"));
}

void showError() {
  switch (mode) {
    case TOOLBOX: Serial.println("ERROR"); break;
    case MANUAL: Serial.println("Ups, hier ist leider etwas schiefgegangen. Bitte versuche es noch einmal."); break;
    default: break;
  }
}

void showOK() {
  switch (mode) {
    case TOOLBOX: Serial.println("OK"); break;
    case MANUAL:
      {
        Serial.println(inputString + " wurde erfolgreich ausgeführt");
      }
      break;
    default:
      break;
  }
}

void echoCommand() {
  switch (mode) {
    case TOOLBOX:
      Serial.println("RDY");
      break;
    case MANUAL:
      {
        Serial.println(inputString + " ausgeführt. Bitte lege jetzt deine Karte auf den Leser!");
        Serial.println();
      }
      break;
    default:
      break;
  }
}

bool checkTwo(uint8_t a[], uint8_t b[]) {
  for (uint8_t k = 0; k < 4; k++) {  // Loop 4 times
    if (a[k] != b[k]) {              // IF a != b then false, because: one fails, all fail
      return false;
    }
  }
  return true;
}

/**
  Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

bool waitForCardReady() {
  bool newCardPresent = false;
  bool canReadSerial = false;
  while (true) {
    newCardPresent = mfrc522.PICC_IsNewCardPresent();
    canReadSerial = mfrc522.PICC_ReadCardSerial();

    if (newCardPresent && canReadSerial) {
      return true;
    }
  }
}
