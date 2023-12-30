#include <Adafruit_Fingerprint.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo myservo;
int LEDM = 2;
int LEDH = 0;
int buzzer = 26;

uint8_t id;

void setup() {  //ini awal setup
  myservo.attach(33);
  pinMode(LEDM, OUTPUT);
  pinMode(LEDH, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  while (!Serial)
    ;  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  // finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  //**********************
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }

}  //ini tutup setup

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

int menu = 0;
bool stop = false;

void loop() {  //ini awal loop

  if (Serial.available()) {  // listenig to serial input;
    menu = Serial.parseInt();
    stop = false;
  }

  if (menu == 0 && stop == false) {
    Serial.println("Pilih :");
    Serial.println("1. Daftar Sidik Jari");
    Serial.println("2. Tes Sidik Jari");
    myservo.write(90);
    digitalWrite(LEDH, LOW);
    digitalWrite(LEDM, LOW);
    digitalWrite(buzzer, LOW);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("pilih 1.daftar");
    lcd.setCursor(7, 1);
    lcd.print("2.scan");
    stop = true;
  } else if (menu == 1 && stop == false) {
    lcd.clear();
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    lcd.setCursor(2, 0);
    lcd.print("masukkan id!");
    id = readnumber();
    if (id == 0) {  // ID #0 not allowed, try again!
      return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);
    while (!getFingerprintEnroll())
      ;
    menu = 0;
    stop = true;
  } else if (menu == 2) {
    getFingerprintID();
    delay(50);
  }

}  //ini tutup loop
// *************fungsi untuk enroll***************
uint8_t getFingerprintEnroll() {
  lcd.clear();
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  lcd.setCursor(1, 0);
  lcd.print("letakkan jari!");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  lcd.clear();
  Serial.println("Remove finger");
  lcd.setCursor(1, 0);
  lcd.print("lepaskan jari!");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("letakkan lagi!");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    Serial.println("Stored!");
    lcd.setCursor(4, 0);
    lcd.print("berhasil");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
// ************fungsi untuk scan******************
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();

  Serial.println("Waiting ");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("menunggu jari!");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    Serial.print(".");
    delay(10);
  }

  Serial.println("Image taken");

  // switch (p) {
  //   case FINGERPRINT_OK:
  //     Serial.println("Image taken");
  //     break;
  //   case FINGERPRINT_NOFINGER:
  //     Serial.println("No finger detected");
  //     return p;
  //   case FINGERPRINT_PACKETRECIEVEERR:
  //     Serial.println("Communication error");
  //     return p;
  //   case FINGERPRINT_IMAGEFAIL:
  //     Serial.println("Imaging error");
  //     return p;
  //   default:
  //     Serial.println("Unknown error");
  //     return p;
  // }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    Serial.println("Found a print match!");
    myservo.write(180);  // menggerakkan servo ke posisi 0 derajat
    digitalWrite(LEDH, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(buzzer, HIGH);
    delay(100);
    lcd.setCursor(0, 0);
    lcd.print("selamat datang!!");
    lcd.setCursor(2, 1);
    lcd.print("hati hati ya");
    // delay(5000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.setCursor(1, 0);
    lcd.print("tidak dikenali");
    lcd.setCursor(2, 1);
    lcd.print("kamu siapa!!");
    digitalWrite(LEDM, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(200);
    return p;

  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

//*************bagian delete********************

// #include <Adafruit_Fingerprint.h>
// HardwareSerial mySerial(2);
// Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// void setup()
// {
//   Serial.begin(9600);
//   mySerial.begin(57600, SERIAL_8N1, 16, 17);
//   while (!Serial);  // For Yun/Leo/Micro/Zero/...
//   delay(100);
//   Serial.println("\n\nDelete Finger");

//   // set the data rate for the sensor serial port
//   finger.begin(57600);

//   if (finger.verifyPassword()) {
//     Serial.println("Found fingerprint sensor!");
//   } else {
//     Serial.println("Did not find fingerprint sensor :(");
//     while (1);
//   }
// }


// uint8_t readnumber(void) {
//   uint8_t num = 0;

//   while (num == 0) {
//     while (! Serial.available());
//     num = Serial.parseInt();
//   }
//   return num;
// }

// void loop()                     // run over and over again
// {
//   Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
//   uint8_t id = readnumber();
//   if (id == 0) {// ID #0 not allowed, try again!
//      return;
//   }

//   Serial.print("Deleting ID #");
//   Serial.println(id);

//   deleteFingerprint(id);
// }

// uint8_t deleteFingerprint(uint8_t id) {
//   uint8_t p = -1;

//   p = finger.deleteModel(id);

//   if (p == FINGERPRINT_OK) {
//     Serial.println("Deleted!");
//   } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//     Serial.println("Communication error");
//   } else if (p == FINGERPRINT_BADLOCATION) {
//     Serial.println("Could not delete in that location");
//   } else if (p == FINGERPRINT_FLASHERR) {
//     Serial.println("Error writing to flash");
//   } else {
//     Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
//   }

//   return p;
// }
