#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h> 
#include <Wire.h> 
#include <ThingSpeak.h>

float temperature;
float humidity;

const char* ssid = "OnePlus6";
const char* password = "skodrander";
unsigned long channelID = 673873; //your channal
const char* myWriteAPIKey = "5SUTO406JKZVF2EN"; 
const char* server = "api.thingspeak.com";
const int postingInterval = 20 * 1000; // post data every 20 seconds

struct profile {
  int id;
  int temp;
  boolean checked_in;
}

WiFiClient client;
DHTesp dht;
LiquidCrystal_I2C lcd(0x27,16,2); 
void setup() {
  dht.setup(0, DHTesp::DHT22);// Connect DHT sensor to GPIO 0 (D3)
  lcd.begin();
  lcd.backlight();
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
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(3000);
}

void loop() {
  data_to_lcd();
  delay(1000);
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
  delay(1100); 
}

void check_in_or_out(){
  
}

void read_RFID(){
  
}

void profile_setup(){
  
}
