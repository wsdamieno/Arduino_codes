#include <SPI.h>
#include <LoRa.h>
#include <SimpleDHT.h>
#include <EEPROM.h>
#include <math.h>

int counter = 0;

// DHT11 parameters 
int dht_dpin = 5;  
SimpleDHT11 DHT_sens(dht_dpin);           //datapin sensor to pin 5 Arduino


void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.print("The values inside the EEPROM are : ");
 /*
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
        Serial.print(" ");
      }
      Serial.println(" ****************** ");
*/ 
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        Serial.print(EEPROM.read(i));
        Serial.print(" ");
      }
      Serial.print(" \n");

   delay(120000); //Wait 120 second before starting
}

void loop() {
  static uint32_t eeprom_write;
  byte h = 0;
  byte t = 0;
  float temp;

  //Compute the temperature with the LMD35DZ
  temp = analogRead(A0)*0.48828125 ;

  //Arround the float temperature to an integer
  byte temperature = floor(temp + 0.5);
  DHT_sens.read(&t, &h, NULL);

  int moisture_yl = analogRead(A1);
  int min_yl = 1023;
  int max_yl = 340;
  byte moisture_yl_percent = map(moisture_yl, min_yl, max_yl, 0, 100); 
  
  int capacitive = analogRead(A2);
  int min_capa = 572;
  int max_capa = 298;
  byte capacitive_percent = map (capacitive, min_capa, max_capa, 0, 100);

  int moisture = analogRead(A3);
  int min_moisture = 213;
  int max_moisture = 885;
  byte moisture_percent = map (moisture, min_moisture, max_moisture, 0, 100);

  Serial.print("Temperature :");
  Serial.print(t);
  Serial.print("/");
  Serial.print(temperature);
  Serial.print("/");
  Serial.print(temp);
  Serial.println(" C");

  Serial.print("Humidity :");
  Serial.print(h);
  Serial.println(" %");
  
  Serial.print("**************************\n");
  Serial.print("Soil Moisture YL-69 : ");
  Serial.print(moisture_yl_percent);
  Serial.println(" %");
  Serial.print("\n**************************\n");
  Serial.print("Capacitive Soil Moisture V1.2 : ");
  Serial.print(capacitive_percent);
  Serial.println(" %");
  Serial.print("\n**************************\n");
  Serial.print(" 1322 Soil Moisture : ");
  Serial.print(moisture);
  Serial.print(" / ");
  Serial.print(moisture_percent);
  Serial.println(" % ");
  Serial.print("\n**************************\n");
  
  
  Serial.print("Sending packet: ");
  Serial.println(counter);

  
  if (eeprom_write <= 1022){
    
  // send packet
  LoRa.beginPacket();
  LoRa.print(counter);

  EEPROM.write(eeprom_write, counter); //
    EEPROM.write(eeprom_write + 1, t); //
    EEPROM.write(eeprom_write + 2, h); //
    EEPROM.write(eeprom_write + 3, temperature); //
    EEPROM.write(eeprom_write + 4, moisture_yl_percent); //
    EEPROM.write(eeprom_write + 5, capacitive_percent); //
    EEPROM.write(eeprom_write + 6, moisture_percent); //
  }
  LoRa.endPacket();

  counter++;
  eeprom_write+=7;

  delay(3000);
}
