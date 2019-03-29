#include <nRF905.h>
#include <time.h>
#include <EEPROM.h>

#define RXADDR 0x58 // Address of this device
#define TXADDR 0xFE // Address of device to send to

//State of packet
#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

//Type of packet
#define PACKET_TYPE_DATA  0
#define PACKET_TYPE_ACK   1

//Pin for buzzer tone
const int buzzer = 6;

static volatile uint8_t packetStatus;

void NRF905_CB_RXCOMPLETE(void)
{
	packetStatus = PACKET_OK;
	nRF905_standby();
}

void NRF905_CB_RXINVALID(void)
{
	packetStatus = PACKET_INVALID;
	nRF905_standby();
}

void setup()
{
	Serial.begin(115200);
	Serial.println(F("Server started"));

	pinMode(A5, OUTPUT); // LED


  
      
  /*
   //read EEPROM value at the address 0x03 = 3 
  byte read_value = EEPROM.read(0x03);
  Serial.print("The values inside the EEPROM are : ");
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    Serial.print(EEPROM.read(i));
    Serial.print(" ");
  }
  Serial.println(" ");
  Serial.print("Size of the EEPROM is : ");
  Serial.println(EEPROM.length());
  */
  
	// Start up
	nRF905_init();

  
  // Set up the radio band to 433 MHz
  nRF905_setBand(NRF905_BAND_433);
	
	// Set address of this device
	nRF905_setListenAddress(RXADDR);

	// Maximum transmit level 10dbm
  nRF905_setTransmitPower(NRF905_PWR_10);
  
  // Disable loww power received
  nRF905_setLowRxPower(NRF905_LOW_RX_DISABLE);

	// Put into receive mode
	nRF905_RX();

 // delay(900000);; //Wait 1 min 30 seconds
}

void loop()
{

  Serial.print("The values inside the EEPROM are : ");
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        Serial.print(EEPROM.read(i));
        Serial.print(" ");
      }
      Serial.print(" \n");
  /*
  //Turn on the module
  nRF905_powerUp();
  // Start up
  nRF905_init();
  
  // Set up the radio band to 433 MHz
  nRF905_setBand(NRF905_BAND_433);
  
  // Set address of this device
  nRF905_setListenAddress(RXADDR);

  // Maximum transmit level 10dbm
  nRF905_setTransmitPower(NRF905_PWR_10);
  
  // Disable loww power receive
  nRF905_setLowRxPower(NRF905_LOW_RX_DISABLE);

  // Put into receive mode
  nRF905_RX();
  */  
  
	static uint32_t pings;
	static uint32_t invalids;
  static uint32_t eeprom_write;
  

	Serial.println(F("Waiting for ping..."));
  
	// Wait for data
  while(packetStatus == PACKET_NONE);

  if(packetStatus != PACKET_OK)
	{
		invalids++;
		Serial.println(F("Invalid packet!"));
		packetStatus = PACKET_NONE;
		nRF905_RX();
	}
	else
	{
		pings++;
		packetStatus = PACKET_NONE;

		// Make buffer for data
		uint8_t buffer[NRF905_MAX_PAYLOAD] ={};
		nRF905_read(buffer, sizeof(buffer));
		
		// Toggle LED
		static uint8_t ledState;
		digitalWrite(A5, ledState ? HIGH : LOW);
		ledState = !ledState;

		// Print out ping contents
		Serial.print(F("Data from server: "));
    int i;
     for (i=0 ; i< sizeof(buffer); i++){
      Serial.print(buffer[i]);
      Serial.print(" ");
    }
     //Time setting up 
    time_t current_time;
    time(&current_time);
    
    Serial.print("\n");
    /*
    tone(buzzer, 10);
    delay(500);        //wait for 0.5 sec before switch off the buzzer
    noTone(buzzer);
  */

    int moisture_yl = analogRead(A0);
    int min_yl = 1023;
    int max_yl = 340;
    byte moisture_yl_percent = map(moisture_yl, min_yl, max_yl, 0, 100); 
    /*
    Serial.print("Value of the Analog moisture (%):");
    Serial.println(moisture_yl_percent);
    */
    EEPROM.write(eeprom_write, pings); //write at the address 1 + k the number of pingg
    EEPROM.write(eeprom_write + 1, invalids); //write at the address 2 + k the number of invalids packets
    EEPROM.write(eeprom_write + 2, moisture_yl_percent); //write at the address 3 + k the moiture percent with the YL-69 soil moisture
    eeprom_write+=3;
     
    Serial.print("Received the :");
    Serial.print(ctime(&current_time));
		Serial.print("\nSize of data received :");
    Serial.print(sizeof(buffer));
    Serial.print("\n************************\n");
    Serial.print("Transmitter adress: ");
    Serial.print(buffer[0]);
    Serial.print("\nPacket type: ");
    Serial.print(buffer[1]);
    Serial.print("\nSource address: ");
    Serial.print(buffer[2]);
    Serial.print("\nDestination address: ");
    Serial.print(buffer[3]);
    Serial.print("\nTemperature DHT11 (°C): ");
    Serial.print(buffer[4]);
    Serial.print("\nMoisture DHT11(%): ");
    Serial.print(buffer[5]);
    Serial.print("\nAnalogical temperature (%): ");
    Serial.print(buffer[6]);
    Serial.print("\nMoisture YL-sensor (%): ");
    Serial.print(buffer[7]);
    Serial.print("\nMoisture Capacitive (%): ");
    Serial.print(buffer[8]);
    Serial.print("\n************************\n");
    Serial.println();

    Serial.println(F("Sending Acknowledgement "));
    buffer[1] = PACKET_TYPE_ACK ;
    // Send back the data, once the transmission has completed go into receive mode
    while(!nRF905_TX(TXADDR, buffer, sizeof(buffer), NRF905_NEXTMODE_RX));

    Serial.println(F("Reply sent"));
    int j;
     for (j=0 ; j< sizeof(buffer); j++){
      Serial.print(buffer[j]);
      Serial.print(" ");
    }
	  }
  	Serial.print(F("Totals: "));
  	Serial.print(pings);
  	Serial.print(F(" Pings, "));
  	Serial.print(invalids);
  	Serial.println(F(" Invalid"));
  	Serial.println(F("------"));

    // Turn off module
  
     //nRF905_powerDown();
     //delay (1500000); // turn off the module for 25min
  /*
  if (eeprom_write >= 50 && eeprom_write < 650){
      EEPROM.write(eeprom_write, t); //write 0 to address 50 + k
      EEPROM.write(eeprom_write + 1, h); //write 0 to address 51 + k
      EEPROM.write(eeprom_write + 2, temperature); //write 0 to address 52 + k 
      EEPROM.write(eeprom_write + 3, moisture_yl_percent); //write 0 to address 53 + k
      EEPROM.write(eeprom_write + 4, capacitive_percent); //write 0 to address 54 + k
      Serial.println(eeprom_write);
      eeprom_write++;
}*/
}
