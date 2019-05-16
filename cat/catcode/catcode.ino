
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Adafruit_WS2801.h"
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(144, 7, NEO_GRBW + NEO_KHZ800);


#define EFFECT_MEOW 2
#define EFFECT_PURR 1
#define EFFECT_HISS 3
#define EFFECT_MULTIMEOW 4

short effect_holdoffs[]={0,1200,400, 200, 860};
#define S_NOTHING 0
#define S_APPROACH 1
#define S_HERE 3
#define S_LEAVE 2

void personality(byte s) {
 static short timer=0;

  switch(s) {
    case(S_NOTHING):
      if (timer == 0) {
   //     start_sndeffect(EFFECT_MULTIMEOW);
        timer = 20000;  
      }
      timer--;
      break;
    case(S_APPROACH):
        push_sndeffect(EFFECT_MEOW);
        Serial.println("APPROACH");
        break;
    case(S_HERE):
        start_sndeffect(EFFECT_PURR);
        Serial.println("HERE");
        break;
    case(S_LEAVE):
       push_sndeffect(EFFECT_HISS);
        Serial.println("LEAVE");
        break;
  }  
}

short eftimer;
byte next_effect;
byte playing_effect;
void push_sndeffect(byte effect) {
  
    next_effect = effect;
}
void start_sndeffect(byte effect) {

  if (eftimer) {
    return;
  }

     eftimer = effect_holdoffs[effect];

  Serial.print("effect: ");
  Serial.print(effect);
  Serial.print(" ");
  Serial.println(eftimer);
  if (effect&1) {
    digitalWrite(4, 1);
    
  }
  if (effect&2) {
    digitalWrite(5, 1);
  }

  if (effect&4) {
    digitalWrite(9, 1);
  }
  //digitalWrite(13, 1);
}

void check_sndeffect() {
  if (eftimer == 150) {
    digitalWrite(4, 0);
    digitalWrite(5, 0);
    digitalWrite(9, 0);
    //digitalWrite(13, 0);
  }

  if (eftimer == 0) {

    if (next_effect) {

        start_sndeffect(next_effect);
        next_effect = 0;
    }
  } else {
    eftimer--;
  }
 
}


void setup() {
  // put your setup code here, to run once:
   pinMode(4, OUTPUT);
   pinMode(2, OUTPUT);
   pinMode(5, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(13, OUTPUT);
   pinMode(6, OUTPUT);
   digitalWrite(10, 1);
   pinMode(9, OUTPUT);
 digitalWrite(6, 0);
 delay(2000);
 digitalWrite(6, 1);
 delay(1000);
 digitalWrite(6,0);
 pixels.begin();
 Serial.begin(9600);
}

byte lastmeasure;
byte lastlastmeasure;
byte measure_stable_count;
byte last_s;
void loop() {

  doleds();
 // delay(10);
 

  byte m = measure();
  if(m) {
    start_sndeffect(EFFECT_MEOW);
  digitalWrite(13, 1);
  } else {
    digitalWrite(13, 0);
  }

  check_sndeffect();
  /*
  if (m== lastmeasure) {
    measure_stable_count ++;
  } else {
    measure_stable_count = 0;
  }
  if (measure_stable_count > 10) {
    byte s;
    s = m | (lastlastmeasure<<1);
    personality(s);
    lastlastmeasure = m;
    measure_stable_count = 0;
    if (last_s != s) {
      Serial.print("STATE: ");
      Serial.print(last_s);
      Serial.print(" ");
      Serial.print(s);
      Serial.print(" ");
      Serial.println(next_effect);
      
    }
    last_s = s;
  }
  check_sndeffect();
  lastmeasure = m;
*/
  /*
  if (measure()) {
    digitalWrite(4,1);
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(4,0);
    digitalWrite(13, 0);
  }
  */
}




void doleds() {
  int i;
  static int pround = 0;
  byte intensity=16;

  if (analogRead(4) < 700) {
    intensity=4;
  }
  pround++;

  if (pround > 144) {
    pround = 0;
  }
  for (i = 0; i < 144; i++) {
    int p = pround + i;
    if (p >=144) {
      p-=144;
    }
    pixels.setPixelColor(p, nextrainbow(i, intensity));
  }
   pixels.show();
  
}

#define COLOR_SECTORS_DIV 24
uint32_t nextrainbow(int i, unsigned int scale) {
  int mode = i / COLOR_SECTORS_DIV;

  if (scale > 16) {
    scale = 16;
  }

  unsigned int level = ((i % COLOR_SECTORS_DIV) * 2) / 3 + 1;
  unsigned int big = scale*16;

  if (big==256) {
    big--;
  }
  level = level * level;
  if (level == 256) {
    level--;
  }

  int levelL = ((255-level)*scale)>>4;
  int levelH = (level*scale)>>4;
  
  //level = big-level;
  //level = (level * scale)>>4;

  switch (mode) {
    /*
      case(0): return pixels.Color(255,0,0,0);
      case(1): return pixels.Color(255,255,0,0);
      case(2): return pixels.Color(0,255,0,0);
      case(3): return pixels.Color(0,255,255,0);
      case(4): return pixels.Color(0,0,255,0);
      case(5): return pixels.Color(255,0,255,0);
    */

    case (0): return Color(big, levelH, 0, 0);
    case (1): return Color(levelL, big, 0, 0);
    case (2): return Color(0, big, levelH, 0);
    case (3): return Color(0, levelL, big, 0);
    case (4): return Color(levelH, 0, big, 0);
    case (5): return Color(big, 0, levelL, 0);
    default: return Color(0, 0, 0, 10);
  }

}
uint32_t Color(byte r, byte g, byte b, byte x)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

byte measure() {
  int counter = 0;
  digitalWrite(2,1);
  delay(1);
  digitalWrite(2,0);


  while (digitalRead(3) == 0);
  while (digitalRead(3) == 1) {
    delay(1);
    //digitalWrite(7, counter&1);
    counter++;
    if (counter > 5) {
      delay(8-counter);
      return 0;
    }
  }
  delay(3);
  return 1;
}
