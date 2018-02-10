/*
DtmfGenerator: library for timer
Copyright (C) 2018  Christoph Tack

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMER_H
#define TIMER_H
#include "Arduino.h"


class Timer
{
public:
    typedef enum{CK1}PRESCALER;
    Timer(){};
    void initialize();
    void setPrescaler(PRESCALER p);
    void pwm();
    void disablePwm();
    void attachInterrupt(void (*isr)());
    void detachInterrupt();
    void setPwmDuty(byte ocr);

    void (*isrCallback)();
};

extern Timer timer;

#endif // TIMER_H
