# this class represents a trigger event

class Event:
    def __init__(self, source=1, trigger_type="button", value=0, timestamp=0):
        self.source=source
        self.trigger_type=trigger_type
        self.value=value
        self.timestamp=timestamp

    def __str__(self):
        return f"Event(source={self.source},trigger_type={self.trigger_type},value={self.value},timestamp={self.timestamp})"
