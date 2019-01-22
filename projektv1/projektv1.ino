#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ThingSpeak.h>

#define btn D4                                    // button pin is defined
#define SS_PIN 15                                 // define pins for RFID reader
#define RST_PIN 16

float desire_temp;                                // desired temp to reach for the room/house/building
float temperature;                                // actual temp
float humidity;                                   // actual humidity
int no_of_profiles = 0;                           // counter to keep track of profiles in the system
float tmpin;                                      // used to read a value from potentiometer to later 
                                                  // set the preferred temp for a user
boolean buttonstate = false;
byte tmp[4] = {};                                 // array used to temporarily store UIDs for new 
                                                  // users before creation
const char* ssid = "OnePlus6";                    // ssid name
const char* password = "skodrander";              // ssid password
unsigned long channelID = 673873;                 // thingspeak channel ID
const char* myWriteAPIKey = "OBCP812TQD70P7MS";   // thingspeak API write key
const char* myreadkey = "7HPNF1W4LZPV7BVU";
const char* server = "api.thingspeak.com";        // server url
const int postingInterval = 20*1000;              // post data every 20 seconds 
const int lcdinterval = 5000;                     // lcd update interval
unsigned long previousMillis = 0;                 // value to ensure interval is not being violated
unsigned long previous = 0;                       // ^^^
float outsideTemp = 0;

struct profile {                                  // struct to cre  ate profiles, containing UID, 
  byte uid[4];                                    // preferred temperature and a boolean describing
  float prefer_temp;                              // if user is at home (true) or out (false)
  boolean checked_in;
};

struct profile profiles[20];                      // array to store users max 20, more could be added

WiFiClient client;                                // initializing wifi, DHT sensor, LCD and RFID scanner
DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {                                    // initializing all the connections and such...
  dht.setup(0, DHTesp::DHT22);                    // Connect DHT sensor to GPIO 0 (D3)
  SPI.begin();                                    // Initiate  SPI bus
  rfid.PCD_Init();                                // init RFID
  pinMode(1, OUTPUT);
  pinMode(3,OUTPUT); 
  lcd.begin();
  lcd.backlight();
  pinMode(D4, INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  lcd.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)           // try to connect to WiFi 
  {
    delay(1000);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("Connected! IP: ");                   // succeeds connection
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000);
}

void loop() {                                     // main loop, which keeps track of all functions
  unsigned long currentMillis = millis();         // and intervals
  read_RFID();
  if(currentMillis - previous >= lcdinterval) {   // lcd post every 5 sec
    previous = currentMillis;
    data_to_lcd();                                
  }
  if(currentMillis - previousMillis >= postingInterval){ //thingspeak post every 20 sec
    previousMillis = currentMillis;
    sendToThingspeak(temperature,humidity); 
  }
  adjust_temp();                                  // always adjusting temperature
}

void data_to_lcd() {                              // function to get the temp and humidity
  temperature = dht.getTemperature();             // and display on LCD
  humidity = dht.getHumidity();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp ");                
  lcd.print(temperature,1);
  lcd.print(" / ");
  lcd.print(outsideTemp,1);
  lcd.setCursor(0, 1);
  lcd.print("Humidity    ");
  lcd.print(humidity,1);
  
}

void adjust_temp() {                              // function to adjust temperature
  int sum = 0;
  float tmp_count = 0;
  for(int k = 0; k <= no_of_profiles; k++){
    if(profiles[k].checked_in == true) {          // gets the preferred temp of all users
      sum += profiles[k].prefer_temp;             // currently home
      tmp_count++;  
    }
  }
  if(tmp_count > 0){
    desire_temp = sum/tmp_count;                  // sets a desired temp based on the 
  } else {                                        // preferred temperature(s) of the users
    desire_temp = temperature; 
  }
  float tmp_diff = temperature - desire_temp;     // difference between actual temp and desired
  if (tmp_diff < -0.5) {                          // if actual temp is within +-0.5 C of desired
    analogWrite(3,-tmp_diff*250);                 // no adjustment is made
    analogWrite(1,0);
  } else if (tmp_diff > 0.5){
    analogWrite(3,0);
    analogWrite(1,tmp_diff*250);                  // the larger the difference, the large the 
  } else {                                        // adjustment
    analogWrite(1,0);
    analogWrite(3,0);
  }
}

void read_RFID() {
  if ( ! rfid.PICC_IsNewCardPresent())            // if no RFID has been read, do nothing
    return;
                                                  // Verify if the UID has been read
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  lcd.clear();
  lcd.print("RFID scanned");
  delay(800);
  for (byte j = 0; j < rfid.uid.size; j++) {
    tmp[j] = rfid.uid.uidByte[j];                 // read the UID into tmp array
  }
  int i = 0;
  while (i <= no_of_profiles) {
    if (memcmp(profiles[i].uid, tmp, 4) == 0 ) {  // if any profile matches the UID,
      lcd.setCursor(0,1);                         // check that user in or out
      lcd.print("status: ");
      profiles[i].checked_in = !profiles[i].checked_in;
      lcd.print(profiles[i].checked_in);
      delay(1500);
      lcd.clear();
      lcd.setCursor(0,0);
      break;
    } else if(i == no_of_profiles){               // else create new user with that UID
      lcd.setCursor(0,1);
      lcd.print("new user");
      profile_setup(tmp, "profile"+String(no_of_profiles,DEC),4);
      break;
    }
    i++;
  }
  delay(800);
}

void profile_setup(byte id_n[], String profilename, int siz) { 
  no_of_profiles++;                               // set profile up with preferred temp
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
    tmpin = tmpin / 65.0 + 12.0;                  //last temp setting 
    lcd.print(tmpin, 1);
    if (digitalRead(btn) == LOW) {                //when button is pressed
      profile profilename = {{},tmpin, true}; 
      for(int k = 0; k < siz; k++) {
        profilename.uid[k] = id_n[k];             //set uid in the struct for new profile
      }
      profiles[no_of_profiles] = profilename;     //create new profile in the array
      break;
    }
    delay(200);
  }
}

void sendToThingspeak(float temp, float humid){   // send data to thingspeak server
  int usersAtHome = 0;                      
  ThingSpeak.begin(client);
  client.connect(server,80);
  ThingSpeak.setField(1,temp);                    // take latest temp
  for(int k = 0; k <= no_of_profiles; k++){       // count users currently at home
    if(profiles[k].checked_in == true) {
      usersAtHome++;
    }
  } 
  ThingSpeak.setField(2, usersAtHome);
  ThingSpeak.setField(3, humid);                  // take lates humidity
  outsideTemp = ThingSpeak.readFloatField(channelID, 4, myreadkey);
  ThingSpeak.writeFields(channelID, myWriteAPIKey); //post temp, humidity and users at home to thingspeak
  client.stop(); 
}
