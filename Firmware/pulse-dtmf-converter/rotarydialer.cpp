#include "rotarydialer.h"

RotaryDialer::RotaryDialer(byte pulsePin):_pulsePin(pulsePin){}

void RotaryDialer::init()
{
    pinMode(_pulsePin, INPUT);
    _debouncer.attach(_pulsePin);
    _debouncer.interval(5);                             // interval in ms
}

void RotaryDialer::update()
{
    _debouncer.update();
    if(_debouncer.fell())                               //Start of pulse detected
    {
        _dataReady=false;                               //Make sure data will not be read when pulses are coming in
        if(_pulseTimeout.hasRunout())                   //This is the first in a series of pulses
        {
            _pulseCounter=0;
        }
        _pulseCounter++;
        _pulseTimeout.start(150);
    }
    if(_pulseTimeout.hasRunout() && _pulseCounter>0)    //data ready when no more new pulses arrive and at least one pulse
    {                                                   //arrived lately
        _dataReady=true;
    }
}

bool RotaryDialer::available()
{
    return _dataReady;
}

byte RotaryDialer::read()
{
    byte number=_pulseCounter==10 ? 0 : _pulseCounter;
    _dataReady=false;                                       //Make sure each number is read only once
    _pulseCounter=0;
    return number;
}

