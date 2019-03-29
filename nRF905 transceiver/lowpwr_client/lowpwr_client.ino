#include <nRF905.h>
#include <SimpleDHT.h>
#include <time.h>
#include <EEPROM.h>

/*
//Libraries for I2C lcd screen
#include <Wire.h> 
#include <hd44780.h>                       
#include <hd44780ioClass/hd44780_I2Cexp.h> 

//Parameters for the LCD 
hd44780_I2Cexp lcd;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
*/ 

#define TXADDR 0xFE // Address of this device (transmitter)
#define RXADDR 0x58 // Address of device to send to (receiver)

#define TIMEOUT 30000 // 30 second ping timeout

//State of packet
#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

//Type of packet
#define PACKET_TYPE_DATA  0
#define PACKET_TYPE_ACK   1

// DHT11 parameters 
int dht_dpin = 5;  
SimpleDHT11 DHT_sens(dht_dpin);           //datapin sensor to pin 5 Arduino

//Packet structure
//#define MAX_PACKET_SIZE (NRF905_MAX_PAYLOAD - 10)
/*
typedef struct {
  byte srcAddress;
  byte dstAddress;
  byte type;
  byte len;
  byte data[MAX_PACKET_SIZE];
} packet_t;
*/
static volatile bool receivedNewData;
static volatile byte newData[NRF905_MAX_PAYLOAD];
static volatile uint8_t packetStatus;

uint8_t i;
void NRF905_CB_RXCOMPLETE(void)
{
	receivedNewData = true;
  nRF905_read((byte*)newData, sizeof(newData));
  Serial.print("Acknowledgement receiveid ");
    for (i = 0; i<sizeof(newData); i++){
        Serial.print(newData[i]);
        Serial.print(" ");
    }
    Serial.print("\n");
	packetStatus = PACKET_OK;
}

void NRF905_CB_RXINVALID(void)
{
	packetStatus = PACKET_INVALID;
}

void setup()
{
	Serial.begin(115200);

 /*Serial.print("The values inside the EEPROM are : ");
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        Serial.print(EEPROM.read(i));
        Serial.print(" ");
      }
      Serial.print(" \n");
*/
  /*
  //Initialisation of the lcd
  int status;
  
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if(status) // non zero status means it was unsuccesful
  {
    status = -status; // convert negative status value to positive number

    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  }

  // Initialize the I2C link  
  Wire.begin();
  
  //pinMode(A5, OUTPUT); // LED
*/

  Serial.print("The values inside the EEPROM are : ");
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        Serial.print(EEPROM.read(i));
        Serial.print(" ");
      }
      Serial.print(" \n");

	// Start up
	nRF905_init();

  // Set up the radio band to 433 MHz
  nRF905_setBand(NRF905_BAND_433);
	
	// Set address of this device
	nRF905_setListenAddress(TXADDR);

	// Maximum transmit level 10dbm
	nRF905_setTransmitPower(NRF905_PWR_10);
	
	// Disable low power receive
	nRF905_setLowRxPower(NRF905_LOW_RX_DISABLE);

  //Wait 2 min before start the loop part
  //delay(1200000); //Wait 2 min 

    
}

byte hop_count = 2;

void loop()
{
  /*
  //Turn on the module
  nRF905_powerUp();

  // Start up
  nRF905_init();

  // Set up the radio band to 433 MHz
  nRF905_setBand(NRF905_BAND_433);
  
  // Set address of this device
  nRF905_setListenAddress(TXADDR);

  // Maximum transmit level 10dbm
  nRF905_setTransmitPower(NRF905_PWR_10);
  
  // Disable low power receive
  nRF905_setLowRxPower(NRF905_LOW_RX_DISABLE);
  */
	static uint8_t counter;
  static uint32_t sent;
 	static uint32_t replies;
	static uint32_t timeouts;
	static uint32_t invalids;
  static uint32_t eeprom_write;

  byte h = 0;
  byte t = 0;
  float temperature = 0;
  
  //Compute the temperature with the LMD35DZ
  temperature = analogRead(A0)*0.48828125 ;
  DHT_sens.read(&t, &h, NULL);
  
  Serial.print("Temperature :");
  Serial.print(t);
  Serial.println(" C");

  Serial.print("Humidity :");
  Serial.print(h);
  Serial.println(" %");

  int moisture_yl = analogRead(A1);
  int min_yl = 1023;
  int max_yl = 340;
  byte moisture_yl_percent = map(moisture_yl, min_yl, max_yl, 0, 100); 
  
  int capacitive = analogRead(A2);
  int min_capa = 572;
  int max_capa = 298;
  byte capacitive_percent = map (capacitive, min_capa, max_capa, 0, 100);
  
  
  Serial.print("**************************\n");
  Serial.print("Soil Moisture YL-69 : ");
  Serial.print(moisture_yl_percent);
  Serial.println(" %");
  Serial.print("\n**************************\n");
  Serial.print("Capacitive Soil Moisture V1.2 : ");
  Serial.print(capacitive_percent);
  Serial.println(" %");
  Serial.print("\n**************************\n");
  
  /*
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humidity 1: ");
  lcd.print(moisture_yl_percent);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Humidity 2: ");
  lcd.print(capacitive_percent);
  lcd.print("%");
  */
  //Create packet data structure called packet_data
  //packet_t packet_data;

	packetStatus = PACKET_NONE;
  /*
  packet_data.dstAddress = RXADDR;
  packet_data.srcAddress = TXADDR;

  packet_data.type = PACKET_TYPE_DATA;
  //packet_data.data[0] = temperature;
  packet_data.data[0] = moisture_yl_percent;
  packet_data.data[1] = capacitive_percent;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Src -> Dest");
  lcd.setCursor(0, 1);
  lcd.print(TXADDR);
  lcd.print(" -> ");
  lcd.print(packet_data.dstAddress);
  //delay(2000);
  */
	uint32_t startTime = millis();
  
  /*
	// Send the data (send fails if other transmissions are going on, keep trying until success) and enter RX mode on completion
	lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending to");
  lcd.setCursor(0, 1);
  lcd.print("Dest -> ");
  lcd.print(RXADDR);
  */

  //Send the data_structure packet
	//sendPacket(&packet_data, hop_count, t, h, RXADDR);

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

  if (nRF905_TX(RXADDR, tmpBuff, sizeof(tmpBuff), NRF905_NEXTMODE_RX)){
    /*
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.setCursor(0, 1);
    lcd.print("To ");lcd.print(NextAdress); 
    // Toggle LED
    */
    Serial.print("Packet sent : ");
    for (i = 0; i<NRF905_MAX_PAYLOAD; i++){
        Serial.print(tmpBuff[i]);
        Serial.print(" ");
    }
    Serial.print("\n");
    sent++;
    //delay(500);
   }else{
    /*
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Packet not sent");
    */
    Serial.print("\n");
    Serial.print(" Packet not sent ");
    //delay(500);
  }
  
 	Serial.println(F("Data sent, waiting for reply ..."));

	uint8_t success;

	// Wait for reply with timeout
	uint32_t sendStartTime = millis();
	while(1)
	{
		success = packetStatus;
		if(success != PACKET_NONE)
			break;
		else if(millis() - sendStartTime > TIMEOUT)
			break;
	}

	if(success == PACKET_NONE){
    /*
    lcd.print("Ping timeout");
    */
		Serial.println(F("Ping timed out"));
		timeouts++;
	}
	else if(success == PACKET_INVALID)
	{
		Serial.println(F("Invalid packet!"));
		invalids++;
	}
	else
	{
		// If success toggle LED and send ping time over UART
		uint16_t totalTime = millis() - startTime;

		static uint8_t ledState;
		digitalWrite(A5, ledState ? HIGH : LOW);
		ledState = !ledState;

		replies++;

		Serial.print(F("Ping time: "));
		Serial.print(totalTime);
		Serial.println(F(" ms"));
	}
  
  
	//Turn off module
  
	//nRF905_powerDown();

	Serial.print(F("Totals: "));
	Serial.print(sent);
	Serial.print(F(" Sent, "));
	Serial.print(replies);
	Serial.print(F(" Replies, "));
	Serial.print(timeouts);
	Serial.print(F(" Timeouts, "));
	Serial.print(invalids);
	Serial.println(F(" Invalid"));
	Serial.println(F("------"));
/*
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tmp: ");
  lcd.print(temperature);
  lcd.print("/");
  lcd.print(t);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(h);
  lcd.print(" %");
*/
  //send the corresponding data
  //delay(4000);
    if (eeprom_write <= 145){
    EEPROM.write(eeprom_write, t); //write 0 to address 50 + k
    EEPROM.write(eeprom_write + 1, h); //write 0 to address 51 + k
    EEPROM.write(eeprom_write + 2, temperature); //write 0 to address 52 + k 
    EEPROM.write(eeprom_write + 3, moisture_yl_percent); //write 0 to address 53 + k
    EEPROM.write(eeprom_write + 4, capacitive_percent); //write 0 to address 54 + k
    EEPROM.write(eeprom_write + 5, sent); //write at the address 10 + k the number of sent packets
    EEPROM.write(eeprom_write + 6, timeouts); //write at the address 10 + k the number of sent packets

    Serial.println(" Values of the EEPROM pointer : ");
    Serial.println(eeprom_write);
    }
    eeprom_write+=7;
    
    
      
    Serial.print("The values inside the EEPROM are : ");
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        Serial.print(EEPROM.read(i));
        Serial.print(" ");
      }
      Serial.print(" \n");

      // Turn off the module
      
      nRF905_powerDown();
      //delay (1800000); // Wait 30 min before send data

}
/*

// Send a packet
static void sendPacket(void* _packet, byte hop_number, byte temperature, byte moisture, byte NextAdress)
{
  // Void pointer to packet_t pointer hack
  // Arduino puts all the function defs at the top of the file before packet_t being declared :/
  packet_t* packet = (packet_t*)_packet;
  static uint32_t sent;
  //Convert packet data to plain byte array
  //byte totalLength = packet->len + 6;
  
  
  memcpy(&tmpBuff[6], packet->data, packet->len);
  //memcpy(packet->data, (byte*)&newData[2], packet->len);

  // Set address of device to send to
  //nRF905_setTXAddress(packet->dstAddress);

  // Send payload (send fails if other transmissions are going on, keep trying until success)
  //while(!nRF905_TX(NextAdress, tmpBuff, totalLength, NRF905_NEXTMODE_STANDBY));
  if (nRF905_TX(NextAdress, tmpBuff, NRF905_MAX_PAYLOAD, NRF905_NEXTMODE_RX)){
    lcd.clear();
    lcd.setCursor(0, 0);
    Serial.println("Packet sent !");
    lcd.setCursor(0, 1);
    lcd.print("To ");lcd.print(NextAdress); 
    // Toggle LED
    sent++;
    //delay(3000);

    int i;
    
    Serial.print("Data Value of position 6: ");
    Serial.print(tmpBuff[6]);
    //Serial.println(" o");
    
    
    
    Serial.print("\n");
    Serial.print(" Size of Data ");
    Serial.print(sizeof(tmpBuff));Serial.print("\n");
        
    //delay(3000);
  }else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Packet not sent");
    Serial.print("\n");
    Serial.print(" Packet not sent ");
    delay(3000);
  }
}
  */
