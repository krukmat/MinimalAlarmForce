# MinimalAlarmForce
A new fork for AlarmForce. The version that simplifies AlarmForce. It uses esp32 deep wake up mechanism through a pin instead of wifi communication.
The stack selected:
* Features: 
    * Online monitoring.
    * Email alerts
    * Remote arm/disarm
    * Complete tracking of activitites through Salesforce
* IOT Proxy:
    * Thingspeak(MQTT)
*  Hardware:
    * ESP32
         * PIR Sensor
    * ESP32-Cam
    * Webserver to store the screenshots
    * MQTT Client to map the events to Salesforce
* Software:         
    * Arduino IDE
    * Salesforce:
         * Custom Object: Sensor

##  Hardware
![Schematics](https://github.com/krukmat/CentralAlarmForce/blob/4649af451f55758c009d65564f2bcd37ad95dbef/Arduino/images/hardware2.png)
![Schematics](https://github.com/krukmat/CentralAlarmForce/blob/390de5740e9e64eadbeb8ba5ed9c90c8c3ecb865/Arduino/images/hardware1.png)

## General Schema
![Schematics](https://github.com/krukmat/CentralAlarmForce/blob/390de5740e9e64eadbeb8ba5ed9c90c8c3ecb865/Arduino/images/Schematics.png)

## Salesforce: 
![Schematics](https://github.com/krukmat/MinimalAlarmForce/blob/3ba9fd4c1d5bf79fed0edfea5b9bedb2016994d6/images/armado.png)
![Schematics](https://raw.githubusercontent.com/krukmat/AlarmForce-Demo/mqtt/images/email%20alert.png)
![Schematics](https://github.com/krukmat/CentralAlarmForce/blob/735e282a2d621d94789de106fbdaf14bafff708b/Arduino/images/email.png)