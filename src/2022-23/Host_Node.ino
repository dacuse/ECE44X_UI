/*
 * Host Node code for ECCE 44x Senior Capstone Project 2022-23
 * By Garren Dutto, Blake Garcia, Emma Dacus, and Grace Mackey
 * Oregon State University
 * 
 * Module uses an Arduino Nano Every with the RFM95 breakout board
 * from Adafruit, connected to a computer with the appropriate data
 * processing code so that sensor data can be uploaded to the website
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
 * D10 - RFM95 CS
 * D11 - MOSI
 * D12 - MISO
 * D13 - SCK
 */

 #include <SPI.h>
 #include <RH_RF95.h>

 #define RFM95_CS 10 //CS pin for RFM95
 #define RFM95_RST 9 //Reset pin
 #define RFM95_INT 2 //Interrupt pin, MUST be pin 2

 #define RF95_FREQ 923.0 //Operating frequency

 //Singleton instance of the radio driver
 RH_RF95 rf95(RFM95_CS, RFM95_INT);

//Address catalog
unsigned int addresses[256];


void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(7));
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //Initialize Serial monitor
  while(!Serial);
  Serial.begin(9600);
  delay(100);

  //Reset the radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //Call the radio initialization routine
  while(!rf95.init()){
    Serial.println("LoRa radio init failed");
    while(1);
  }
  Serial.println("LoRa radio initialized");

  //Set the freqency
  if(rf95.setFrequency(RF95_FREQ)){
    Serial.println("Failed to set frequency");
    while(1);
  }
  Serial.println("Set Frequency success");

  //Set transmitting power, minimum 5 and max 23 dBm
  rf95.setTxPower(23, false);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  //Check to see if a message is available
  if(rf95.available()){

    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    //read in the message
    if(rf95.recv(buf, &len)){
      //Read the first byte, if it is a 0, then send an available address and the path
      //If it is a 1, read in the data and process it
      //If it is a 2, notify the website
      switch(buf[0]){
        case '0':
          //Find the first available address and attach it into the message
          break;
        case '1':
          //Read in the data and parse through it
          break;
        case '2':
          //Read the error and report to the website
          break;
        default:
         //Something must have gone wrong with the packet, so drop it
      }
    }else{
      //Something went wrong reading in the message
    }
  }
}
