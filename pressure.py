# This class represents the implementation of a pressure sensor detection

class Pressure:
    def __init__(self, source=1, mode="puff", threshold=560, edge="rising", debounce="20", next=None, action=None):
        self.source=source
        self.mode=mode
        self.threshold=threshold
        self.edge=edge
        self.debounce=debounce
        self.next=next
        self.action=action
        self.state="this"

    def __str__(self):
        return f"Pressure(source={self.source},mode={self.mode},threshold={self.threshold},{self.edge},debounce={self.debounce},next={self.next})"

    def process(self, event):
        if self.state == "next":
            if self.next is not None:
                self.next.process(event)
        else:
            if event.trigger_type == "pressure":
                print(f"button: processing event {event}")
                if self.mode == "puff" and event.value >= self.threshold:
                    if not self.action == None:
                        self.action.execute()
                    else:
                        self.state = "next"
