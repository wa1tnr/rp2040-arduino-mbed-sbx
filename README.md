Mon Jul  5 13:03:27 UTC 2021
Mon Jul  5 05:22:36 UTC 2021

Initial program development complete.

 Blinks an LED with some adjustments in timing.
 Operates a 'thread' whatever that is.
 Writes to Serial (USB port CDC/ACM).
 Waits for serial connection with a short duration blink cycle.
 Resets MCU back to acquire serial port state rather than reflash() (also possible).

 Just enable the end of the story you wanted - reflash() or system_reset() or something else, to suit.

Other:

   https://github.com/arduino/mbed-os

