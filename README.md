# Telerobi
Firmware and server application for DIY telepresence robot

## Links
[Documentation](https://docs.google.com/document/d/1HfuViC98gQP4LPHOGDCBuGeYhDl_x02C) 
created during the Erasmus+ project named 
[TRinE](https://www.trine-platform.com/)
.

## Quick Guide
* Install the arduino IDE
* Load each project in the firmware folder
  * Install the arduino code to the arduino UNO
  * Install the esp32 code to the ESP32
    * you will need to install the esp boards in your arduino IDE, then select the board: AI Thinker ESP32-CAM
* Host this repository on a LAMP server (needs PHP and python 2)
* Start the esp32, it will open an SSID named 'trineBot'
  * connect to it, password is: bot12345
    * if connecting from android,you may have to explicitly demand to stay connected because the network does not provide internet acccess..
  * open 192.168.4.1/settings, this will open a config site
    * configure..
      * a robot name
      * the local SSID and password the robot should connect to
      * the server IP address
      
## Trouble shooting
* if the esp board does not appear in the Arcuino IDE:
  * check if /dev/ttyUSB0 is there
    * if not check USB messages with 'sudo dmesg -w'

