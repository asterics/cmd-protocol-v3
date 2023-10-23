# This class represents the implementation of button press detections
import time
from enum import Enum

class TriggerState(Enum):
    STARTED="started"
    NEXT="next"
    CANCELLED="cancelled"
    FIRED="fired"

RISING=1
FLAT=0
FALLING=-1

class Trigger:
    state = TriggerState.STARTED
    prev_event_value=0
    edge=FLAT
    t_start=-1
    t_start_rising=-1
    t_start_falling=-1

    def __init__(self, name="", source=1, mode=None, timeout=200, next=None, action=None):
        self.name=name
        self.source=source
        self.mode=mode
        self.timeout=timeout
        self.next=next
        self.action=action

    def __str__(self):
        return f"Trigger(name={self.name},source={self.source}, self.mode{self.mode},timeout={self.timeout},next={self.next})"

    def reset(self):
        #print(f"resetting Button(name={self.name})")
        self.t_start = -1
        self.t_start_rising = -1
        self.t_start_falling = -1
        self.prev_event_value = 0
        self.state = TriggerState.STARTED
        self.edge=FLAT

    def update_timeout(self):
        if self.state==TriggerState.CANCELLED or self.state==TriggerState.FIRED or self.state==TriggerState.NEXT:
            return self.state

        if self.state == TriggerState.STARTED:
            current_time=time.time_ns()/1000000
            time_diff = current_time - self.t_start

            if time_diff > self.timeout:
                self.state=TriggerState.CANCELLED
                #print(f"Timeout for: {self.name}")

        return self.state

    def process(self, event):
        if self.state == TriggerState.FIRED or self.state == TriggerState.CANCELLED:
            self.reset()

        if self.state == TriggerState.NEXT:
            if self.next is not None:
                self.next.process(event)
        else:
            if self.t_start == -1:
                self.t_start = event.timestamp

            if self.update_timeout()==TriggerState.CANCELLED:
                return self.state

            #print(f"event: {event}, prev_event_value: {self.prev_event_value}")
            self.edge=event.value-self.prev_event_value

            if self.edge >= RISING:
                self.t_start_rising=event.timestamp
            elif self.edge <= FALLING:
                self.t_start_falling=event.timestamp

            #print(f"Before: {self} processing event {event}, delta_t_start={delta_t_start}, edge={self.edge}")

            self.process_mode(event)
            #print(f"After: {self} processing event {event}, delta_t_start={delta_t_start}, edge={self.edge}")

        self.prev_event_value=event.value
        return self.state

    def execute(self):
        #print(f"{self}")
        if self.action is not None:
            self.action.execute()

class Button(Trigger):
    count_pressed=0

    def __init__(self, name="", source=1, mode="pressed", count=1, debounce="20", timeout=200, duration=200, next=None, action=None):
        super().__init__(name=name,source=source,mode=mode,timeout=timeout,next=next,action=action)
        self.count=count
        self.debounce=debounce
        self.duration=duration

    def __str__(self):
        return f"Button(name={self.name},source={self.source},mode={self.mode},count={self.count},debounce={self.debounce},timeout={self.timeout},next={self.next}: pressed_count={self.count_pressed})"

    def process_mode(self, event):
        if event.trigger_type == "button":
            if self.mode == "pressed" and self.edge == FALLING:
                self.count_pressed+=1

                if self.count_pressed==self.count:
                    if self.action is not None:
                        self.state=TriggerState.FIRED
                    else:
                        self.state=TriggerState.NEXT
            elif self.mode == "long_pressed" and self.edge == FALLING:
                #print(f"t_start_falling {self.t_start_falling}, t_start_rising {self.t_start_rising}")
                t_pressed=self.t_start_falling-self.t_start_rising

                # long press successful
                if t_pressed >= self.duration:
                    self.t_start=-1
                    if self.action is not None:
                        self.state=TriggerState.FIRED
                    else:
                        self.state=TriggerState.NEXT
                # long press too short --> cancelled
                else:
                    self.state = TriggerState.CANCELLED

        return self.state

    def reset(self):
        super().reset()
        self.count_pressed=0

# This is the trigger implementation for a pressure trigger type
class Pressure(Trigger):
    def __init__(self, name="", source=1, mode="puff", timeout=200, threshold=560, edge="rising", debounce="20", next=None, action=None):
        super().__init__(name=name,source=source,mode=mode,timeout=timeout,next=next,action=action)
        self.threshold=threshold
        self.edge=edge
        self.debounce=debounce

    def __str__(self):
        return f"Pressure(source={self.source},mode={self.mode},threshold={self.threshold},{self.edge},debounce={self.debounce},next={self.next})"

    def process_mode(self, event):
        if self.state == TriggerState.NEXT:
            if self.next is not None:
                self.next.process(event)
        else:
            if event.trigger_type == "pressure":
                print(f"button: processing event {event}")
                if self.mode == "puff" and event.value >= self.threshold:
                    if not self.action == None:
                        self.action.execute()
                    else:
                        self.state = TriggerState.NEXT


