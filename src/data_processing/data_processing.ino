#define TEMP 25
#define HUMID 80
#define EC 50

#define DELAY 1000

#include <string.h>

String URL;

void setup() {

  Serial.begin(115200);
  while(!Serial);

}

void loop() {

    URL = "https://web.engr.oregonstate.edu/~eaglea/add_data.php?temp=" + String(TEMP) + "&hum=" + String(HUMID) + "&pr=" + String(EC);

    for (int i = 0; i < 10; i++) {
        wait();
        Serial.println(String(i+1) + " minutes have passed");
    }

    Serial.println(URL);

}

void wait() {

    for (int i = 0; i < 60; i++) {
        delay(DELAY);
        Serial.println(String(i+1) + " seconds have passed");
    }

}