# cmd-protocol-v3

Documentation and reference implementation of command protocol v3, which adds mapping between trigger events and actions.

## Installation

```bash
pip install -r requirements.txt
```

## Run Program

```bash
python main.py
```
Use the keys <kbd>1</kbd> and <kbd>2</kbd> to simulate button presses.

## Documentation

### Class diagram

```mermaid
---
title: cmd protocol class diagram
---
classDiagram
    TriggerType <|-- Button
    TriggerType <|-- Pressure
    TriggerType : +TriggerState state
    TriggerType : +Action action
    TriggerType : +TriggerType next
    TriggerType: +process()

    TriggerType "1" --> "0..1" TriggerType

    class Button{
    }
    class Pressure{
    }

    Action <|-- MouseAction
    Action <|-- IRAction
    Action "1" --> "0..1" Action

    class Action{
        +execute()
    }
    class MouseAction{
    }
    class IRAction{
    }
```

### TriggerType Button

A button trigger type can be triggered by several modalities:

1. single-click
2. double-click
3. triple-click
4. long-press
5. (combination of triggers 1-4 of the same button or another one.)

All triggers can have a timeout value, which resets the current state and starts evaluating a trigger type from the beginning.

### Trigger States

A Trigger type is implemented as a class and maintains it's own state. 
The states can be one of the following:

* STARTED: Trigger object waits for key input to be evaluated
* CANCELLED: Input evaluation went into timeout
* FIRED: The Trigger object has had a valid combination of the required key inputs within the defined timeout time span.
* NEXT: The Trigger object has had a valid combination of the required key inputs and is cascaded with a subsequent Trigger object which does further processing of key inputs now.

### Example Configuration

```python
    a1 = Action(name="single-click action")
    a2 = Action(name="double-click action")
    a3 = Action(name="triple-click action")
    a4 = Action(name="long press action")
    a5 = Action(name="combined action")

    # Button 1, single-click
    b1=Button(name="b1",source=1,mode="pressed",count=1, timeout=400,action=a1)
    # Button 1, double-click
    b2 = Button(name="b2",source=1, mode="pressed", count=2, timeout=400, action=a2)
    # Button 1, triple-click
    b3 = Button(name="b3",source=1, mode="pressed", count=3, timeout=400, action=a3)
    # Button 1, long press
    b4 = Button(name="b4", source=1, mode="long_pressed", count=1, timeout=600, duration=100, action=a4)
    # Button 1 single-click + Button 2 single-click
    b5 = Button(name="b5", source=1, mode="pressed", count=1, timeout=200,
                next_trigger=Button(name="b5a", source=2, mode="pressed", count=1, timeout=200, action=a5)
```
### Algorithm

The trigger mappings must be grouped by trigger_type (e.g. button and source number) and then sorted by priority within that group.

#### Example

1. Button 1, long press
2. Button 1, triple-click
3. Button 1, single-click + Button 2, single-click
4. Button 1, double-click
5. Button 1, single-click

```python
   trigger_defs=[b4,b3,b5,b2,b1]
```

#### Evaluation

A button press is then propagated to all trigger mappings and the action is executed according to the following flowchart:

```mermaid
  graph TD;
      A[Start] -->B4{B4 Fired?};
      B4 -- Yes --> A4[Execute Action A4]
      B4 -- No --> B3{B3 Fired?};
      B3 -- Yes --> A3{Execute Action A3}
      B3 -- No --> B5{B5 Fired?}
      B5 -- Yes --> A5{Execute Action A5}
      B5 --> No --> B2{B2 Fired?}
      B2 -- Yes --> A2[Execute Action A2]
      B2 -- No --> A1[Execute Action A1]
```
