# ESP32 ChickenCoop - work in progress.  Feedback welcome.

ESP32 code on Arduino IDE to manage a chicken coop door.

This script is designed for the ESP32 and channel A of an L298N motor controller.
Its purpose is to control a chicken coop door.
It uses a photoresistor to measure the light level and two reed swithces on the door.
If light detected = LIGHT and door is closed, it will open the door.
If dark detected = DARK and door is open, it will close the door.
If door is not detected as open or close, it will flash an LED in error.
