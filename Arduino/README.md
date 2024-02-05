# cmd-protocol-v3 - Alternative Arduino implementation
Alternative version of a new command protocol, can be used with FABI / FlipMouse via AT-commands.

## Prerequisites
Install / clone the SmartButton Library from https://github.com/ChrisVeigl/SmartButton
(a small patch was applied in this fork which was needed for compatibility with the Arduino Nano 2040 Connect.)
You can also install the original SmartButton library via the Arduino Library Manager and then apply the patch manually.


## Documentation
Should work with current FABI (Arduino Micro) or FlipMouse v3 (Arduino Nano 2040 Connect).
Build and run the sketch "SmartButtonTriggers.ino"
Connect up to 3 switches, and try the AT commands via Serial Window:

<pre>
 at ti source=Button1 hold,action=B1held!
 at ti source=Button2 click1,action=B2clicked!
 at ti source=Button3 pressed,action=B3pressed!
 at ti source=Button3 released,action=B3released!
 at ti source=Button2 click2,action=B2doubleClicked!
 at ti source=Button1 click1,action=B2dbl-B1clicked!,condition=Button2 click2
 at free
 at clear\n
</pre>


