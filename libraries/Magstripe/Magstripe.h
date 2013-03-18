// Arduino Magstripe reader library, for use with V3B-K magstripe readers
// (C) Copyright 2013 Matthew Gates <matthewg42@gmail.com> (@mousefad)
// This code was derived from code originally written by Stefan King
// which is understood to be in the public domain.
//
// This code is distributed under the terms of the GNU LGPL Version 3
// See http://www.gnu.org/licenses/lgpl.txt for more details.

#ifndef MAGSTRIPE_H
#define MAGSTRIPE_H

//! @file Magstripe.h
//! @brief Magstripe reading library for Arduino
//!
//! This library allow you to interface with V3B-K type magstripe readers.  These
//! readers do no decode the content of magstripe cards, instead sending the raw
//! data along a serial interface to the arduino with three data lines.
//! 
//! This library uses interrupts to put this raw data into a buffer, and then
//! de-codes it into a card buffer, which can be read with a simple function call.
//! 
//! Functions prefixed with an underscore (_ms...) are intended for internal use - 
//! only functions beginning with ms need be used directly.
//! 
//! @note at time of writing, the clock pin value passed to msInit() does nothing
//!       so it should be left as the default value: 3.

#include <Arduino.h>

#define MS_READ_OK              0   //!< Card read status: read a card OK
#define MS_ERR_NOT_READY	    1   //!< Card read failed: no card has completed reading
#define MS_ERR_RAW_OVERFLOW	    2   //!< Card read failed: raw buffer overflow before card completed (e.g. 'wiggle attack')
#define MS_ERR_CARD_OVERFLOW	3   //!< Card read failed: decoded card buffer overflowed (probably a bug)

#define MS_DATABUF_SZ	400         //!< The size of the buffer for raw data (in bytes, internal use only)
#define MS_CARDBUF_SZ	40          //!< The size of the decoded card data buffer (in bytes, internal use only)

//! @brief Initialize the Magstripe library
//!
//! This only needs calling once and should generally be done from the setup() function in 
//! your arduino sketch.
//! @param rdyPin arduino pin connected to data ready line from Magstripe reader (orange wire)
//! @param cclkPin arduino pin connected to card clock line from Magstripe reader (red wire)
//! @param dataPin arduino pin connected to data line from Magstripe reader (brown wire)
void msInit(int rdyPin, int cclkPin, int dataPin);

//! @brief Try to read a magstripe card.
//!
//! Reads data from the card if it is ready, decodes it and puts the
//! result in the card data buffer, which can then be retrieved using
//! fetchCardData().  This should be called from the loop() function in
//! your arduino sketch whenever you think there might be a card swipe
//! to read (e.g. every few hundred milliseconds).  If no card has been 
//! swiped then the return value will be MS_ERR_NOT_READY.
//! @return MS_READ_OK if a card was read and decoded without an error being detected,
//!         else one of the error codes: MS_ERR_NOT_READY (no card has been swiped),
//!         MS_ERR_RAW_OVERFLOW (card has more data than expected), or
//!         MS_ERR_CARD_OVERFLOW (more data ended up in the card buffer then expected).
int msReadCard();

//! @brief Fetch the card contents for the last read card.
//!
//! Get a pointer to a null-terminated string containing the last card data which was read.
//! @note This function should not be called unless a card has just been sucessfully read
//!       (i.e. msReadCard was called and returned MS_READ_OK).
//! @return a pointer to the card buffer of the last read card contents.  The data is in
//!         the form of a NULL terminated character string. If card has been read, the
//!         pointer will point at an empty string.
const char* msFetchLastCard();

//! @brief Dump some internal state to the serial interface.
//!
//! Dumps the pin values and some other data to the serial interface using Serial.print().
void msDebug();

void _msChangeBit();                            //!< used to set raw buffer (internal use only)
void _msWriteBit();                             //!< used to set raw buffer (internal use only)
int _msGetStartSentinal();                      //!< used to detect beginning of card data (internal use only)
void _msPrintMyByte(int thisByte[]);            //!< used to decode data (internal use only)
char _msDecodeByte(int thisByte[]);             //!< used to decode data (internal use only)
void _msResetDataBuffer();                      //!< used to ensure clear raw buffer between reads (internal use only)
void _msResetCardBuffer();                      //!< used to ensure clear card buffer between reads (internal use only)

static int _msRdyPin  = 4;			            //!< Data ready pin from Magstripe reader (orange)
static int _msCClkPin = 3;			            //!< Card clock pin from Magstripe reader (red)
static int _msDataPin = 2;			            //!< Data pin from Magstripe reader (brown)
static int _msReading = 0; 			            //!< Flag to determine if a card is currently being read or not
static volatile int _msDataBuf[MS_DATABUF_SZ];	//!< Raw data buffer (still stripe encoded)
static volatile int _msDataPtr = 0; 		    //!< Pointer for use with the data buffer
static volatile int _msBit = 0; 		        //!< "bit" used with interrupts for reading incoming serial data
static int _msCardPtr = 0; 			            //!< Counter for card characters
static char _msCardBuf[MS_CARDBUF_SZ+1]; 	    //!< Where the card textual data ends up

//! Flag to show buffer overflow state when reading (or other internal error)
//!     0 = no error detected
//!     1 = data buffer overflow
//!     2 = card buffer overflow
static int _msErrorFlag = 0;

#endif

