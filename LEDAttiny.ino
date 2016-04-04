#include <Adafruit_NeoPixel.h> 
#include <avr/eeprom.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
/*
 WS2811/Neopixel pattern switcher for ATtiny85 (and Arduino)
 Requires Adafruit NeoPixel Library
 WS2811 Signal, Digital Pin 4
 Button, Digital Pin 0
 GPL v3
*/

// Define
#define NUM_LEDS 9
#define DATA_PIN 4
#define BTN_PIN 3
#define BTN_DELAY 250
#define NUM_PATTERNS 10
#define CTR_THRESH 16

// Init Vars
uint8_t j = 0;
uint8_t pattern=1;
uint8_t buttonState=0;
uint8_t lastPix=0; 
uint8_t myPix=0;
uint8_t direction=1;
uint8_t counter=0;
uint8_t colors[3];
uint32_t setColor=0;
unsigned long mark;

uint32_t reseedRandomSeed EEMEM = 0xFFFFFFFF;

// Start Strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
     // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
    #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    // End of trinket special code
    
    pinMode(BTN_PIN, INPUT);     
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    
    reseedRandom( &reseedRandomSeed );    
    pattern = random(10);
}

void loop() {
    // if button pressed, advance, set mark
    chkBtn(digitalRead(BTN_PIN));
   
    // if pattern greater than #pattern reset
    if (pattern > NUM_PATTERNS) { pattern = 1; }
    
    // choose a pattern
    pickPattern(pattern);

    // set direction
    if (direction == 1) { j++;  } else {  j--; }

    if (j > 254) { direction = 0; }
    if (j < 1) { direction = 1; }   
	
}

/* pick a pattern */
void pickPattern(uint8_t var) {
      switch (var) {
        case 1:  colorWipeCenter(strip.Color(200,0,0),100);  //red                      
            break;
        case 2:  colorWipeCenter(strip.Color(0,200,0),100);  //green  
            break;
        case 3:  colorWipeCenter(strip.Color(0,0,200),100);  //blue
            break;
        case 4:  fadeEveOdd(random(360),0,10);
                 fadeEveOdd(random(360),1,10);             
            break;
        case 5:   fadeEveOdd(0,0,10);
                  fadeEveOdd(120,1,10);
            break;  
        case 6:  colorWipeCenter(strip.Color(random(255),random(255),random(255)),1000);  //blue    
            break;
        case 7:  police(200);
            break;
        case 8:  xmark(75);
            break;
        case 9:   rainbow(10);
                  counter++;
            break;
        case 10:  rainbowCycle(10);
                 counter++;
             break; 
      }
}


////////////////////////////////////////////////////////////////

/* check button state */
boolean chkBtn(int buttonState) {
   if (buttonState == HIGH && (millis() - mark) > BTN_DELAY) {
       j = 0;
       mark = millis();
       pattern++;
       return true;
    } 
    else { return false; }
}



// Fill the dots one after the other with a color
// Modified from Neopixel sketch to break on button press
// color wipe from center
void colorWipeCenter(uint32_t c, uint8_t wait) {
  uint8_t mid=strip.numPixels()/2;
  strip.setPixelColor(mid,c);
  for(uint16_t i=0; i<=strip.numPixels()/2; i++) {
      if(chkBtn(digitalRead(BTN_PIN))) { break; }
      strip.setPixelColor(mid+i, c);
      strip.setPixelColor(mid-i, c);
      strip.show();
      delay(wait);
  }
}

// Rainbow Cycle, modified from Neopixel sketch to break on button press
void rainbowCycle(uint8_t wait) 
{
    uint16_t i;
    
    for (i = 0; i < strip.numPixels(); i++) 
    {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
}

void rainbow(uint8_t wait) 
{
    uint16_t i;
    
    for (i = 0; i < strip.numPixels(); i++) 
    {
        strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
}

void police(uint8_t wait) 
{
    uint16_t i;
    
    for (i = 0; i < strip.numPixels(); i++) 
    {
        if( i == 2 || i == 5 || i == 8 )
          strip.setPixelColor(i, 0);
        else
          strip.setPixelColor(i, strip.Color(255,0,0));
    }
    strip.show();
    delay(wait);
    
    for (i = 0; i < strip.numPixels(); i++) 
    {
        if( i == 0 || i == 3 || i == 6 )
          strip.setPixelColor(i, 0);
        else
          strip.setPixelColor(i, strip.Color(0,0,255));
    }
    strip.show();
    delay(wait);
}

void xmark(uint8_t wait) 
{
    uint16_t i;
    
    for (i = 0; i < strip.numPixels(); i++) 
    {
        if( i == 1 || i == 3 || i == 5 || i == 7 )
          strip.setPixelColor(i, 0);
        else
          strip.setPixelColor(i, strip.Color(255,0,0));
    }
    strip.show();
    delay(wait);
    
    for (i = 0; i < strip.numPixels(); i++) 
    {
          strip.setPixelColor(i, 0);
    }
    strip.show();
    delay(wait);
}

void fadeEveOdd(int c1,byte rem,uint8_t wait) 
{
              for(int j=0; j < CTR_THRESH; j++) {
                      for(int i=0; i< strip.numPixels(); i++) {
                        if(i % 2== rem) {
                           HSVtoRGB(c1,255,(255/CTR_THRESH)*j,colors);
                           strip.setPixelColor(i,colors[0],colors[1],colors[2]);
                         }
                      }           
                      if(chkBtn(digitalRead(BTN_PIN))) { break; }
                      strip.show();
                      delay(wait);
                }
                for(int j=CTR_THRESH; j >= 0; j--) {
                      for(int i=0; i< strip.numPixels(); i++) {
                        if(i % 2== rem) {
                           HSVtoRGB(c1,255,(255/CTR_THRESH)*j,colors);
                           strip.setPixelColor(i,colors[0],colors[1],colors[2]);
                         }
                      }             
                     if(chkBtn(digitalRead(BTN_PIN))) { break; }
                      strip.show();
                      delay(wait);
                } 
}

////////////////////////////////////////////////////////////////
// helpers 

void colorFast(uint32_t c, uint8_t wait) {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    }
    strip.show();
    delay(wait);
}

uint32_t Wheel(byte WheelPos) {
    if (WheelPos < 85) {
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}


// HSV to RGB colors
// hue: 0-359, sat: 0-255, val (lightness): 0-255
// adapted from http://funkboxing.com/wordpress/?p=1366
void HSVtoRGB(int hue, int sat, int val, uint8_t * colors) {
    int r, g, b, base;
    if (sat == 0) { // Achromatic color (gray).
        colors[0] = val;
        colors[1] = val;
        colors[2] = val;
    } else {
        base = ((255 - sat) * val) >> 8;
        switch (hue / 60) {
        case 0:
            colors[0] = val;
            colors[1] = (((val - base) * hue) / 60) + base;
            colors[2] = base;
            break;
        case 1:
            colors[0] = (((val - base) * (60 - (hue % 60))) / 60) + base;
            colors[1] = val;
            colors[2] = base;
            break;
        case 2:
            colors[0] = base;
            colors[1] = val;
            colors[2] = (((val - base) * (hue % 60)) / 60) + base;
            break;
        case 3:
            colors[0] = base;
            colors[1] = (((val - base) * (60 - (hue % 60))) / 60) + base;
            colors[2] = val;
            break;
        case 4:
            colors[0] = (((val - base) * (hue % 60)) / 60) + base;
            colors[1] = base;
            colors[2] = val;
            break;
        case 5:
            colors[0] = val;
            colors[1] = base;
            colors[2] = (((val - base) * (60 - (hue % 60))) / 60) + base;
            break;
        }

    }
}

///////////////////////////////////////////////////////////////
/*==============================================================================
  Call reseedRandom once in setup to start random on a new sequence.  Uses 
  four bytes of EEPROM.
==============================================================================*/

void reseedRandom( uint32_t* address )
{
  static const uint32_t HappyPrime = 937;
  uint32_t raw;
  unsigned long seed;

  // Read the previous raw value from EEPROM
  raw = eeprom_read_dword( address );

  // Loop until a seed within the valid range is found
  do
  {
    // Incrementing by a prime (except 2) every possible raw value is visited
    raw += HappyPrime;

    // Park-Miller is only 31 bits so ignore the most significant bit
    seed = raw & 0x7FFFFFFF;
  }
  while ( (seed < 1) || (seed > 2147483646) );

  // Seed the random number generator with the next value in the sequence
  srandom( seed );  

  // Save the new raw value for next time
  eeprom_write_dword( address, raw );
}

inline void reseedRandom( unsigned short address )
{
  reseedRandom( (uint32_t*)(address) );
}


void reseedRandomInit( uint32_t* address, uint32_t value )
{
  eeprom_write_dword( address, value );
}

inline void reseedRandomInit( unsigned short address, uint32_t value )
{
  reseedRandomInit( (uint32_t*)(address), value );
}




