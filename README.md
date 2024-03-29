## ESP-8266 Jaz's Light Panel

Simple toggleable board with web interface to turn on or off a light panel.

This works well on the LoL1n board, with ESP8266 chip.

### Arduino setup:
Be aware that the board comes as 9600 baud, but this board seems to work fine
with 921600 baud.
default for LoL1N(WEMOS) D1 mini Pro, 80MHz QIO, per settings in Arduino IDE.

In other words, the initial upload will take a while, but subsequent uploads are quick.

### Hardware setup

Until the chip boots, output pins can be floating.
Avoiding GPIO-0 (D3), since it is used to indicate program flashing.
The switch output is on pin D4. This should be pulled down with a 10K resistor.
The power button input is on D5. This should be pulled down with a 10K resistor.
![Pinout](./ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png)

### Development

Serial Port is at 115200

Developed using Arduino IDE plus ESP8266 packages.

