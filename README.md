# Dingus

Hardware example to illustrate an Internet of Things ecosystem. A dingus is a kind of a thing.

## Features

### Easy Wifi connection
Automatically connect to known wifi if available. If no known wifi is available, the device will fire up an Access Point of it's own that you can connect to in order to configure a new ssid and password.

### Built in server
Find the device's IP address via the serial monitor or your preferred network sniffer and control the led via a browser on a local network.

### MQTT
State change in led is published to a channel based on the ESP8266 Chip ID like so `/feeds/dingus-[CHIP_ID]`

### Input toggles output
Pressing a button connected to pin 12 will toggle the state of an led (or other simple acutator) connected to pin 15.

## Hardware assumptions

I'm building this for a HuzzahFeather. Any wifi enabled Arduino compatable board should work. I have an led hooked up to pin 15 and a button wired up to pin 12.

## Installation notes

You'll most likely need to install some libraries at teh top of dingus.ino before this will compile. This can be done in the Arduino IDE Library manager by searching for any library that the compiler complains about. I don't know a better way to handle package management at this time.
