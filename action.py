# represents an action

class Action:
    def __init__(self, name="left click"):
        self.name=name

    def execute(self):
        print(f"action: {self.name}")

    def __str__(self):
        return f"{self.name}"

