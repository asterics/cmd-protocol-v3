# This is a sample Python script.
import time
import keyboard
import queue
import threading

from triggers import Button, TriggerState, Pressure

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
        #print(f"qsize: {event_queue.qsize()}")
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

        if trigger_state==TriggerState.FIRED and (prev_trigger_state is None or prev_trigger_state == TriggerState.CANCELLED):
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

def keyboard_hook(key_event):
    #print(key_event)
    #print(f"{key_event.event_type},{key_event.name},{key_event.time}")

    if key_event.name in ["1","2","3"]:
        if key_event.event_type==keyboard.KEY_DOWN:
            add_to_event_queue(Event(source=int(key_event.name), trigger_type="button", value=1, timestamp=time.time_ns() / 1000000))
        elif key_event.event_type==keyboard.KEY_UP:
            add_to_event_queue(Event(source=int(key_event.name), trigger_type="button", value=0, timestamp=time.time_ns() / 1000000))

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    a1 = Action(name="single-click action")
    a2 = Action(name="double-click action")
    a3 = Action(name="triple-click action")
    a4 = Action(name="long press action")
    a5 = Action(name="combined action")

    b1=Button(name="b1",source=1,mode="pressed",count=1, timeout=400,action=a1)
    b2 = Button(name="b2",source=1, mode="pressed", count=2, timeout=400, action=a2)
    b3 = Button(name="b3",source=1, mode="pressed", count=3, timeout=400, action=a3)
    b4 = Button(name="b4", source=1, mode="long_pressed", count=1, timeout=600, duration=100, action=a4)
    b5 = Button(name="b5", source=1, mode="pressed", count=1, timeout=200,
                next_trigger=Button(name="b5a", source=2, mode="pressed", count=1, timeout=200, action=a5)
)

    #p1=Pressure(source=1,mode="puff",threshold=560,edge="rising",action=a2)
    #p2=Pressure(source=1,mode="puff",threshold=530,edge="rising",action=a3)
    #b_p2=Button(source=1, mode="pressed", count=2, debounce=30, next_trigger=p2)

    #trigger_defs=[b1,p1,b_p2]
    trigger_defs=[b4,b3,b5,b2,b1]
    #trigger_defs=[b5]

    #keyboard.add_hotkey("1", lambda: add_to_event_queue(Event(source=1,trigger_type="button",value=1,timestamp=time.time_ns()/1000000)))
    #keyboard.on_release_key("1", add_to_event_queue,args=[Event(source=1, trigger_type="button", value=0, timestamp=time.time_ns() / 1000000)],trigger_on_release=True)

    #keyboard.add_hotkey("2", lambda: add_to_event_queue(Event(source=2, trigger_type="button", value=1, timestamp=time.time_ns()/1000000)))
    #keyboard.add_hotkey("2", lambda: add_to_event_queue(Event(source=2, trigger_type="button", value=0, timestamp=time.time_ns() / 1000000)),trigger_on_release=True)

    #keyboard.add_hotkey("3", lambda: add_to_event_queue(Event(source=3, trigger_type="button", value=1, timestamp=time.time_ns()/1000000)))
    #keyboard.add_hotkey("3", lambda: add_to_event_queue(Event(source=3, trigger_type="button", value=0, timestamp=time.time_ns() / 1000000)),trigger_on_release=True)

    keyboard.hook(keyboard_hook)
    threading.Thread(target=keyboard.wait,daemon=True).start()
    main_loop()