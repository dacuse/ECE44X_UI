
## Oregon State University ECE Senior Capstone: Integrated Environmental Sensor for Thai Agriculture Application
Project Members: [Aspen Eagle](eaglea@oregonstate.edu), [Samantha Arroyo](arroyovs@oregonstate.edu), [Shelby Westerberg](westerbs@oregonstate.edu), [Bryson Goto](gotob@oregonstate.edu)

<p align="center">
 <img  src="https://i.ibb.co/vzkn4VF/20220429-170130.jpg" width="550">   
</p>

## Purpose
The goal of this project is to create an environmental sensing package that automatically took temperature, humidity, and soil moisture measurements of our stakeholder's farm in Thailand and transmitted said data to an online database where it could be automatically processed and graphed. Since these measurements are collected automatically, the farmer can then spend their efforts on more advanced tasks while the system handles the redundant, menial ones. While the system only collects 3 types of data, the future versions may include more environmental data measurements as well as 4G capabilities so that publishing the data does not require a computer with internet connection. 

## Methods
### Hardware
The system uses an Adafruit Feather M0 LoRa, [Hypnos board](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/Hypnos), a Teros 11 soil moisture sensor, a BME280 temperature and humidity sensor, as well as a custom PCB used for power management and sensor integration. These components are for the sensor package side (node). The node collects the environmental sensing data, forms a URL with the collected data, and transmits the URL string through LoRa radio communication. 

From there, the receiver hub listens for the node's transmission signal and collects the data accordingly. The hub consists of an Adafruit Feather M0 LoRa as well as an Adafruit DS3231 RTC Featherwing. The RTC Featherwing is used to check the amount of time that passes in between packets. As per our engineering requirement, the system must show an error of transmission within 2 seconds of expected collection which is what the RTC is used for. The RTC notifies the website that too much time has passed and sends an error point on the webpage. 

### Software
The software can be divided into three programs: the receiver, the node, as well as the data processing code. The receiver and node code are programmed in the Arduino IDE while the data processing code was coded in the [Processing app](https://processing.org/download) which is done through Java. The node code collects the sensor data, forms the URL with the data, transmits it through LoRa, and powers down to conserve battery life. The receiver code constantly listens for an active LoRa signal and collects the message after all of the packets were sent. Once collected, the program prints the URL into the serial monitor for the data processing code to work. The data processing code runs in the background and reads the serial monitor for the URL. Once read, the program opens the link which stores the data values and displays it on screen for users to see. 

<p align="center">
 <img  src="https://i.ibb.co/7GNMrQt/ece443-dataflow.png" width="550">   
</p>

## Results
Any collected data will be stored and charted on our [website](https://web.engr.oregonstate.edu/~eaglea/sensordata.php). The website allows the user to adjust the time to look for data in a certain time interval. With the chart, you may hover over any part of the data and see the value at that given time. In the chart below, we ran the system to test how long the battery would last. The first part of the graph is through earlier tests with no testing setup so the data is dummy values. At around May 1, 3:03PM, the system was powered on to track battery life without solar power. The system was able to last 12 hours without solar power without a fully charged battery. 

<p align="center">
 <img  src="https://i.ibb.co/3pCkp2z/data.png" width="550">   
</p>

## Future
As stated before, 4G capabilities would be added to a future iteration of the system. This would remove the necessity of having a computer with internet connection on standby. Another improvement for the system would be to add a power button on the outside of the case so users would not need to unscrew the lid to power the system. The last known improvement of the system would be to add a sensor to read water levels of the surrounding area. This could be a simple sensor that reads the water level and if it reaches a certain threshold, it triggers an interrupt. 

## Questions?
Email one of the [project members](https://github.com/BGoto808/ECE44X_Integrated_Sensor_22/edit/main/README.md#oregon-state-university-ece-senior-capstone-integrated-environmental-sensor-for-thai-agriculture-application) to learn more about the project!
