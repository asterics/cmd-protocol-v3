# This class represents the implementation of button press detections
import time
from enum import Enum

class TriggerState(Enum):
    STARTED="started"
    NEXT="next"
    CANCELLED="cancelled"
    FIRED="fired"

class Trigger:
    state = TriggerState.STARTED
    t_start=-1
    prev_button_value=-1

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
        self.count_pressed = 0
        self.t_start = -1
        self.t_start_pressed = -1
        self.t_start_released = -1
        self.prev_button_value = -1
        self.state = TriggerState.STARTED

    def update_timeout(self):
        if self.state==TriggerState.CANCELLED or self.state==TriggerState.FIRED:
            return self.state

        if self.state != TriggerState.FIRED and self.state != TriggerState.NEXT:
            current_time=time.time_ns()/1000000
            time_diff = current_time - self.t_start

            if time_diff > self.timeout:
                self.state=TriggerState.CANCELLED
                return self.state

        return self.state

    def execute(self):
        if self.action is not None:
            self.action.execute()

class Button(Trigger):
    count_pressed=0
    t_start_pressed=-1
    t_start_released=-1

    def __init__(self, name="", source=1, mode="pressed", count=1, debounce="20", timeout=200, duration=200, next=None, action=None):
        super().__init__(name=name,source=source,mode=mode,timeout=timeout,next=next,action=action)
        self.count=count
        self.debounce=debounce
        self.duration=duration

    def __str__(self):
        return f"Button(name={self.name},source={self.source},mode={self.mode},count={self.count},debounce={self.debounce},timeout={self.timeout},next={self.next}(pressed_count={self.count_pressed}))"

    def process(self, event):
        if self.state == TriggerState.FIRED or self.state == TriggerState.CANCELLED:
            self.reset()

        if self.state == TriggerState.NEXT:
            if self.next is not None:
                self.next.process(event)
        else:
            if self.t_start == -1:
                self.t_start=event.timestamp

            if event.trigger_type == "button":
                time_diff = event.timestamp - self.t_start

                print(f"{self} processing event {event}, time_diff={time_diff}")

                if self.mode == "pressed" and event.value == 1 and time_diff <= self.timeout:
                    self.count_pressed+=1

                    if self.count_pressed==self.count:
                        if self.action is not None:
                            self.state=TriggerState.FIRED
                        else:
                            self.state=TriggerState.NEXT
                elif self.mode == "long_pressed":
                    time_duration=event.timestamp-self.t_start_pressed

                    # button press started
                    if self.t_start_pressed == 0 and event.value == 1:
                        self.t_start_pressed=event.timestamp
                    # long press successful
                    elif time_duration >= self.duration:
                        if self.action is not None:
                            self.state=TriggerState.FIRED
                        else:
                            self.state=TriggerState.NEXT
                    # button 0 event before duration reached --> long press cancelled
                    elif event.value == 0 and time_duration < self.duration:
                        self.state = TriggerState.CANCELLED
                elif time_diff > self.timeout:
                    self.state = TriggerState.CANCELLED
                    return self.state

                return self.state




