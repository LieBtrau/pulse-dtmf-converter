#ifndef ROTARYDIALER_H
#define ROTARYDIALER_H

#include "Arduino.h"
#include "Bounce2.h"
#include "singletimer.h"

class RotaryDialer
{
public:
    RotaryDialer(byte pulsePin);
    void init();
    void update();
    bool available();
    byte read();
private:
    byte _pulsePin;
    Bounce _debouncer;
    SingleTimer _pulseTimeout;
    byte _pulseCounter;
    bool _dataReady;
};

#endif // ROTARYDIALER_H
