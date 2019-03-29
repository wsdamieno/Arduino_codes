#include <nRF905.h>
#include <EEPROM.h>

#define RXADDR 0x58 // Address of this device
#define TXADDR 0xFE // Address of device to send to

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

//Type of packet
#define PACKET_TYPE_DATA  0
#define PACKET_TYPE_ACK   1

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
	Serial.println(F("Server started"));

	//pinMode(A5, OUTPUT); // LED

  delay(120000); //Wait 120 seconds before starting
  
  // Start up
	nRF905_init();
	
	// Set address of this device
	nRF905_setListenAddress(RXADDR);
 
  // Disable loww power received
  nRF905_setLowRxPower(NRF905_LOW_RX_DISABLE);

	// Put into receive mode
	nRF905_RX();
}

void loop()
{
	static uint32_t pings;
	static uint32_t invalids;
  static uint32_t eeprom_write;

	Serial.println(F("Waiting for ping..."));
  int moisture_yl = analogRead(A0);
  int min_yl = 1023;
  int max_yl = 340;
  byte moisture_yl_percent = map(moisture_yl, min_yl, max_yl, 0, 100); 
   Serial.print("Moisture :"); 
   Serial.print(moisture_yl);
   
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
    
    Serial.println(F("Sending Acknowledgement "));
    buffer[1] = PACKET_TYPE_ACK ;

		Serial.println(F("Got ping, sending reply..."));

    
		// Send back the data, once the transmission has completed go into receive mode
		nRF905_TX(TXADDR, buffer, sizeof(buffer), NRF905_NEXTMODE_RX);

    
    EEPROM.write(eeprom_write, pings); //write at the address 1 + k the number of pingg
    EEPROM.write(eeprom_write + 1, invalids); //write at the address 2 + k the number of invalids packets
    EEPROM.write(eeprom_write + 2, moisture_yl_percent); //write at the address 3 + k the moiture percent with the YL-69 soil moisture
    eeprom_write+=3;
    
    Serial.print(" Values of the EEPROM pointer : ");
    Serial.println(eeprom_write);

    /*
		Serial.println(F("Reply sent"));

    // Toggle LED
		static uint8_t ledState;
		digitalWrite(A5, ledState ? HIGH : LOW);
		ledState = !ledState;

		// Print out ping contents
		Serial.print(F("Data from server: "));
		Serial.write(buffer, sizeof(buffer));
		Serial.println();
   */
	}
    
    
    
    
	Serial.print(F("Totals: "));
	Serial.print(pings);
	Serial.print(F(" Pings, "));
	Serial.print(invalids);
	Serial.println(F(" Invalid"));
	Serial.println(F("------"));
}
