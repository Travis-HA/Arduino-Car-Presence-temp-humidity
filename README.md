# Arduino-Car-Presence-temp-humidity
MQTT pub / sub for Wemos D1mini with SHT30 shield

Very simple Arduino sketch to connect to WiFi, subscribe to MQTT Broker, setup LWT messages and send Temperature / Humidity sensor
info to Home Assistant via MQTT.

Also included 3 x subscribed topics to allow for control of GPIO pins.  This could be to control anything in the car via GPIOs
eg. status LEDs, fan etc.

I use the Wemos D1mini attached to a permanent power source in the car to monitor internal temperature while parked at home.
My idea is to have a home assistant automation turn on an internal fan to vent hot air outside the vehicle when the temperature
reaches a preset value and then turn off the fan again when the temperature drops below another preset value.

Another way to power the Wemos is to connect to the ignition and have the D1mini only come alive while the engine is running.
This would allow you to control garage doors / entrance gates via an automation sensing the LWT state + Cover state and either
opening or closing the doors.

No part of this code is unique and was gathered from different sources online.  
Main thanks and credit goes to Aderusha for his GitHub project https://github.com/aderusha/MQTTCarPresence

Appologies if you are the original author of any other code that I have not given you credit for as I don't remember where I found all
the different code.  I am not a programmer and other than copying and pasting existing code, I have no idea what I'm doing.

Normally I would prefer to use TASMOTA or any other existing arduino sketch to accomplish what I need but in this project 
I couldn't find anything that fit my needs exactly so I decided to put this little project together instead.

