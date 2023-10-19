# This class represents the implementation of button press detections
import time


class Button:
    pressed_count=0
    prev_timestamp=-1

    def __init__(self, name="", source=1, mode="pressed", count=1, debounce="20", timeout=200, next=None, action=None):
        self.name=name
        self.source=source
        self.count=count
        self.mode=mode
        self.debounce=debounce
        self.timeout=timeout
        self.next=next
        self.action=action
        self.state="this"

    def __str__(self):
        return f"Button(name={self.name},source={self.source},mode={self.mode},count={self.count},debounce={self.debounce},timeout={self.timeout},next={self.next}(pressed_count={self.pressed_count}))"

    def process(self, event):
        if self.state == "fired" or self.state == "cancelled":
            self.reset()

        if self.state == "next":
            if self.next is not None:
                self.next.process(event)
        else:
            if self.prev_timestamp == -1:
                self.prev_timestamp=event.timestamp

            if event.trigger_type == "button":
                time_diff = event.timestamp - self.prev_timestamp
                print(f"{self} processing event {event}, time_diff={time_diff}")

                if self.mode == "pressed" and event.value == 1 and time_diff <= self.timeout:
                    self.pressed_count+=1

                    if self.pressed_count==self.count:
                        if self.action is not None:
                            self.state="fired"
                        else:
                            self.state="next"

                elif time_diff > self.timeout:
                    self.state = "cancelled"
                    return self.state

                return self.state

    def reset(self):
        self.pressed_count=0
        self.prev_timestamp=-1
        self.state="this"

    def update_timeout(self):
        if self.state=="cancelled" or self.state=="fired":
            return self.state

        if self.state != "fired":
            current_time=time.time_ns()/1000000
            time_diff = current_time - self.prev_timestamp

            if time_diff > self.timeout:
                self.state="cancelled"
                return self.state

        return self.state

    def execute(self):
        if self.action is not None:
            self.action.execute()



