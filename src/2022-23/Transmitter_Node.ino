/*
 * Transmitter Code for ECE 44x Senior Capstone Project 2022-23
 * By Garren Dutto, Blake Garcia, Emma Dacus, and Grace Mackey.
 * 
 * Module uses an Arduino Nano Every with the RFM95 breakout board
 * from Adafruit, 
 * (TODO: INSERT SENSOR PARTS TO COMMENT)
 * 
 * Connections: 
 * D0 - 
 * D1 - 
 * D2 - RFM95 Pin G0 (Interrupt Request)
 * D3 -
 * D4 - 
 * D5 - 
 * D6 - 
 * D7 - 
 * D8 - 
 * D9 - RFM95 Reset
 * D10 - RFM95 SS
 * D11 - MOSI
 * D12 - MISO
 * D13 - SCK
 * 
 * A0 - 
 * A1 - 
 * A2 - 
 * A3 - 
 * A4 - 
 * A5 - 
 * A6 - 
 * A7 - 
 */

 #include <SPI.h>
 #include <RH_RF95.h>

 #define RFM95_CS 10 //SS pin for RFM95
 #define RFM95_RST 9
 #define RFM95_INT 2

 #define RF95_FREQ 923.0

//Singleton instance of the radio driver
 RH_RF95 rf95(RFM95_CS, RFM95_INT);

unsigned int address; //Node's address

unsigned int addr_list[256]; //list of nodes connected to this one
int num_addr; //Number of nodes connected to this one

/*Function: hexToDec
 * Description: Takes in a hex character as input and returns its decimal equivalent
 * Parameters: Character from 0-F
 */
int hexToDec(char c){
  if(c >= '0' && c <= '9'){
    return c - '0';
  }
  switch (c){
    case 'A':
      return 10;
    case 'B':
      return 11;
    case 'C':
      return 12;
    case 'D':
      return 13;
    case 'E':
      return 14;
    case 'F':
      return 15;
    default:
      return 0;
  }
}


/*******************************************************************************
 * Function: address_request
 * Description: Handles an address request from another node by checking the 
 * contents and whether this node has already seen the request, then appending
 * its address if it has not.
 * Parameters: array of bytes that contains the address request message
 * Return value: none
 *******************************************************************************/
 void address_request(uint8_t buf[]){

  //Two cases: from the base station and from another node
  //Determine the source by finding the "S:" part of the transmission
  int i = 0;
  while(buf[i] != 'S' || buf[i+1] != ':'){
    i++;
  }

  //Pull out the sender's address
  unsigned int sender = 0;
  sender += 16 * 16 * 16 * hexToDec(buf[i+2]);
  sender += 16 * 16 * hexToDec(buf[i+3]);
  sender += 16 * hexToDec(buf[i+4]);
  sender += hexToDec(buf[i+5]);

  //If the base station was the one that sent the message, check to see if this station is on the intended path
  

 }

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(7));
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  num_addr = 0;

  //Initialize Serial Monitor
  while (!Serial);
  Serial.begin(9600);
  delay(100);

  //Reset the radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Call the initialization routine
  while (!rf95.init()){
    Serial.println("LoRa radio init failed");
    while(1);
  }
  Serial.println("Lora radio initialized");

  //Set the frequency to the specified value
  if(!rf95.setFrequency(RF95_FREQ)){
    Serial.println("Failed to set frequency");
    while(1);
  }
  Serial.println("Set Freqency success");

  //Set transmitting power, can be anywhere from 5 to 23 dBm
  rf95.setTxPower(23, false);

  //Send out address request
  address = 0;
  while(address == 0){
    uint8_t msg_id = random(32, 127); //Random ASCII identifier to prevent multiple receiving paths to host
    uint8_t msg[] = "0 x S:FFFF D:0000 R:"; //0 identifier is for address request, 1 is for sensor reading, 2 is for error
    msg[2] = msg_id; 
    Serial.print("Address request message: ");
    Serial.print((char*)msg);
    rf95.send(msg, sizeof(msg));
    rf95.waitPacketSent();

    delay(5000); //wait 5 seconds for a response
    if(rf95.available()){
      //The structure of the message should be "0 x S:0000 D:XXXX" with XXXX being the new address in hex
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      //Make sure that the message is received properly
      if(rf95.recv(buf, &len) && buf[2] == msg_id){
        //Parse through the message for the address and store it in the address variable
        int i = 0;
        while(buf[i] != 'D' || buf[i+1] != ':'){
          i++;
        }
        address = 16 * 16 * 16 * hexToDec(buf[i+2]);
        address += 16 * 16 * hexToDec(buf[i+3]);
        address += 16 * hexToDec(buf[i+4]);
        address += hexToDec(buf[i+5]);
      }
    }

    /*If no address reply was received, the node will loop every 5 seconds and send a new
     * address request until it gets a reply.
     */
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

  //Check to see if a message has been received
  if(rf95.available()){

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    //Read in the message
    if(rf95.recv(buf, &len)){
      //Check the first byte of the transmission. If it is 0, then it is an address request
      //If it is 1, then it is a data transmission.
      switch(buf[0]){
        case '0': 
          //Append node's address to the message and then broadcast if it is not from base station
          //If it is from base station, check to see if this is the intended recipient
          address_request(buf);
          break;
        case '1':
          //Check if the address is in the sender's address list and broadcast the message if it is
          break;
        case '2':
          //Broadcast the error message
          break;
        default:
          //something must have gone wrong with the packet, so drop it
      }
    }else{
      //Something went wrong reading in the message, honestly idk what to put here
    }
  }

  delay(5000);

}
