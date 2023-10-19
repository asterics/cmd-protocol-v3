# This is a sample Python script.
import time
import keyboard
import queue
import threading

from button import Button
from pressure import Pressure
from event import Event
from action import Action

#global queue to collect input events
event_queue=queue.Queue()

def test_events(event_list):
    e1=Event(source=1,trigger_type="button",value=0,timestamp=1)
    e2 = Event(source=1, trigger_type="button", value=1, timestamp=2)
    e3 = Event(source=1, trigger_type="pressure", value=500, timestamp=1)
    e4 = Event(source=1, trigger_type="pressure", value=530, timestamp=1)
    e5 = Event(source=1, trigger_type="pressure", value=560, timestamp=1)

    #event_list=[e1,e3,e4,e5,e2,e1,e2,e3,e4]
    event_list=[e1,e2,e2]

def main_loop():
    while True:
        process_events()
        time.sleep(0.1)

# processes the events and updates timeout values
# if a trigger_type fired it's corresponding action is executed.
def process_events():
    global event_queue

    prev_trigger_state=None

    # if there are new events, process them by the trigger_defs to update the states
    try:
        while event_queue.qsize() > 0:
            event=event_queue.get()
            for trigger_def in trigger_defs:
                trigger_state=trigger_def.process(event)
    except queue.Empty:
        pass

    # update timeouts each loop iteration and execute action if fired or previous trigger_def changed state to cancelled.
    fired=False
    for trigger_def in trigger_defs:
        trigger_state=trigger_def.update_timeout()
        #print(f"trigger_def: {trigger_def}, trigger_state={trigger_state}, prev_trigger_state={prev_trigger_state}")

        if trigger_state=="fired" and (prev_trigger_state is None or prev_trigger_state == "cancelled"):
            trigger_def.execute()
            fired=True
            break

        prev_trigger_state = trigger_state

    # reset states in case action was executed
    if fired:
        for trigger_def in trigger_defs:
            trigger_def.reset()

# adds keyboard events to the event queue
def add_to_event_queue(event):
    global event_queue

    event_queue.put(event)

# waits for keyboard events in another thread
def keyboard_wait():
    threading.Thread(target=keyboard.wait,daemon=True).start()


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    a1=Action(name="left click")
    a2 = Action(name="middle click")
    a3 = Action(name="right click")

    b1=Button(name="b1",source=1,mode="pressed",count=1, timeout=500,action=a1)
    b2 = Button(name="b2",source=1, mode="pressed", count=2, timeout=300, action=a2)
    b3 = Button(name="b3",source=1, mode="pressed", count=3, timeout=300, action=a3)

    p1=Pressure(source=1,mode="puff",threshold=560,edge="rising",action=a2)
    p2=Pressure(source=1,mode="puff",threshold=530,edge="rising",action=a3)
    b_p2=Button(source=1,mode="pressed",count=2, debounce=30,next=p2)

    #trigger_defs=[b1,p1,b_p2]
    trigger_defs=[b3,b2,b1]

    print(b1)
    print(p1)
    print(b_p2)

    keyboard.add_hotkey("1", lambda: add_to_event_queue(Event(source=1,trigger_type="button",value=1,timestamp=time.time_ns()/1000000)))
    keyboard.add_hotkey("2", lambda: add_to_event_queue(Event(source=2, trigger_type="button", value=1, timestamp=time.time_ns()/1000000)))
    keyboard.add_hotkey("3", lambda: add_to_event_queue(Event(source=3, trigger_type="button", value=1, timestamp=time.time_ns()/1000000)))

    keyboard_wait()
    main_loop()