#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <DHTesp.h>
#include <ESP8266WiFi.h> 

LiquidCrystal_I2C lcd(0x27,16,2);

DHTesp dht;

void setup() {
  
  dht.setup(0, DHTesp::DHT22); // Connect DHT sensor to GPIO 0 (D3)
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  lcd.setCursor(0,0);
  lcd.clear();
}

void loop() {
  
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();
  Serial.println(temperature);
  Serial.println(humidity);
  lcd.clear();
  lcd.print("hejsa");
  delay(500);

}
