#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#define btn D4
//#include <ThingSpeak.h>

#define SS_PIN 15
#define RST_PIN 16

float temperature;
float humidity;
int no_of_profiles = 0;
float tmpin;
boolean buttonstate = false;
byte tmp[4] = {};
const char* ssid = "OnePlus6";
const char* password = "skodrander";

/*unsigned long channelID = 673873; //your channal
  const char* myWriteAPIKey = "5SUTO406JKZVF2EN";
  const char* server = "api.thingspeak.com";
  const int postingInterval = 20 * 1000; // post data every 20 seconds */

struct profile {
  byte uid[4];
  const int prefer_temp;
  boolean checked_in;
};
profile hej = {{101, 166, 173, 117}, 25.5, true};


profile profiles[1] = {hej};

WiFiClient client;
DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() { //initializing all the connections and such...
  dht.setup(0, DHTesp::DHT22);// Connect DHT sensor to GPIO 0 (D3)
  SPI.begin();      // Initiate  SPI bus
  rfid.PCD_Init(); // init RFID
  lcd.begin();
  lcd.backlight();
  pinMode(D4, INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  lcd.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("Connected! IP: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000);
}

void loop() {
  read_RFID();
}

void data_to_lcd() {
  temperature = dht.getTemperature();
  humidity = dht.getHumidity();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  delay(1000);
}

void check_in_or_out() {

}

void read_RFID() {
  lcd.clear();
  lcd.print("Scan your tag:");
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  //printHex(rfid.uid.uidByte, rfid.uid.size);
  for (byte i = 0; i < rfid.uid.size; i++) {
    tmp[i] = rfid.uid.uidByte[i];
  }
  int i = 0;
  while (i <= no_of_profiles) {
    if (memcmp(profiles[i].uid, tmp, 4) != 0) {
      Serial.println("new user detected");
    } else if(i == no_of_profiles){
      Serial.println("old user");
    }
    i++;
  }
  printHex(tmp, 4);
  Serial.println();
  printHex(hej.uid,4);
  delay(3000);
}

void profile_setup(int id_n) {
  no_of_profiles++;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setup Profile: ");
  delay(5000);
  while (buttonstate == false) {
    lcd.clear();
    lcd.print("What temp do you");
    lcd.setCursor(0, 1);
    lcd.print("prefer?");
    lcd.setCursor(10, 1);
    tmpin = analogRead(A0);
    tmpin = tmpin / 65.0 + 12.0; //last temp setting
    lcd.print(tmpin, 1);

    if (digitalRead(btn) == LOW) { //when button is pressed,
      buttonstate = true;       // the temp set from the potentiometer
      //is read and set as the desired temp for that profile
    }
    delay(200);
  }
  //create type of profile with desired profile, id and checked in
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
