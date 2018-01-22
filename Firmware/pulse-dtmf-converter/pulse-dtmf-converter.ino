#include "dtmfgenerator.h"
#include "rotarydialer.h"

DtmfGenerator dtmf;
RotaryDialer rd(5);

void setup()
{
    dtmf.init();
    rd.init();
}

void loop()
{
    rd.update();
    if(rd.available())
    {
        byte nr=rd.read();
    }
//    for(byte i=0;i<10;i++)
//    {
//        digitalWrite(LED_PIN,1);
//        dtmf.generateTone('0'+i);
//        delay(50);
//        digitalWrite(LED_PIN,0);
//        dtmf.stopTone();
//        delay(100);
//    }
//    delay(3000);
}
