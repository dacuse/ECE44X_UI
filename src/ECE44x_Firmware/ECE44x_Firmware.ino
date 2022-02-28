/***************************************************************************
* ECE44x_Firmware.ino: Environmental sensing group's firmware implementing the
*   I2C BME280 temperature/humidity sensor as well as SD module for data
*   logging. 
* Author: Bryson Goto, 2/23/2022
****************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <SPI.h>
#include <SD.h>
#include <SDISerial.h>

#define NUM_SAMPLES 5

short int packet = 1;
float temp, humid;
char* samples;

void setup() {
    Serial.begin(115200);
    while(!Serial);
}

void loop() { 

    Adafruit_BME280 bme; // I2C
    File myFile;
    SDISerial sdi_serial_connection(7, 1);
    
    float dielectric[NUM_SAMPLES];
    float degC[NUM_SAMPLES];

    float dielectricMean = 0;
    float degCMean = 0;

    for(uint8_t i = 0; i < NUM_SAMPLES; i++){
        samples = get_measurement(sdi_serial_connection);

      // get sensor address and disregard
      char* val = strtok(samples, "+");

      // get volumetric water content (string -> floating point)
      val = strtok(NULL, "+");
      dielectric[i] = pow(2.887*pow(10, -9)*pow(atof(val), 3)-2.080*pow(10, -5)*pow(atof(val),2)+5.276*pow(10,-2)*atof(val)-43.39, 2);

      // get temperature (string -> floating point)
      val = strtok(NULL, "+");
      degC[i] = atof(val);


      // sum total for mean
      dielectricMean += dielectric[i];
      degCMean += degC[i]; 
    }

    // calculate mean
    dielectricMean /= NUM_SAMPLES;
    degCMean /= NUM_SAMPLES;

    // print results
    Serial.print("Dielectric: ");
    Serial.println(dielectricMean);
    Serial.print("Degrees Celsius: ");
    Serial.println(degCMean);

    bme_measure(bme);
    sd_log(myFile);

    packet++;
    delay(1000);
}

// Teros11 function: Taking data measurement
char* get_measurement(SDISerial sdi_serial_connection){

  sdi_serial_connection.begin();

  // request instantenous values in the form 
  // a+<calibratedCountsVWC>+-<temperature>
  
  char* serviceRequest = sdi_serial_connection.sdi_query("?M!", 30000);

  // wait for response
  char* serviceRequestComplete = sdi_serial_connection.wait_for_response(30000);

  if(serviceRequestComplete == NULL)
    Serial.println("ERROR: Sensor is not responding");
  
  // return data from the sensor
  return sdi_serial_connection.sdi_query("?D0!", 30000);
}

// BME280 measurement function
void bme_measure(Adafruit_BME280 bme) {

    bme.begin(0x76);

    temp = bme.readTemperature();
    humid = bme.readHumidity();
 
    Serial.print("Temp:");
    Serial.println(temp);
    Serial.println("C");

    Serial.print("Humidity=");
    Serial.println(humid);
    Serial.println("%");
  
}

void sd_log(File myFile) {

    if (packet == 1) {
        if (!SD.begin(10)) 
            return;

        myFile = SD.open("test.csv", FILE_WRITE);

        if (myFile) {
            myFile.println("Packet,Temperature,Humidity,Soil_Moisture");
            myFile.close();
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
      myFile.print(samples);
      myFile.println(",");
      
      myFile.close();
      return;
    }

}