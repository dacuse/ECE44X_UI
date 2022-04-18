// Standard Arduino library
#include <Arduino.h>

// Teros libraries
#include "SDI12.h"

// BME libraries
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// SD libraries
#include <SD.h>
#include <SPI.h>

// LoRa libraries
#include <RH_RF95.h>  

// Data manipulation
#include <string>
#include <cstring>

// Teros data wire
#define DATAPIN 11

// RTC interrupt pin
#define CLOCK_INTERRUPT_PIN 12

// LoRa pins
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define SLEEP_TIMER 15      // How long system should sleep in minutes

SDI12 mySDI12(DATAPIN);

int packet = 1;
float dielectric, temp, humid;

Adafruit_BME280 bme; // I2C
File myFile;

#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Keeps track of active addresses each bit represents an address:
// 1 is active (taken), 0 is inactive (available)
// setTaken('A') will set the proper bit for sensor 'A'
byte addressRegister[8] = {
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000,
  0B00000000
};


// Converts allowable address characters '0'-'9', 'a'-'z', 'A'-'Z',
// to a decimal number between 0 and 61 (inclusive) to cover the 62 possible addresses
byte charToDec(char i){
  if((i >= '0') && (i <= '9')) return i - '0';
  if((i >= 'a') && (i <= 'z')) return i - 'a' + 10;
  if((i >= 'A') && (i <= 'Z')) return i - 'A' + 37;
  else return i;
}

// Gets identification information from a sensor, and prints it to the serial port
// Expects a character between '0'-'9', 'a'-'z', or 'A'-'Z'.
void printInfo(char i){
  int j;
  String command = "";
  command += (char) i;
  command += "I!";
  for(j = 0; j < 1; j++){
    mySDI12.sendCommand(command);
    delay(30);
    if(mySDI12.available()>1) break;
    if(mySDI12.available()) mySDI12.read();
  }

  while(mySDI12.available()){
    char c = mySDI12.read();
    if((c!='\n') && (c!='\r')) Serial.write(c);
    delay(5);
  }
}

void printBufferToScreen(){
  String buffer = "";
  mySDI12.read(); // consume address
  mySDI12.read(); // consume address
  while(mySDI12.available()){
    char c = mySDI12.read();
    if(c == '+' || c == '-'){
      buffer += ',';
      if(c == '-') buffer += '-';
    }
    else if ((c != '\n') && (c != '\r')) {
      buffer += c;
    }
    delay(50);
  }
 char* val = strdup(buffer.c_str());
 dielectric = pow(2.887*pow(10, -9)*pow(atof(val), 3)-2.080*pow(10, -5)*pow(atof(val),2)+5.276*pow(10,-2)*atof(val)-43.39, 2);
 Serial.print("Dielectric: ");
 Serial.println(dielectric);
}

void takeMeasurement(char i){
  String command = "";
  command += i;
  command += "M!"; // SDI-12 measurement command format  [address]['M'][!]
  mySDI12.sendCommand(command);
  // wait for acknowlegement with format [address][ttt (3 char, seconds)][number of measurments available, 0-9]
  String sdiResponse = "";
  delay(30);
  while (mySDI12.available())  // build response string
  {
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r'))
    {
      sdiResponse += c;
      delay(5);
    }
  }
  mySDI12.clearBuffer();

  // find out how long we have to wait (in seconds).
  unsigned int wait = 0;
  wait = sdiResponse.substring(1,4).toInt();

  // Set up the number of results to expect
  // int numMeasurements =  sdiResponse.substring(4,5).toInt();

  unsigned long timerStart = millis();
  while((millis() - timerStart) < (1000 * wait)){
    if(mySDI12.available())  // sensor can interrupt us to let us know it is done early
    {
      mySDI12.clearBuffer();
      break;
    }
  }
  // Wait for anything else and clear it out
  delay(30);
  mySDI12.clearBuffer();

  // in this example we will only take the 'DO' measurement
  command = "";
  command += i;
  command += "D0!"; // SDI-12 command to get data [address][D][dataOption][!]
  mySDI12.sendCommand(command);
  while(!mySDI12.available()>1); // wait for acknowlegement
  delay(300); // let the data transfer
  printBufferToScreen();
  mySDI12.clearBuffer();
}

// this checks for activity at a particular address
// expects a char, '0'-'9', 'a'-'z', or 'A'-'Z'
boolean checkActive(char i){

  String myCommand = "";
  myCommand = "";
  myCommand += (char) i;                 // sends basic 'acknowledge' command [address][!]
  myCommand += "!";

  for(int j = 0; j < 3; j++){            // goes through three rapid contact attempts
    mySDI12.sendCommand(myCommand);
    if(mySDI12.available()>1) break;
    delay(30);
  }
  if(mySDI12.available()>2){       // if it hears anything it assumes the address is occupied
    mySDI12.clearBuffer();
    return true;
  }
  else {   // otherwise it is vacant.
    mySDI12.clearBuffer();
  }
  return false;
}

// this quickly checks if the address has already been taken by an active sensor
boolean isTaken(byte i){
  i = charToDec(i); // e.g. convert '0' to 0, 'a' to 10, 'Z' to 61.
  byte j = i / 8;   // byte #
  byte k = i % 8;   // bit #
  return addressRegister[j] & (1<<k); // return bit status
}

// this sets the bit in the proper location within the addressRegister
// to record that the sensor is active and the address is taken.
boolean setTaken(byte i){
  boolean initStatus = isTaken(i);
  i = charToDec(i); // e.g. convert '0' to 0, 'a' to 10, 'Z' to 61.
  byte j = i / 8;   // byte #
  byte k = i % 8;   // bit #
  addressRegister[j] |= (1 << k);
  return !initStatus; // return false if already taken
}

// BME280 measurement function
void bme_measure() {

    bme.begin(0x76);

    temp = bme.readTemperature();
    humid = bme.readHumidity();
 
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println(" %");
  
}

void sd_log() {

    SD.begin(10);

    if (packet == 1) {
        if (!SD.begin(10)) {
            Serial.println("SD failed to initialize");
            return;
        }

        myFile = SD.open("test.csv", FILE_WRITE);

        if (myFile) {
            myFile.println("Packet,Temperature,Humidity,Dielectric");
            myFile.close();
            Serial.println("SD initialized");
        }
    }

    myFile = SD.open("test.csv", FILE_WRITE);

    if (myFile) {
      // Packet Number
      myFile.print(packet);
      myFile.print(",");

      // Temperature
      myFile.print(temp);
      myFile.print(",");

      // Humidity
      myFile.print(humid);
      myFile.print(",");

      // Electrical Conductivity 
      myFile.print(dielectric);
      myFile.println(",");

      Serial.println("Logged to SD");
      myFile.close();
      return;
    }
    Serial.println("Log failed");
}

void setup(){
  
  Serial.begin(9600);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW); // Sets pin 5, the pin with the 3.3V rail, to output and enables the rail
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH); // Sets pin 6, the pin with the 5V rail, to output and enables the rail
 
  // LoRa: Manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  mySDI12.begin();
  delay(500); // allow things to settle

  Serial.println("Scanning all addresses, please wait...");
  /*
      Quickly Scan the Address Space
   */

  for(byte i = '0'; i <= '9'; i++) if(checkActive(i)) setTaken(i);   // scan address space 0-9

  for(byte i = 'a'; i <= 'z'; i++) if(checkActive(i)) setTaken(i);   // scan address space a-z

  for(byte i = 'A'; i <= 'Z'; i++) if(checkActive(i)) setTaken(i);   // scan address space A-Z

  /*
      See if there are any active sensors.
   */
  boolean found = false;

  for(byte i = 0; i < 62; i++){
    if(isTaken(i)){
      found = true;
      Serial.print("Found address:  ");
      Serial.println(i);
      break;
    }
  }
}

void loop(){

  // scan address space 0-9
  for(char i = '0'; i <= '9'; i++) if(isTaken(i)){
    takeMeasurement(i);
  }

  // scan address space a-z
  for(char i = 'a'; i <= 'z'; i++) if(isTaken(i)){
    takeMeasurement(i);
  }

  // scan address space A-Z
  for(char i = 'A'; i <= 'Z'; i++) if(isTaken(i)){
    takeMeasurement(i);
  };

  bme_measure();
  sd_log();

  // Form URL with data
  String temp_link = "https://web.engr.oregonstate.edu/~eaglea/add_data.php?temp=" + String(temp) + "&hum=" + String(humid) + "&pr=" + String(dielectric);
  int link_len = temp_link.length() + 1;
  char converted_value[link_len];
  temp_link.toCharArray(converted_value, link_len);

  Serial.println(converted_value);

  // Send URL through LoRa
  rf95.send((uint8_t *)converted_value, link_len);
  delay(10);
  rf95.waitPacketSent();

  sleep();
}

void sleep() {

  digitalWrite(5, HIGH); // Disabling all pins before going to sleep.
  digitalWrite(6, LOW);
  pinMode(23, INPUT);    // Disables SPI communication to SD before going to sleep
  pinMode(24, INPUT);
  pinMode(10, INPUT);

  pinMode(DATAPIN, INPUT);

  for (int i = 0; i < SLEEP_TIMER; i++) {
    delay(60000);
    Serial.println("Minutes passed: " + String(i+1));
  }

  digitalWrite(5, LOW); // Enabling all pins after wake up has completed.
  digitalWrite(6, HIGH);
  pinMode(10, OUTPUT);  // Enables SPI communication to SD after going to sleep
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);

  pinMode(DATAPIN, OUTPUT);

  delay(10);
}
