#include <nRF905.h>
#include <SimpleDHT.h>
#include <EEPROM.h>
#include <math.h>

#define TXADDR 0xFE // Address of this device
#define RXADDR 0x58 // Address of device to send to

#define TIMEOUT 1000 // 1 second ping timeout

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

//Type of packet
#define PACKET_TYPE_DATA  0
#define PACKET_TYPE_ACK   1

// DHT11 parameters 
int dht_dpin = 5;  
SimpleDHT11 DHT_sens(dht_dpin);           //datapin sensor to pin 5 Arduino


static volatile uint8_t packetStatus;

void NRF905_CB_RXCOMPLETE(void)
{
	packetStatus = PACKET_OK;
}

void NRF905_CB_RXINVALID(void)
{
	packetStatus = PACKET_INVALID;
}

void setup()
{
  Serial.begin(115200);
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
	Serial.println(F("Client started"));
	
	//pinMode(A5, OUTPUT); // LED

  delay(180000); //Wait 180 second before starting
  

	// Start up
	nRF905_init();
	
	// Set address of this device
	nRF905_setListenAddress(TXADDR);

  // Maximum transmit level 10dbm
  nRF905_setTransmitPower(NRF905_PWR_10);
}

void loop()
{
	static uint8_t counter;
	static uint32_t sent;
	static uint32_t replies;
	static uint32_t timeouts;
	static uint32_t invalids;
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

	// Make data
	char data[NRF905_MAX_PAYLOAD] = {0};
	sprintf(data, "test %hhu", counter);
	counter++;
	
	packetStatus = PACKET_NONE;

	Serial.print(F("Sending data: "));
	Serial.println(data);
	
	uint32_t startTime = millis();

  byte tmpBuff[NRF905_MAX_PAYLOAD] = {};
  tmpBuff[0] = TXADDR;
  tmpBuff[1] = PACKET_TYPE_DATA;
  tmpBuff[2] = TXADDR;
  tmpBuff[3] = RXADDR;
  tmpBuff[4] = t;
  tmpBuff[5] = h;
  tmpBuff[6] = temperature;
  tmpBuff[7] = moisture_yl_percent;
  tmpBuff[8] = capacitive_percent;

  
	// Send the data (send fails if other transmissions are going on, keep trying until success) and enter RX mode on completion
	//while(!nRF905_TX(TXADDR, data, sizeof(data), NRF905_NEXTMODE_RX));

  if (sent <= 170) {
	if (nRF905_TX(RXADDR, tmpBuff, sizeof(tmpBuff), NRF905_NEXTMODE_STANDBY)){
    Serial.print("Packet sent : ");
    for (int i = 0; i<NRF905_MAX_PAYLOAD; i++){
        Serial.print(tmpBuff[i]);
        Serial.print(" ");
    }
    Serial.print("\n");
    //sent++;
   }
  }
   
	
  	Serial.print(F("Totals: "));
  	Serial.print(sent);
  	Serial.println(F(" Sent, "));
  	Serial.println(F("--------------------"));
    sent++;
    
    EEPROM.write(eeprom_write, t); //write 0 to address 50 + k
    EEPROM.write(eeprom_write + 1, h); //write 0 to address 51 + k
    EEPROM.write(eeprom_write + 2, temperature); //write 0 to address 52 + k 
    EEPROM.write(eeprom_write + 3, moisture_yl_percent); //write 0 to address 53 + k
    EEPROM.write(eeprom_write + 4, capacitive_percent); //write 0 to address 54 + k
    EEPROM.write(eeprom_write + 5, sent); //write at the address 10 + k the number of sent packets
  
    Serial.print(" Values of the EEPROM pointer : ");
    Serial.println(eeprom_write);
    
    eeprom_write+=6;
   
	delay(2000);
}
