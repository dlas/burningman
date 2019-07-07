
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

  byte mode = readdip();
  doleds(mode);
 // delay(10);
 

  byte m = measure();
  if(m) {
  //  digitalWrite(5,1);
  //  delay(100);
   // digitalWrite(5,0);
    start_sndeffect(EFFECT_MEOW);
  digitalWrite(13, 1);
  } else {
    digitalWrite(13, 0);
  }
  delay(2);
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

byte readdip(void) {
  byte x = 0;
  if (analogRead(0) > 500) {
    x|=1;
  }

  if(analogRead(1) > 500) {
    x|=2;
  }

  if (analogRead(2) > 500) {
    x|=4;
  }

  if (analogRead(3) > 500) {
    x|=8;
  }

  return x;
  
}



void doleds(byte mode) {
  int i;
  static int pround = 0;
  byte intensity=16;

  if (analogRead(4) > 600) {
    intensity=3;
  }
  // intensity=4;
  switch (mode &3) {
    case(0):
    effect_rainbow(intensity);
    break;
    
    case(1):
    effect_wipe2(intensity);
    break;
    
    case(2):
    effect_split(intensity);
    break;
    
    case(3):
    effect_chase(intensity);
    break;
  }
  //effect_split(intensity);
  //effect_chase(intensity);
}

void effect_rainbow(byte intensity) {
  static byte pround;
   byte i;
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


void effect_sparkle(byte intensity) {
  static short rc;


  pixels.clear();
  rc+=random(3)-1;
  if (rc < 0) {
    rc+=144;
  }
  if (rc >= 144) {
    rc-=144;
  }
  byte i;
  byte pos;
  uint32_t bgc = nextrainbow(rc, intensity/4 + 1);

  for (i = 0; i < 144; i++) {
    pixels.setPixelColor(i, bgc);
  }

  for (i = 0; i < 2; i++) {
    pos = random(144);
    pixels.setPixelColor(pos, Color(i2color(intensity),i2color(intensity),i2color(intensity),i2color(intensity)));
  }

  pixels.show();
}


void effect_wipe2(byte intensity) {
  static uint32_t c;
  static byte s1=0, s2=0;
  static byte steps = 0;

  if (steps > 72) {
    s1 = s2 = random(143);
    s1++;
    //c = nextrainbow(random(144), intensity);
    c=Color(i2color(intensity),i2color(intensity),i2color(intensity),i2color(intensity));
    steps=0;
  }
  steps++;
  dim(intensity);
  pixels.setPixelColor(s1, c);
  pixels.setPixelColor(s2, c);
  

  s1++;
  if (s1 > 143) s1 = 0;
  if (s2 == 0) {
    s2 = 143;
  } else {
    s2--;
  }

  pixels.show();
  
}

byte i2color(byte i) {
  if (i<16) {
    return i*i;
  } else {
    return 255;
  }
}
// 18; 65; 48
void effect_split(byte intensity) {

  byte i;
  for (i =144-58; i <144; i++) {
    uint32_t c;
    c = pixels.getPixelColor(i);
    pixels.setPixelColor(i-1, c);
  }

  for (i = 60; i > 17; i--) {
    uint32_t c;
    c = pixels.getPixelColor(i);
    pixels.setPixelColor(i+1, c);
  }
  pixels.setPixelColor(18, 0);

  for (i = 0; i < 18; i++) {
    pixels.setPixelColor(i, Color(i2color(intensity),i2color(intensity),i2color(intensity),i2color(intensity)));
    
  }

  dim(intensity);
  

  uint32_t c;
  c = Color(intensity*8+random(intensity*7), 0, intensity*8+random(intensity*7), 0);

   pixels.setPixelColor(random(144), c);
   pixels.show();
  
}

void effect_chase(byte intensity) {
  //byte c = random(144);
  static byte c;
  c++;
  if (c > 144) c = 0;
  static byte l[2]={0,80};

  byte i;
dim(intensity);
  for (i = 0; i < 2; i++) {
    byte pos = l[i];
    
   pos+=1;//random(10);
   if (pos > 144) {
     pos=0;
   }
   l[i] = pos;
   //byte l = random(144);
   uint32_t col;

    col = nextrainbow(c, intensity);

  // pixels.clear();
   pixels.setPixelColor(pos, col);
  }
  
  pixels.show();
}

void dim(byte intensity) {
  byte i;

  byte cols[4];
  byte cutby = intensity/4 + 1;
  uint32_t * cp = (uint32_t*)(cols);

  for (i = 0; i < 144; i++) {
    *cp = pixels.getPixelColor(i);

    byte j;
    for (j = 0; j < 4; j++) {
      cols[j] = dec1(cols[j], cutby);
    }

    pixels.setPixelColor(i,*cp);
    
  }
}

byte dec1 (byte d, byte cutby) {
  if (d > cutby) {
    d-=cutby;
  } else {
    d = 0;
    
  }
  return d;
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
  short test = 1;
  digitalWrite(2,1);
  delay(1);
  digitalWrite(2,0);


  while (digitalRead(3) == 0 && test++);
  while (digitalRead(3) == 1) {
    delay(1);
    //digitalWrite(7, counter&1);
    counter++;
    if (counter > 5) {
      delay(3);
      return 0;
    }
  }
  delay(8-counter);
  return 1;
}
