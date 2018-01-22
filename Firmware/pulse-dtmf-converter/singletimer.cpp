#include "singletimer.h"
#include "Arduino.h"

void SingleTimer::start(unsigned long timeout)
{
    ulTimeout=timeout;
    ulStarttime=millis();
    bRunning=true;
}

bool SingleTimer::hasRunout()
{
    if(!bRunning)
    {
        return true;
    }
    bRunning = millis() < ulStarttime+ulTimeout;
    return  !bRunning;
}
