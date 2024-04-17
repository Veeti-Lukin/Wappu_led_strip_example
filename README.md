# Wappu_led_strip_example
Example codes for studet event where students get to solder and program their own led strips accessories.

There are two versions of the example code:
The first version varies between effects selected in programming. The effects are selected by storing them as function pointers in an array, from which they are then executed. The code provides an easy way to adjust the duration of the effect and choose which effects to include.

In the second version, there are the same effects as in the first version, but the effect can be selected with a button (which unfortunately does not work with the case you'll receive at the event), as well as over the serial port, which works directly from, for example, the Arduino IDE or Putty. This version also saves the selected effect to EEPROM, allowing the user to choose their desired effect over the serial port, for example, with a computer at home, and then reconnect the Arduino to a power bank, meaning the effect can be chosen afterwards without reprogramming.

## ArduinoIDE programming instructions

First follow these instructions to setup your programming environment: [Environment setup instructions pdf](Programming_instructions_wappu_led.pdf)
Then follow these instructions to download and import the example codes: [Importing instrcutions pdf](Example_code_importing_instructions.pdf)
