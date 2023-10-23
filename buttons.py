# This class represents the implementation of button press detections
import time


class Button:
    count_pressed=0
    t_start=-1
    t_start_pressed=-1
    t_start_released=-1
    prev_button_value=-1
    state = "this"

    def __init__(self, name="", source=1, mode="pressed", count=1, debounce="20", timeout=200, duration=200, next=None, action=None):
        self.name=name
        self.source=source
        self.count=count
        self.mode=mode
        self.debounce=debounce
        self.timeout=timeout
        self.duration=duration
        self.next=next
        self.action=action

    def __str__(self):
        return f"Button(name={self.name},source={self.source},mode={self.mode},count={self.count},debounce={self.debounce},timeout={self.timeout},next={self.next}(pressed_count={self.count_pressed}))"

    def process(self, event):
        if self.state == "fired" or self.state == "cancelled":
            self.reset()

        if self.state == "next":
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
                            self.state="fired"
                        else:
                            self.state="next"
                elif self.mode == "long_pressed":
                    time_duration=event.timestamp-self.t_start_pressed

                    # button press started
                    if self.t_start_pressed == 0 and event.value == 1:
                        self.t_start_pressed=event.timestamp
                    # long press successful
                    elif time_duration >= self.duration:
                        if self.action is not None:
                            self.state="fired"
                        else:
                            self.state="next"
                    # button 0 event before duration reached --> long press cancelled
                    elif event.value == 0 and time_duration < self.duration:
                        self.state = "cancelled"
                elif time_diff > self.timeout:
                    self.state = "cancelled"
                    return self.state

                return self.state

    def reset(self):
        self.count_pressed = 0
        self.t_start = -1
        self.t_start_pressed = -1
        self.t_start_released = -1
        self.prev_button_value = -1
        self.state = "this"

    def update_timeout(self):
        if self.state=="cancelled" or self.state=="fired":
            return self.state

        if self.state != "fired" and self.state != "next":
            current_time=time.time_ns()/1000000
            time_diff = current_time - self.t_start

            if time_diff > self.timeout:
                self.state="cancelled"
                return self.state

        return self.state

    def execute(self):
        if self.action is not None:
            self.action.execute()



