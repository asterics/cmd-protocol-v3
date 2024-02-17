#ifndef SMART_BUTTON_H
#define SMART_BUTTON_H

#include "SmartButtonDefs.h"

namespace smartbutton {

class SmartButton {
public:
    enum class State {
        RELEASED,
        PRESSED,
        HOLD,
        LONG_HOLD
    };

    enum class Event {
        RELEASED,
        PRESSED,
        CLICK,
        HOLD,
        HOLD_REPEAT,
        LONG_HOLD,
        LONG_HOLD_REPEAT
    };

    using IsPressedHandler = bool (*)(SmartButton *button);
    using EventCallback = void (*)(SmartButton *button, SmartButton::Event event, int clickCounter);

    explicit SmartButton(SmartButton::IsPressedHandler isPressedHandler);

    SmartButton::State getState(void);
    bool isPressedDebounced(void);

    void begin(SmartButton::EventCallback eventCallback, void *context = NULL);
    void begin(void *context = NULL);
    void end();

    void* getContext();

    void process();
    static void service(SmartButton * button);
    static void blockChilds(SmartButton * button);
    static void removeBlock(uint32_t triggerID);
    static void addToRemoveList(uint32_t triggerID);
    static void clearRemovedBlocks();
    static void freeAll(SmartButton *button);
    static SmartButton * findTrigger(SmartButton * root, uint32_t triggerID);

    // TBD: use private members
    uint32_t triggerID; // identifies the trigger source (e.g. Button1, xAxisUp, PressureStrongSip etc.)  and event
                        // event (e.g. pressed, released, hold, click) sotred in bit 16-23, click count is stored in bit 24-31
    uint32_t blockedBy;
    uint32_t blockToRemove;    
    uint32_t waitForChildsTimeout;
    char * action;  // the action to be triggered, just a placeholder string
    SmartButton *child;


    SmartButton(
        SmartButton::IsPressedHandler isPressedHandler,
        bool *isPressedFlag,
        unsigned long debounceTimeout = DEFAULT_DEBOUNCE_TIMEOUT,
        unsigned long clickTimeout = DEFAULT_CLICK_TIMEOUT,
        unsigned long holdTimeout = DEFAULT_HOLD_TIMEOUT,
        unsigned long longHoldTimeout = DEFAULT_LONG_HOLD_TIMEOUT,
        unsigned long holdRepeatPeriod = DEFAULT_HOLD_REPEAT_PERIOD,
        unsigned long longHoldRepeatPeriod = DEFAULT_LONG_HOLD_REPEAT_PERIOD
    );

private:
    SmartButton() = delete;
    SmartButton(const SmartButton&) = delete;
    SmartButton& operator=(const SmartButton&) = delete;

    void debounce();
    void callEvent(SmartButton::Event event);

    bool getInputState();

    bool *isPressedFlag;
    SmartButton::IsPressedHandler isPressedHandler;
    SmartButton::EventCallback eventCallback;
    void *context;

    const unsigned long debounceTimeout;
    const unsigned long clickTimeout;
    const unsigned long holdTimeout;
    const unsigned long longHoldTimeout;
    const unsigned long holdRepeatPeriod;
    const unsigned long longHoldRepeatPeriod;

    unsigned long pressTick;
    unsigned long debounceTick;
    unsigned long repeatTick;

    bool pressedFlag;

    int clickCounter;
    SmartButton::State state;

    SmartButton *next;
};

extern SmartButton *_smartButtons;

};

#endif /* SMART_BUTTON_H */
