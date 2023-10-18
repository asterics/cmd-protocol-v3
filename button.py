# This class represents the implementation of button press detections
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
        return f"Button(name={self.name},source={self.source},mode={self.mode},count={self.count},debounce={self.debounce},timeout={self.timeout},next={self.next})"

    def process(self, event):
        if(self.state=="fired"):
            self.reset()
            #self.state="ended"
            return self.state

        if self.state == "next":
            if self.next is not None:
                self.next.process(event)
        else:
            if self.prev_timestamp == -1:
                self.prev_timestamp=event.timestamp

            if event.trigger_type == "button":
                print(f"button(name={self.name}: processing event {event}")
                time_diff=event.timestamp-self.prev_timestamp

                if self.mode == "pressed" and event.value == 1 and time_diff <= self.timeout:
                    self.pressed_count+=1

                    if self.pressed_count==self.count:
                        if not self.action == None:
                            self.action.execute()
                            self.state="fired"
                            return self.state
                        else:
                            self.state="next"

    def reset(self):
        self.pressed_count=0
        self.prev_timestamp=-1
        self.state="this"
