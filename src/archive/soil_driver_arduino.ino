/*******
 * Marley Bennett
 * soil_driver
 * 
 * Resources: https://wetlandsandr.wordpress.com/2015/09/12/building-a-soil-moisture-and-conductivity-probe/
 * https://github.com/joranbeasley/SDISerial
 * Requirements: import <SDISerial.h> from above git repo and save to library folder
 * 
 * Hardware setup:
 * Brown soil moisture wire: 5V
 * Stripped soil moisture wire: GND
 * Yellow/Orange wire: Digital Pin 2
 */


int incomingByte = 0;
char* data;

#include <SDISerial.h>

#define DATA_PIN 7
#define NUM_SAMPLES 5
#define INVERTED 1

int sensorDelay = 30000;
char* samples;
long totMeasurement = 0;
int numMeasurements = 0;


SDISerial sdi_serial_connection(DATA_PIN, INVERTED);

void setup() {
  sdi_serial_connection.begin();
  Serial.begin(9600);
  long init = millis();
  Serial.print("starting time..: ");
  Serial.println(init);
  
  // initial delay for sensor startup
  delay(145);
}

void loop() {
  
  float dielectric[NUM_SAMPLES];
  float degC[NUM_SAMPLES];

  float dielectricMean = 0;
  float degCMean = 0;

  for(uint8_t i = 0; i < NUM_SAMPLES; i++){
      samples = get_measurement();

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

  delay(1000);
}

char* get_measurement(){
  
  // request instantenous values in the form 
  // a+<calibratedCountsVWC>+-<temperature>
  long initTime = millis();
  char* serviceRequest = sdi_serial_connection.sdi_query("?M!", sensorDelay);

  // wait for response
  char* serviceRequestComplete = sdi_serial_connection.wait_for_response(sensorDelay);

  if(serviceRequestComplete == NULL){
    
    Serial.println("ERROR: Sensor is not responding");
  }
  else{
    //Serial.print("Time since start: ");
    //Serial.println(millis());
  }

  long time = millis();
   //Serial.print("time to take measurement: ");
    //Serial.println((time - initTime));
    totMeasurement += (time-initTime);
    numMeasurements++;
    //Serial.print("avg measurment: ");
    //Serial.println(totMeasurement/numMeasurements);
  // return data from the sensor
  return sdi_serial_connection.sdi_query("?D0!", sensorDelay);
}