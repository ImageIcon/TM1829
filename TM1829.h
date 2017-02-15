#ifndef TM1829_H
#define TM1829_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#define NEO_RGB  ((0 << 6) | (0 << 4) | (1 << 2) | (2))
#define NEO_BRG  ((1 << 6) | (1 << 4) | (2 << 2) | (0))
#define NEO_KHZ800 0x0000 

typedef uint8_t  neoPixelType;


class TM1829 {

 public:

  // Constructor: number of LEDs, pin number, LED type
  TM1829(uint16_t n, uint8_t p=6, neoPixelType t=NEO_RGB + NEO_KHZ800);
  TM1829(void);
  ~TM1829();

  void
    begin(void),
    show(void),
    setPin(uint8_t p),
    setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b),
    setPixelColor(uint16_t n, uint32_t c),
    clear(),
    updateLength(uint16_t n),
    updateType(neoPixelType t);
  uint8_t
   *getPixels(void) const,
    getBrightness(void) const;
  int8_t
    getPin(void) { return pin; };
  uint16_t
    numPixels(void) const;
  static uint32_t
    Color(uint8_t r, uint8_t g, uint8_t b);
  uint32_t
    getPixelColor(uint16_t n) const;
  inline bool
    canShow(void) { return (micros() - endTime) >= 50L; }

 private:

  boolean
    begun;         // true if begin() previously called
  uint16_t
    numLEDs,       // Number of RGB LEDs in strip
    numBytes;      // Size of 'pixels' buffer below (3 or 4 bytes/pixel)
  int8_t
    pin;           // Output pin number (-1 if not yet set)
  uint8_t
   *pixels,        // Holds LED color values (3 or 4 bytes each)
    rOffset,       // Index of red byte within each 3- or 4-byte pixel
    gOffset,       // Index of green byte
    bOffset;
  uint32_t
    endTime;       // Latch timing reference
};

#endif // TM1829_H
