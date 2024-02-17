# cmd-protocol-v3 - Alternative Arduino implementation
Alternative version of a new command protocol, can be used with FABIv3 / FlipMouse via AT-commands.
The code is based upon the SmartButton Arduino Library by Marcin Borowicz: https://github.com/marcinbor85/SmartButton


## Prerequisites
  * should work with FABI v3 (RP Pico W) or FlipMouse v3 (Arduino Nano 2040 Connect)
  * for compile/build: Arduino-IDE with RP2040 board support (e.g. Arduino Pico Core https://github.com/earlephilhower/arduino-pico)
  * for testing: 3 Buttons connected to GPIO 17, 20, 28 (active low)


## Documentation
Build and run the sketch "SmartButtonTriggers.ino"
Connect Button 1, 2 and 3, and try the AT commands as explained in the Serial Window.
The trigger id string (e.g. "Button1 pressed") needs to be correctly written. 
Conditions can be created by placing id strings one after the other (separated by "->").
The action string is just a placeholder which is printed when the action is triggered.

Example AT commands:

<pre>
  at ti id=Button1 pressed,action=B1 was pressed
  at ti id=Button2 pressed,action=B2 was pressed
  at ti id=Button3 pressed,action=B3 was pressed
  at ti id=Button2 pressed->Button1 pressed,action=B1 after B2!
  at ti id=Button3 released,action=B3 was released
  at ti id=Button3 click1,action=B3 was clicked
  at ti id=Button3 click2,action=B3 was doublelicked
</pre>


