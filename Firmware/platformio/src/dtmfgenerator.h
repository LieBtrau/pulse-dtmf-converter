/*
DtmfGenerator: library for generating DTMF-signals
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

#ifndef DTMFGENERATOR_H
#define DTMFGENERATOR_H
#include "timer.h"

class DtmfGenerator
{
public:
    DtmfGenerator(){};
    void init (void);
    bool generateTone(char key);
    void stopTone();
private:
    const char* keypad = "123A456B789C*0#D"; // position for lookup into Freq. Table
};

#endif // DTMFGENERATOR_H
