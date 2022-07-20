#include "DHT.h"

#define DHTPIN 2     // Digital Pin pour le capteur DHT11 (Pin 2)
#define DHTTYPE DHT11   // Type de capteur utilisé DHT 11

int analogInPin = A0;  // Pin Analogique A0 pour le branchement du capteur d'eau
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int buzzer = 4; // Pin for buzzer
//int note = 255; // Buzzer tone

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
// initialize serial communications at 9600 bps:
  Serial.begin(9600);
  dht.begin(); //début procédure de captage de l'humidiy et de la température
}

void loop() {

 delay(3000);

  // -----------------------------------------------------------
  //--------------- Capteur d'humidité et de température -----------------------------
  //-----------------------------------------------------------
 // Récupération des valeurs du capteur DHT 11
  // taux d'humidity 
  float h = dht.readHumidity(); 
  // température en Celsius (par défaut)
  float t = dht.readTemperature(); 
  // température en Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true); 
  
  // Vérifie s'il ya erreur lors de la lecture.


  // Calcul de la valeur en Fahrenheit
  float hif = dht.computeHeatIndex(f, h);
  // Calcul de la valeur en Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(hic);
  Serial.print(" *C   ");
  Serial.print(hif);
  Serial.print(" *F\t");
 // -----------------------------------------------------------
  //--------------- Capteur d'eau -----------------------------
  //-----------------------------------------------------------
 // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  // change the analog out value:
  
  analogWrite(analogOutPin, outputValue);
  
  // print the results to the serial monitor:
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);
    
if (hic < 20){
  if (sensorValue > 401)
  {
    tone(buzzer, 30);
  }else {noTone(buzzer);}
}else {noTone(buzzer);}
}
