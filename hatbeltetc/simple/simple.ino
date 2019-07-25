
/* Libraries we need */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

/* Setup for the newpixel */
#define LEDS 144


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS, 2, NEO_GRBW + NEO_KHZ800);

/* Setup for the display board */
Adafruit_SSD1306 display(8);


/* Init function */
void setup(void) {

  analogReference(EXTERNAL);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();
  display.clearDisplay();

  pixels.begin();
};


/* Some global variables */
short phase=0;
short last_button = 0;
short mode = 0;

/* Figure out which button was pressed. Return 0 (no button pressed) or 1-4 */
short button_check() {
  short s = analogRead(1) / 4;
  short button = 0;
  if (s < 20) {
    button = 1;
  } else if (s > 35 && s < 60) {
    button = 2;
  } else if (s > 65 && s < 90) {
    button = 4;
  } else if ( s > 95 && s < 110) {
    button = 3;
  }

  return button;
}

/*
 * Writes some words to the display.
 */
void update_display(void) {
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.println(phase);
  display.println(mode);
  display.display();
  
}

/*
 * Runs over and over and over again 
 */
void loop(void) {
  short button;

  /* Change the mode (which is the color) every time a button is pressed */
  button=button_check();
  if (button != last_button && button > 0) {
    mode++;
    if (mode > 7) mode = 0;
  }

  /* Keep track of the button pressed right now so we know when it CHANGES */
  last_button = button;

  /* Draw stuff on the screen */
  update_display();
  
  short i;

  /*
   * Loop and decide the color of every LED
   */
  for ( i = 0; i < LEDS; i++) {
    
    short pos; /* Which LED??? */
    short value; /* How bright */

    /* Compute the LED we want to set.  This provides for an effect that rotates down the chain.
     */
    pos = phase + i;
    if (pos > LEDS) pos-=LEDS;

    /* Compute the brightness that we want */
    value = i;


    /* pixels.setPixelColor() is the critical function. its arguments are:
     *  LED POSITION, RED, GREEN, BLUE, WHITE. 
     *  LED POSITION goes from 0 to 299 and rgbw to from 0 to 255.
     *  
     *  Set different colors depending on the mode. 
     */
    switch(mode) {
      case(0):
        pixels.setPixelColor(pos,value, 0, 0, 0); 
        break;
      case(1):
        pixels.setPixelColor(pos, value, value, 0, 0);
        break;
      case(2):
        pixels.setPixelColor(pos, 0, value, 0, 0);
        break;
      case(3):
        pixels.setPixelColor(pos, 0, value, value, 0);
        break;
      case(4):
        pixels.setPixelColor(pos, 0, 0, value, 0);
        break;
      case(5):
        pixels.setPixelColor(pos, value, 0, value, 0);
        break; 
      case(6):
        pixels.setPixelColor(pos, 0, 0, 0, value);
        break;
      case(7):
        pixels.setPixelColor(pos, nextrainbow(i, 15)); 
        break;
    }
      
  }

  /* adjust phase. phase offsets the calculations in the above loop to make an effect that slowly streams across the belt. 
   *  when it gets to the end, put phase back to 0.
   */
  phase++;
  if (phase > LEDS) {
    phase = 0;
  }

  /*
   * Actually send the data we computed above to the belt.
   */
  pixels.show();
  
}



uint32_t nextrainbow(int i, unsigned int scale) {
  int mode = i / (LEDS/6);

  if (scale > 16) {
    scale = 16;
  }

  unsigned int level = ((i % 24) * 2) / 3 + 1;
  unsigned int big = scale*16;

  if (big==256) {
    big--;
  }
  level = level * level;
  if (level >= 256) {
    level=255;
  }

  int levelL = ((255-level)*scale)>>4;
  int levelH = (level*scale)>>4;
  
  //level = big-level;
  //level = (level * scale)>>4;

  switch (mode) {

    case (0): return pixels.Color(big, levelH, 0, 0);
    case (1): return pixels.Color(levelL, big, 0, 0);
    case (2): return pixels.Color(0, big, levelH, 0);
    case (3): return pixels.Color(0, levelL, big, 0);
    case (4): return pixels.Color(levelH, 0, big, 0);
    case (5): return pixels.Color(big, 0, levelL, 0);
    default: return pixels.Color(0, 0, 0, 10);
  }
}
