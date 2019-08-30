# PowerMeasure
Measure current power consuption by reading the IR pulse from your distribution board.
Then upload the result to a specified MQTT server

## Hardware needed
* NodeMCU V3 with ESP-12E 
* 10k resistor
* IR receiver diode


## Wiring
Connect:
* VU or VV to IR Diode receiver + (the long leg)
* IR Diode negative (short leg) to A0 
* 10k Resistor from IR Diode negative to GND

![Schematics](docs/schematic.png?raw=true "Title")

## Code setup

Change mqtt_server, mqtt_port, mqtt_user & mqtt_pass to match your MQTT server setup.
power_topic is the topic where the measurements will be posted.

## Code desciption

The unit will do 5 reads and the report the average consumption by calculating the time taken between theese.
Every read has a 20 ms cooldown so same pulse wont be read twice.

## Usage

Place the diode receiver head on the ir diode on the distribution board.
Make sure that the receiver isnt interferred with any other light source.

Power up the unit by connecting a micro-usb cable.
The unit will first boot up in "AP-mode"

Connect onto the device and configure it to connect to your local wifi network.
The device will restart, and if everything is setup correct you should begin receiving measurement messages on your MQTT server.

