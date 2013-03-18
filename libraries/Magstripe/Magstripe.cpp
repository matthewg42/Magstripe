// Arduino Magstripe reader library, for use with V3B-K magstripe readers
// (C) Copyright 2013 Matthew Gates <matthewg42@gmail.com> (@mousefad)
// This code was derived from code originally written by Stefan King
// which is understood to be in the public domain.
//
// This code is distributed under the terms of the GNU LGPL Version 3
// See http://www.gnu.org/licenses/lgpl.txt for more details.

#include "Magstripe.h"
#include "Arduino.h"

void msInit(int rdyPin, int cclkPin, int dataPin)
{
  _msRdyPin = rdyPin;
  _msCClkPin = cclkPin;
  _msDataPin = dataPin;

//  pinMode(_msRdyPin, INPUT);
//  pinMode(_msCClkPin, INPUT); 
//  pinMode(_msDataPin, INPUT);

  // Make sure the card buffer is empty as we have not read a card yet.
  _msResetCardBuffer();
  _msResetDataBuffer();

  // Set up interrupts as required
  attachInterrupt(0, _msChangeBit, CHANGE);
  attachInterrupt(1, _msWriteBit, FALLING);
    
}

int msReadCard()
{
  while(digitalRead(_msRdyPin) == LOW) {
    _msReading = 1;
  }

  if (!_msReading)
    return MS_ERR_NOT_READY;
  
  _msResetCardBuffer();

  int sentinal = _msGetStartSentinal();
  int j;
  int i = 0;
  int k = 0;
  int thisByte[5];
  int ret;
   
  for(j=sentinal; j<MS_DATABUF_SZ-sentinal; j++)
  {
    thisByte[i] = _msDataBuf[j];
    i++;
    if(i % 5 == 0)
    {
      i = 0;
      
      // detect end of data (NULL accross four consecutive components
      if(thisByte[0]==0 & thisByte[1]==0 & thisByte[2]==0 & thisByte[3]==0 & thisByte[4]==0)
        break;
      
      _msPrintMyByte(thisByte); // append char to card buffer
    }
  }

  _msCardBuf[_msCardPtr] = 0; // NULL terminate card

  ret = _msErrorFlag;
  //_msErrorFlag = 0;
  //_msReading = 0;
  //_msBit = 0;
  
  // reset the read buffer and reading/error flags
  _msResetDataBuffer();
  
  return ret;
}

const char* msFetchLastCard()
{
  return _msCardBuf;
}

void msDebug()
{
  Serial.print("Magstripe: rdypin=");
  Serial.print(_msRdyPin);
  Serial.print("; crdclk=");
  Serial.print(_msCClkPin);
  Serial.print("; datapin=");
  Serial.print(_msDataPin);
  Serial.print("; cardbuffer=\"");
  Serial.print(_msCardBuf);
  Serial.print("\"; error=");
  Serial.println(_msErrorFlag);
}

// Flips the global bit
void _msChangeBit()
{
  if(_msBit == 0)
    _msBit = 1;
  else
    _msBit = 0;

  return;
}

// Writes the bit to the buffer
void _msWriteBit()
{
  if (_msDataPtr >= MS_DATABUF_SZ) {
    _msErrorFlag = MS_ERR_RAW_OVERFLOW;
    _msDataPtr = 0;
  }
  
  _msDataBuf[_msDataPtr] = _msBit;
  _msDataPtr++;
}

int _msGetStartSentinal()
{
  int j;
  int queue[5];
  int sentinal = 0;
 
  for(j=0; j<MS_DATABUF_SZ; j++)
  {
    queue[4] = queue[3];
    queue[3] = queue[2];
    queue[2] = queue[1];
    queue[1] = queue[0];
    queue[0] = _msDataBuf[j];
  
    if(queue[0] == 0 & queue[1] == 1 & queue[2] == 0 & queue[3] == 1 & queue[4] == 1)
    {
      sentinal = j - 4;
      break;
    }
  }
  
  return sentinal;
}

void _msPrintMyByte(int thisByte[])
{
  if (_msCardPtr >= MS_CARDBUF_SZ) {
    _msErrorFlag = MS_ERR_CARD_OVERFLOW;
    _msCardPtr = 0;    
  }
  
  _msCardBuf[_msCardPtr] = _msDecodeByte(thisByte);
  _msCardPtr++;
  return;
}

char _msDecodeByte(int thisByte[])
{
  if(thisByte[0] == 0 & thisByte[1] == 0 & thisByte[2] == 0 & thisByte[3] == 0 & thisByte[4] == 1)
    return '0';
    
  if(thisByte[0] == 1 & thisByte[1] == 0 & thisByte[2] == 0 & thisByte[3] == 0 & thisByte[4] == 0)
    return '1';
    
  if(thisByte[0] == 0 & thisByte[1] == 1 & thisByte[2] == 0 & thisByte[3] == 0 & thisByte[4] == 0)
    return '2';
 
  if(thisByte[0] == 1 & thisByte[1] == 1 & thisByte[2] == 0 & thisByte[3] == 0 & thisByte[4] == 1)
    return '3';
 
  if(thisByte[0] == 0 & thisByte[1] == 0 & thisByte[2] == 1 & thisByte[3] == 0 & thisByte[4] == 0)
    return '4';
      
  if(thisByte[0] == 1 & thisByte[1] == 0 & thisByte[2] == 1 & thisByte[3] == 0 & thisByte[4] == 1)
    return '5';
 
  if(thisByte[0] == 0 & thisByte[1] == 1 & thisByte[2] == 1 & thisByte[3] == 0 & thisByte[4] == 1)
    return '6';
 
  if(thisByte[0] == 1 & thisByte[1] == 1 & thisByte[2] == 1 & thisByte[3] == 0 & thisByte[4] == 0)
    return '7';
 
  if(thisByte[0] == 0 & thisByte[1] == 0 & thisByte[2] == 0 & thisByte[3] == 1 & thisByte[4] == 0)
    return '8';
 
  if(thisByte[0] == 1 & thisByte[1] == 0 & thisByte[2] == 0 & thisByte[3] == 1 & thisByte[4] == 1)
    return '9';
 
  if(thisByte[0] == 0 & thisByte[1] == 1 & thisByte[2] == 0 & thisByte[3] == 1 & thisByte[4] == 1)
    return ':';
 
  if(thisByte[0] == 1 & thisByte[1] == 1 & thisByte[2] == 0 & thisByte[3] == 1 & thisByte[4] == 0)
    return ';';
 
  if(thisByte[0] == 0 & thisByte[1] == 0 & thisByte[2] == 1 & thisByte[3] == 1 & thisByte[4] == 1)
    return '<';
 
  if(thisByte[0] == 1 & thisByte[1] == 0 & thisByte[2] == 1 & thisByte[3] == 1 & thisByte[4] == 0)
    return '=';
 
  if(thisByte[0] == 0 & thisByte[1] == 1 & thisByte[2] == 1 & thisByte[3] == 1 & thisByte[4] == 0)
    return '>';
 
  if(thisByte[0] == 1 & thisByte[1] == 1 & thisByte[2] == 1 & thisByte[3] == 1 & thisByte[4] == 1)
    return '?';
    
  return 'X';
}

void _msResetDataBuffer()
{
  int i;
  for(i=0; i<MS_DATABUF_SZ; i++) {
    _msDataBuf[i] = 0;
  }
  _msDataPtr = 0;
  _msErrorFlag = 0;
  _msReading = 0;
  _msBit = 0;
}

void _msResetCardBuffer()
{
  //memset(_msCardBuf, 0, sizeof(char)*MS_CARDBUF_SZ+1);
  int i;
  for(i=0; i<MS_CARDBUF_SZ+1; i++) {
    _msCardBuf[i] = 0;
  }

  _msCardPtr = 0;
}
