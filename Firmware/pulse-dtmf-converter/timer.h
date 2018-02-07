#ifndef TIMER_H
#define TIMER_H
#include "Arduino.h"


class Timer
{
public:
    typedef enum{NO_PRESCALING}PRESCALER;
    Timer(){};
    void initialize();
    void setPrescaler(PRESCALER p);
    void enablePwm();
    void attachInterrupt(void (*isr)());
    void detachInterrupt();
    void setPwmDuty(byte ocr);
    void (*isrCallback)();
};

extern Timer timer;

#endif // TIMER_H
