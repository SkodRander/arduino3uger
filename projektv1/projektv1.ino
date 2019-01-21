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

float desire_temp; 
float temperature;
float humidity;
int no_of_profiles = 0;
float tmpin;
boolean buttonstate = false;
byte tmp[4] = {};
const char* ssid = "OnePlus6";
const char* password = "skodrander";
const int red = 1;
const int blue = 3;

/*unsigned long channelID = 673873; //your channal
  const char* myWriteAPIKey = "5SUTO406JKZVF2EN";
  const char* server = "api.thingspeak.com";
  const int postingInterval = 20 * 1000; // post data every 20 seconds */

struct profile {
  byte uid[4];
  float prefer_temp;
  boolean checked_in;
};
struct profile profiles[10];



WiFiClient client;
DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() { //initializing all the connections and such...
  
  //profile hej = {{101, 166, 173, 117}, 25.5, true};
  //profiles[no_of_profiles] = hej;

  dht.setup(0, DHTesp::DHT22);// Connect DHT sensor to GPIO 0 (D3)
  SPI.begin();      // Initiate  SPI bus
  rfid.PCD_Init(); // init RFID
  pinMode(1, OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(1,LOW);
  digitalWrite(3,LOW);
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
  data_to_lcd();
  adjust_temp();
}

void data_to_lcd() {
  temperature = dht.getTemperature();
  humidity = dht.getHumidity();
  //temperature = analogRead(D3);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  delay(3000);
}

void adjust_temp() {
  int sum = 0;
  float tmp_count = 0;
  for(int k = 0; k <= no_of_profiles; k++){
    if(profiles[k].checked_in == true) {
      sum += profiles[k].prefer_temp;
      tmp_count++;
      
    }
  }
  if(tmp_count > 0){
    desire_temp = sum/tmp_count;
  } else {
    desire_temp = temperature; 
  }
 // Serial.print("desire tmp: ");
 // Serial.println(desire_temp);
  float tmp_diff = temperature - desire_temp;
  lcd.clear();
  lcd.print(tmp_diff);
  delay(500);
  if (tmp_diff < -0.2) {
    analogWrite(3,1000);
    analogWrite(1,0);
  } else if (tmp_diff > 0.2){
    analogWrite(3,0);
    analogWrite(1,1000);
  } else {
    analogWrite(1,0);
    analogWrite(3,0);
  }
}

void read_RFID() {
  lcd.clear();
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  //Show UID on serial monitor
  //Serial.println();
  //Serial.print(" UID tag :");
  lcd.print("RFID scanned");
  delay(800);
  //printHex(rfid.uid.uidByte, rfid.uid.size);
  for (byte j = 0; j < rfid.uid.size; j++) {
    tmp[j] = rfid.uid.uidByte[j];
  }
  int i = 0;
  while (i <= no_of_profiles) {
    if (memcmp(profiles[i].uid, tmp, 4) == 0 ) {
    //  Serial.println();
    //  Serial.println("old user");
      lcd.setCursor(0,1);
      lcd.print("status: ");
      profiles[i].checked_in = !profiles[i].checked_in;
      lcd.print(profiles[i].checked_in);
      delay(1500);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(profiles[i].prefer_temp);
      break;
    } else if(i == no_of_profiles){
     // Serial.println("new user detected");
      lcd.setCursor(0,1);
      lcd.print("new user");
      profile_setup(tmp, "profile"+String(no_of_profiles,DEC),4);
      break;
    }
    i++;
  }
  /*Serial.println();
  printHex(tmp, 4);
  Serial.println();*/
  delay(3000);
}

void profile_setup(byte id_n[], String profilename, int siz) {
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
    
    if (digitalRead(btn) == LOW) { //when button is pressed
      profile profilename = {{},tmpin, true};
      for(int k = 0; k < siz; k++) {
        profilename.uid[k] = id_n[k];
      }
      profiles[no_of_profiles] = profilename;
      /*Serial.println();
      Serial.print("user 1:");
      printHex(profiles[0].uid,4);
      Serial.println();
      Serial.print("user 2:");
      printHex(profiles[1].uid,4);
      Serial.println();
      Serial.print("user 3:");
      printHex(profiles[2].uid,4);*/
      break;
      // the temp set from the potentiometer

      //is read and set as the desired temp for that profile
    }
    delay(200);
  }
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
