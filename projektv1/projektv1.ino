#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h> 
#include <Wire.h> 
#define btn D4
//#include <ThingSpeak.h>

float temperature;
float humidity;
int n = 0; 
float tmpin;
boolean buttonstate = false;

const char* ssid = "OnePlus6";
const char* password = "skodrander";

/*unsigned long channelID = 673873; //your channal
const char* myWriteAPIKey = "5SUTO406JKZVF2EN"; 
const char* server = "api.thingspeak.com";
const int postingInterval = 20 * 1000; // post data every 20 seconds

struct profile {
  const int id;
  const int temp;
  boolean checked_in;
}*/

WiFiClient client;
DHTesp dht;
LiquidCrystal_I2C lcd(0x27,16,2); 

void setup() { //initializing all the connections and such...
  dht.setup(0, DHTesp::DHT22);// Connect DHT sensor to GPIO 0 (D3)
  lcd.begin();
  lcd.backlight();
  pinMode(D4,INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  lcd.print("Connecting");
  Serial.println("hej");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    lcd.print(".");
  }
  lcd.clear();
  lcd.print("Connected! IP: ");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(3000);
}

void loop() {
  //data_to_lcd();  

  lcd.clear(); // this is all the stuff down in the profile_setup function
  lcd.setCursor(0,0);
  lcd.print("Setup Profile: ");
  delay(5000);
  while(buttonstate == false) {
    lcd.clear();
    lcd.print("What temp do you");
    lcd.setCursor(0,1);
    lcd.print("prefer?");
    lcd.setCursor(10,1);
    tmpin = analogRead(A0);
    tmpin = tmpin/65.0+12.0;
    lcd.print(tmpin,1);
    
    if(digitalRead(btn) == LOW){
      buttonstate = true; 
    }
    delay(200);
  }
  
  Serial.println(tmpin);
  
  delay(5000);
}

void data_to_lcd(){ 
  temperature = dht.getTemperature(); 
  humidity = dht.getHumidity();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  delay(1000); 
}

void check_in_or_out(){
  
}

void read_RFID(){
  
}

void profile_setup(int id_n){
  n++;
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Setup Profile: ");
  delay(5000);
  while(buttonstate == false) {
    lcd.clear();
    lcd.print("What temp do you");
    lcd.setCursor(0,1);
    lcd.print("prefer?");
    lcd.setCursor(10,1);
    tmpin = analogRead(A0);
    tmpin = tmpin/65.0+12.0; //last temp setting
    lcd.print(tmpin,1);
    
    if(digitalRead(btn) == LOW){ //when button is pressed, 
       buttonstate = true;       // the temp set from the potentiometer
                                 //is read and set as the desired temp for that profile
    }
    delay(200);
  }
  //create type of profile with desired profile, id and checked in
  
  
}
