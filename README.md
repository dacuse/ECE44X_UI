# ECE44X_Integrated_Sensor_22
## ECE44X Senior Capstone Project 21-22': Integrated Environmental Sensor
Project Members: [Aspen Eagle](eaglea@oregonstate.edu), [Samantha Arroyo](arroyovs@oregonstate.edu), [Shelby Westerberg](westerbs@oregonstate.edu), [Bryson Goto](gotob@oregonstate.edu)

<a href="https://ibb.co/ysqjFd9"><img src="https://i.ibb.co/vzkn4VF/20220429-170130.jpg" alt="20220429-170130" border="0"></a>

## Purpose
The goal of this project is to create an environmental sensing package that automatically took temperature, humidity, and soil moisture measurements of our stakeholder's farm in Thailand and transmitted said data to an online database where it could be automatically processed and graphed. Since these measurements are collected automatically, the farmer can then spend their efforts on more advanced tasks while the system handles the redundant, menial ones. While the system only collects 3 types of data, the future versions may include more environmental data measurements as well as 4G capabilities so that publishing the data does not require a computer with internet connection. 

## Methods
### Hardware
The system uses an Adafruit Feather M0 LoRa, [Hypnos board](https://github.com/OPEnSLab-OSU/OPEnS-Lab-Home/wiki/Hypnos), a Teros 11 soil moisture sensor, a BME280 temperature and humidity sensor, as well as a custom PCB used for power management and sensor integration. These components are for the sensor package side (node). The node collects the environmental sensing data, forms a URL with the collected data, and transmits the URL string through LoRa radio communication. 

From there, the receiver hub listens for the node's transmission signal and collects the data accordingly. The hub consists of an Adafruit Feather M0 LoRa as well as an Adafruit DS3231 RTC Featherwing. The RTC Featherwing is used to time the amount of time that passes in between packets. As per an engineering requirement, the system must show an error of transmission within 2 seconds of expected collection which is what the RTC is used for. The RTC notifies the website that too much time has passed and sends an error point on the webpage. 

### Software
The software can be divided into three programs: the receiver, the node, as well as the data processing code. The receiver and node code are programmed in the Arduino IDE while the data processing code was coded in the [Processing app](https://processing.org/download) which is done through Java. The node code collects the sensor data, forms the URL with the data, transmits it through LoRa, and powers down to conserve battery life. The receiver code constantly listens for an active LoRa signal and collects the message after all of the packets were sent. Once collected, the program prints the URL into the serial monitor for the data processing code to work. The data processing code runs in the background and reads the serial monitor for the URL. Once read, the program opens the link which stores the data values and displays it on screen for users to see. 
