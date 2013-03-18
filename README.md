Arduino Magstripe Library for V3B-K Type Devices
================================================

The magstripe reader I have is labeled V3B-K.  It's a pretty cheap unit ($15) with 
5 wires emerging from the rear of the unit, terminated in a plastic connector.

The wires are color coded:

- Green: Ground
- Yellow: +5V DC
- Orange: Data ready
- Red: Clock
- Brown: Data value

When a card is swiped, raw card data is can be read by serial.  This data must
be decoded to retrieve the ASCII card contents.

I found some code on the Internet for reading the serial and decoding the data, 
written by a Stephen King.  The code worked, but suffered a problem where the
input buffer would overflow if a card swipe produced too much data for the
statically allocated buffer.  This could happen if the card was not swiped
too fast, or if the user jiggled the card back and forth instead of doing a
normal swipe.  Buffer overflows could cause the arduino to lock up or produce
erroneous results on subsequent reads.

Anyway, I added from buffer overflow protection, and wrapped up the code into 
this easy to use library.

See the Doxygen generated documentation for API and examples.

-Mouse

