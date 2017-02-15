// This is a mash-up of the Due show() code + insights from Michael Miller's
// ESP8266 work for the NeoPixelBus library: github.com/Makuna/NeoPixelBus
// Needs to be a separate .c file to enforce ICACHE_RAM_ATTR execution.

#ifdef ESP8266

#include <Arduino.h>
#include <eagle_soc.h>

static uint32_t _getCycleCount(void) __attribute__((always_inline));
static inline uint32_t _getCycleCount(void) {
  uint32_t ccount;
  __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
  return ccount;
}

void ICACHE_RAM_ATTR espShow(
 uint8_t pin, uint8_t *pixels, uint32_t numBytes, boolean is800KHz) {

#define CYCLES_800_T0H  (F_CPU / 2100000) // 0.4us
#define CYCLES_800_T1H  (F_CPU / 1250000) // 0.8us
#define CYCLES_800      (F_CPU /  800000) // 1.25us per bit


  uint8_t *p, *end, pix, mask;
  uint32_t t, time0, time1, period, c, startTime, pinMask;

  pinMask   = _BV(pin);
  p         =  pixels;
  end       =  p + numBytes;
  pix       = *p++;
  mask      = 0x80;
  startTime = 0;


    time0  = 30;
    time1  = 80;
    period = 120;

 

  for(t = time0;; t = time0) {
	GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);
    if(pix & mask) t = time1; 
	while(((c = _getCycleCount()) - startTime) < period);        
    startTime = c;             
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);	
    while(((c = _getCycleCount()) - startTime) < t); 
    if(!(mask >>= 1)) {                                   
      if(p >= end) break;
      pix  = *p++;
      mask = 0x80;
    }
  }
  while((_getCycleCount() - startTime) < period); 
}

#endif // ESP8266
