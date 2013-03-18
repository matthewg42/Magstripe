#include <Magstripe.h>
#include <Arduino.h>
#include <avr/wdt.h>    // watchdog timer

int reread_ticks = 0;

void setup()
{
  Serial.begin(9600); 
  
  // initialize magstripe library. 
  // data ready pin = 4,
  // card clock pin = 3
  // data read pin  = 2
  msInit(4,3,2);
  
  // enable watchdog timer (in case of arduino hang,
  // reset after 4 seconds)
  wdt_enable(WDTO_4S);

  Serial.println("setup() done");
}

void loop()
{
  int mag_read_status;
  
  if (reread_ticks > 0)
    reread_ticks--;
    
  mag_read_status = msReadCard();
  if (mag_read_status == MS_READ_OK) {
    if (reread_ticks == 0) {
      Serial.print("Read card data: ");
      Serial.println(msFetchLastCard());
      reread_ticks = 40;
    }
    else {
      Serial.println("Card reread blocked (too soon since last time)");
    }
  }
  else if (mag_read_status == MS_ERR_RAW_OVERFLOW || mag_read_status == MS_ERR_CARD_OVERFLOW) {
    Serial.println("Magstripe read error: buffer overflow (card wiggle attack?)");
  }

  // reset the watchdog timer to prevent reboot
  wdt_reset();
  
  // wait a tick
  delay(50);
  
}

