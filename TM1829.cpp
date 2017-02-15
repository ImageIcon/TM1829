#include "TM1829.h"

// Constructor when length, pin and type are known at compile-time:
TM1829::TM1829(uint16_t n, uint8_t p, neoPixelType t) :
  begun(false), pixels(NULL), endTime(0)
{
  updateType(t);
  updateLength(n);
  setPin(p);
}
TM1829::TM1829() :

  begun(false), numLEDs(0), numBytes(0), pin(-1), pixels(NULL),
  rOffset(1), gOffset(0), bOffset(2), endTime(0)
{
}

TM1829::~TM1829() {
  if(pixels)   free(pixels);
  if(pin >= 0) pinMode(pin, INPUT);
}

void TM1829::begin(void) {
  if(pin >= 0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  begun = true;
}

void TM1829::updateLength(uint16_t n) {
  if(pixels) free(pixels); // Free existing data (if any)

  // Allocate new data -- note: ALL PIXELS ARE CLEARED
  numBytes = n * 3;
  if((pixels = (uint8_t *)malloc(numBytes))) {
    memset(pixels, 0, numBytes);
    numLEDs = n;
  } else {
    numLEDs = numBytes = 0;
  }
}

void TM1829::updateType(neoPixelType t) {
  
  
  rOffset = (t >> 4) & 0b11; 
  gOffset = (t >> 2) & 0b11;
  bOffset =  t       & 0b11;

  if(pixels) {
    updateLength(numLEDs);
  }
}


extern "C" void ICACHE_RAM_ATTR espShow(
  uint8_t pin, uint8_t *pixels, uint32_t numBytes, uint8_t type);


void TM1829::show(void) {

  if(!pixels) return;
  while(!canShow());
  noInterrupts(); // Need 100% focus on instruction timing
	
	espShow(pin, pixels, numBytes, true);

  interrupts();
  endTime = micros(); // Save EOD time for latch on next call
}

// Set the output pin number
void TM1829::setPin(uint8_t p) {
  if(begun && (pin >= 0)) pinMode(pin, INPUT);
    pin = p;
    if(begun) {
      pinMode(p, OUTPUT);
      digitalWrite(p, LOW);
    }
}

// Set pixel color from separate R,G,B components:
void TM1829::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < numLEDs) {
    uint8_t *p;
    p = &pixels[n * 3];    // 3 bytes per pixel
    p[rOffset] = r;          // R,G,B always stored
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

// Set pixel color from 'packed' 32-bit RGB color:
void TM1829::setPixelColor(uint16_t n, uint32_t c) {
  if(n < numLEDs) {
    uint8_t *p,
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
 
      p = &pixels[n * 3];
    p[rOffset] = r;
    p[gOffset] = g;
    p[bOffset] = b;
  }
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t TM1829::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}


uint8_t *TM1829::getPixels(void) const {
  return pixels;
}

uint16_t TM1829::numPixels(void) const {
  return numLEDs;
}

void TM1829::clear() {
  memset(pixels, 0, numBytes);
}