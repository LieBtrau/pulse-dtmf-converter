#include "dtmfgenerator.h"

#define LED_PIN 13

DtmfGenerator dtmf;

void setup()
{
    // Setup the LED :
    pinMode(LED_PIN,OUTPUT);
    dtmf.init();
}

void loop()
{
    for(byte i=0;i<10;i++)
    {
        digitalWrite(LED_PIN,1);
        dtmf.generateTone('0'+i);
        delay(50);
        digitalWrite(LED_PIN,0);
        dtmf.stopTone();
        delay(100);
    }
    delay(3000);
}
