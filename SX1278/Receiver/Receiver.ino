#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>


byte sign (float value_read);

void setup() {
  Serial.begin(9600);
     
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  
  /*
  for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
        Serial.print(" ");
      }
     Serial.println(" ****************** ");
   */  
  Serial.print("The values inside the EEPROM are : ");
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        Serial.print(EEPROM.read(i));
        Serial.print(" ");
      }
      Serial.print(" \n");

  //delay(120000); //Wait 120 seconds before starting
      
}

void loop() {
  static uint32_t eeprom_write;
  static uint32_t pings;
  
  // try to parse packet
  int packetSize = LoRa.parsePacket();
    
  if (packetSize) {
    int moisture_yl = analogRead(A0);
    int min_yl = 1023;
    int max_yl = 460;
    byte moisture_yl_percent = map(moisture_yl, min_yl, max_yl, 0, 100); 
    Serial.print(" Moisture : "); 
    Serial.print(moisture_yl);
    Serial.print(" / "); 
    Serial.println(moisture_yl_percent);
    // received a packet
    Serial.print("Received packet '");

    String received;
    // read packet
    while (LoRa.available()) {
      received = received+(const char)LoRa.read();
    }
    Serial.print(received);
    byte sign_rssi = sign(LoRa.packetRssi());
    byte sign_snr = sign(LoRa.packetSnr());

    byte abs_rssi = fabs(LoRa.packetRssi());
    byte abs_snr = floor(fabs(LoRa.packetSnr())+ 0.5);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.print(LoRa.packetRssi());
    Serial.print("/");
    Serial.print(sign_rssi);
    Serial.print("(sign)");
    Serial.println(abs_rssi);

    // print SNR of packet
    Serial.print(" with SNR ");
    Serial.print(LoRa.packetSnr());
    Serial.print("/");
    Serial.print(sign_snr);
    Serial.print("(sign)");
    Serial.println(abs_snr);

    Serial.print(" Value received : ");
    // Define 
    
    // Length (with one extra character for the null terminator)
    int str_len = received.length() + 1; 
    
    // Prepare the character array (the buffer) 
    char value_char[str_len];
    
    // Copy it over 
    received.toCharArray(value_char, str_len);
    byte read_value = atoi(value_char);
    //char value_char [] = received.toCharArray(); 
    Serial.println(read_value);
    
   
    EEPROM.write(eeprom_write, pings); //write at the address 1 + k the number of pingg
    EEPROM.write(eeprom_write + 1, read_value); //write at the address 2 + k the number of invalids packets
    EEPROM.write(eeprom_write + 2, moisture_yl_percent); //write at the address 3 + k the moiture percent with the YL-69 soil moisture
    EEPROM.write(eeprom_write + 3, sign_rssi); // Sign of the received RSSI  
    EEPROM.write(eeprom_write + 4, abs_rssi); // Abs value of the received RSSI  
    EEPROM.write(eeprom_write + 5, sign_snr); // Sign of the received SNR  
    EEPROM.write(eeprom_write + 6, abs_snr); // Abs of the received SNR  
    eeprom_write+=7;
    pings++;
    
  }
}

byte sign (float value_read){
  if (value_read < 0){
    return 1;
  }else {
    return 0;
  }
}
