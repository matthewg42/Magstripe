Arduino V3B-K Magstripe Reader Library
======================================

Easy to use library for using V3B-K magstripe readers with Arduinos.

This is more or less a wrapper around Stephan King's code which has been 
floating around the nets for a while.  I fixed a small problem with 
a potential buffer overflow if someone jiggles a card around while it
is being read, or if a card is swiped too fast. 

Minimal Example
===============

    #include <Magstripe.h>
    
    void setup() {
        Serial.begin(9600);
        msInit(4,3,2);
    }
    
    void loop() {
        if (msReadCard() == MS_READ_OK) {
            Serial.print("Read a magstripe.  Card data: ");
            Serial.print(msFetchLastCard());
        }
        delay(100);
    }


Notes
=====

This library uses interrupts.  This may intefere with other code if interrupts
are used. 


-Mouse
