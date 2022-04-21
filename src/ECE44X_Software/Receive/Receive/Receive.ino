// Standard Arduino library
#include <Arduino.h>

// LoRa libraries
#include <SPI.h>
#include <RH_RF95.h>

// RTC library
#include "RTClib.h"

// LoRa pins
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// RTC instance
RTC_DS3231 rtc;

#define CLOCK_INTERRUPT_PIN 12

void onAlarm() {
    Serial.println("Alarm occured!");
}

void alarmSet() {

    // schedule an alarm 10 seconds in the future
    if(!rtc.setAlarm1(
            //rtc.now() + TimeSpan(10),
            rtc.now() + TimeSpan(0, 0, 15, 2),
            DS3231_A1_Minute // this mode triggers the alarm when the seconds match. See Doxygen for other options
    )) {
        Serial.println("Error, alarm wasn't set!");
    }else {
        Serial.println("Alarm will happen in 1 minute and 2 seconds!");  
    }
  
}

void setup() {

  Serial.begin(9600);

  // RTC setup
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // We don't need the 32K Pin, so disable it
  rtc.disable32K();

  // Making it so, that the alarm will trigger an interrupt
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);
  
  // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
  // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  
  // stop oscillating signals at SQW Pin
  // otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  rtc.disableAlarm(2);

  // LoRa setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

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

  // Setting LoRa settings
  rf95.setTxPower(23, false);        // Full power
  rf95.setSignalBandwidth(125000);   // Bandwidth = 125kHz
  rf95.setSpreadingFactor(10);       // Spreading factor 2^10 = 1024
  rf95.setCodingRate4(8);            // Coding rate: 4/8
}

void loop() {
  
  if (rf95.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
        // Print URL and set a new alarm
        Serial.println((char*)buf);
        alarmSet();
    }
  }

  if(rtc.alarmFired(1)) {

      // Print statement for error
      Serial.println("Error");

      // Clear alarm and set a new one
      rtc.clearAlarm(1);
      alarmSet();
      
  }
}
