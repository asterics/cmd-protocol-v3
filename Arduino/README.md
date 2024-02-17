# cmd-protocol-v3 - Alternative Arduino implementation
Alternative version of a new command protocol, can be used with FABIv3 / FlipMouse via AT-commands.
The code is based upon the SmartButton Arduino Library by Marcin Borowicz: https://github.com/marcinbor85/SmartButton


## Prerequisites
  * for Compile/Build:Arduino with RP2040 board support (e.g. Arduino Pico Core https://github.com/earlephilhower/arduino-pico),
  * for testing: 3 Buttons connected to GPIO 17, 20, 28 (active low).


## Documentation
Should work with FABI v3 (RP Pico W) or FlipMouse v3 (Arduino Nano 2040 Connect).
Build and run the sketch "SmartButtonTriggers.ino"
Use Button 1, 2 and 3, and try the AT commands as explained in the Serial Window

<pre>
  at ti id=Button1 pressed,action=B1pressed
  at ti id=Button2 pressed,action=B2pressed
  at ti id=Button3 pressed,action=B3pressed
  at ti id=Button2 pressed->Button1 pressed,action=B2p->B1p
  at ti id=Button3 released,action=B3released
  at ti id=Button3 click1,action=B3click
  at ti id=Button3 click2,action=B3doubleClick
</pre>


