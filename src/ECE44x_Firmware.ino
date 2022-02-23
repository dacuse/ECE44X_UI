/***************************************************************************
* ECE44x_Firmware: Environmental sensing group's firmware implementing the
*   I2C BME280 temperature/humidity sensor as well as SD module for data
*   logging. 
* TODO: Add Teros11 soil moisture sensor for data logging
* Author: Bryson Goto, 2/23/2022
****************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <SPI.h>
#include <SD.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
File myFile;

int packet = 1;

float temp, humid, pressure, alt;

void setup() {

    Serial.begin(9600);
    while(!Serial);

    // Setting up BME280
    
    unsigned status;
    status = bme.begin(0x76);  
    
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

    Serial.println("BME280 initialized");

    // Setting up SD
    
    if (!SD.begin(10)) {
      Serial.println("SD initialization failed!");
      while (1);
    }
    Serial.println("SD initialization done.");

    myFile = SD.open("test.csv", FILE_WRITE);

    if (myFile) {
      Serial.print("Writing to test.csv...");
      myFile.println("Packet, Temperature, Humidity, Pressure, Altitude, Soil_Moisture");
      // close the file:
      myFile.close();
      Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.csv");
    }
    
    Serial.println();
}


void loop() { 

    printValues_BME();
    sd_log();
    delay(1000);

    packet++;

}

void printValues_BME() {

    temp = bme.readTemperature();
    humid = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;
    alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(pressure);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(alt);
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(humid);
    Serial.println(" %");

    Serial.println();
}

void sd_log() {

    myFile = SD.open("test.csv", FILE_WRITE);

    if (myFile) {
      Serial.print("Writing data");
      
      myFile.print(packet);
      myFile.print(",");

      myFile.print(temp);
      myFile.print(",");

      myFile.print(humid);
      myFile.print(",");

      myFile.print(pressure);
      myFile.print(",");

      myFile.print(alt);
      myFile.print(",");

      myFile.print(0);
      myFile.println(",");
      
      // close the file:
      myFile.close();
      Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.csv");
    }
    
}
