/*
DESCRIPTION
====================
Reports through serial (57600 baud) the time since
a button press (transition from HIGH to LOW).

*/

// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include <Bounce2.h>
#include "dtmfgenerator.h"

#define BUTTON_PIN 2
#define LED_PIN 13

// Instantiate a Bounce object :
Bounce debouncer = Bounce();

unsigned long buttonPressTimeStamp;
unsigned long ulTimer;
DtmfGenerator dtmf;

void setup()
{
    Serial.begin(9600);

    // Setup the button with an internal pull-up :
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    // After setting up the button, setup the Bounce instance :
    debouncer.attach(BUTTON_PIN);
    debouncer.interval(5);

    // Setup the LED :
    pinMode(LED_PIN,OUTPUT);
    Serial.println("start");
    ulTimer=millis();
    dtmf.init();
    dtmf.generateTone('0');
}

void loop()
{

    // Update the Bounce instance :
    debouncer.update();

    // Call code if Bounce fell (transition from HIGH to LOW) :
    if ( debouncer.fell()  )
    {;

        Serial.println( millis()-buttonPressTimeStamp );
        buttonPressTimeStamp = millis();

    }
    if(millis()-ulTimer>500)
    {
        ulTimer=millis();
        digitalWrite(LED_PIN, digitalRead(LED_PIN) ? 0 : 1);
    }
}
