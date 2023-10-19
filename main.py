# This is a sample Python script.
import time
import keyboard

from button import Button
from pressure import Pressure
from event import Event
from action import Action

def test_events(event_list):
    e1=Event(source=1,trigger_type="button",value=0,timestamp=1)
    e2 = Event(source=1, trigger_type="button", value=1, timestamp=2)
    e3 = Event(source=1, trigger_type="pressure", value=500, timestamp=1)
    e4 = Event(source=1, trigger_type="pressure", value=530, timestamp=1)
    e5 = Event(source=1, trigger_type="pressure", value=560, timestamp=1)

    #event_list=[e1,e3,e4,e5,e2,e1,e2,e3,e4]
    event_list=[e1,e2,e2]


def process_event(event_list):
    to_be_fired=None
    prev_trigger_state=None

    for event in event_list:
        for trigger_def in trigger_defs:
            trigger_state=trigger_def.process(event)

            print(f"trigger_state={trigger_state}, prev_trigger_state={prev_trigger_state}")
            if prev_trigger_state is None or prev_trigger_state == "cancelled":
                if trigger_state == "fired":
                    print(f"setting to_be_fired: {to_be_fired}")
                    to_be_fired=trigger_def

            prev_trigger_state=trigger_state

    print(f"to_be_fired: {to_be_fired}")
    if to_be_fired is not None :
        to_be_fired.action.execute()

def event_loop():
    keyboard.wait("esc")

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    a1=Action(name="left click")
    a2 = Action(name="right click")
    a3 = Action(name="middle click")

    b1=Button(name="b1",source=1,mode="pressed",count=1, timeout=500,action=a1)
    b2 = Button(name="b2",source=1, mode="pressed", count=2, timeout=500, action=a2)

    p1=Pressure(source=1,mode="puff",threshold=560,edge="rising",action=a2)
    p2=Pressure(source=1,mode="puff",threshold=530,edge="rising",action=a3)
    b_p2=Button(source=1,mode="pressed",count=2, debounce=30,next=p2)

    #trigger_defs=[b1,p1,b_p2]
    trigger_defs=[b2,b1]

    print(b1)
    print(p1)
    print(b_p2)

    keyboard.add_hotkey("1", lambda: process_event([Event(source=1,trigger_type="button",value=1,timestamp=time.time_ns()/1000000)]))
    keyboard.add_hotkey("2", lambda: process_event([Event(source=2, trigger_type="button", value=1, timestamp=time.time_ns()/1000000)]))
    keyboard.add_hotkey("3", lambda: process_event([Event(source=3, trigger_type="button", value=1, timestamp=time.time_ns()/1000000)]))

    keyboard.wait("esc")
# See PyCharm help at https://www.jetbrains.com/help/pycharm/