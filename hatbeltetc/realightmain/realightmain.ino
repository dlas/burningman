/*********************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x32 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/
//#define BIKE 1
#define HAT 1

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2





byte controls[20];
#define main_mode controls[0]
#define main_speed controls[1]
#define main_brightness_mode controls[2]
#define main_brightness controls[3]
#define main_sparkle controls[4]
#define main_color_r controls[5]
#define main_color_g controls[6]
#define main_color_b controls[7]
#define main_super_dim controls[8]
#define main_audio_amp controls[9]
#define main_hall_sensor controls[10]
#define main_knob_sensor controls[11]
#define main_audio_adc controls[12]
#define main_motion controls[13]
#define main_save controls[14]

#define main_color_w controls[15]



#ifndef BIKE
#include <Adafruit_NeoPixel.h>
//byte rawpixeldata[144*4];
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(144, 2, rawpixeldata, NEO_GRBW + NEO_KHZ800);

//Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(25, 4, rawpixeldata, NEO_GRB + NEO_KHZ800);


#ifndef HAT
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(144, 2, NEO_GRBW + NEO_KHZ800);
#else
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(144, 2, NEO_GRBW + NEO_KHZ800);

#endif
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(32, 4, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(32, 6, NEO_GRB + NEO_KHZ800);

#define NUMPIXELS 144
#define NUMPIXELS2 32

uint32_t Color(byte r, byte g, byte b, byte w) {
  if (main_super_dim) {
    return pixels.Color(r>>2, g>>2, b>>2,w>>2);
  } else {
    return pixels.Color(r,g,b,w);
  }
}

uint32_t Color2(byte r, byte g, byte b, byte w) {
  if (main_super_dim) {
    return pixels.Color(r>>3, g>>2, b>>2);
  } else {
    return pixels2.Color(r,g,b);
  }
}
#endif

#ifdef BIKE
#include "Adafruit_WS2801.h"
#define NUMPIXELS 24
Adafruit_WS2801 pixels = Adafruit_WS2801(24, 2, 3);
// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b, byte w)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}
#define NUMPIXELS2 32

uint32_t Color2(byte r, byte g, byte b, byte w) {

  return 0;
  
}

class NullPixels {
  public:
  void setPixelColor(byte i, uint32_t c) {};
  void setPixelColor(byte i, byte r, byte g, byte b, byte w) {};
  void show() {}
  void begin(){}
};

NullPixels pixels2, pixels3;

#endif


#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


typedef struct  {
  byte mi_max;
  const  char * mi_name;
} MenuItem;




//const MenuItem menu[] = {};


const  MenuItem  menu[]  = {
  {
    9,
     "main mode"
  },
  
  
  {
    16,
    "speed"
  },
  {
    3,
    "brght mode"
    
  },
  {
    16,
    "brightness"
  },
  {
    16,
    "sparkle"
  },
  {
    16,
    "red"
  },

  {
    16,
    "green"
  },
  {
    16,
    "blue"
  },
  {
    1,
    "dim"
  },

  {
    16,
    "audio amp"
  },
  {
    0,
    "hall sensor"
  },
  {
    0,
    "knob sensor"
  },
  {
    0,
    "audio ADC"
  },
  {
    0,
    "motion"
  },
  {
    1,
    "SAVE"
  },
  

  {
     0, NULL
  }
};


void setup()   {
  analogReference(EXTERNAL);
  restore();
  main_save = 0;
 // Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  display.clearDisplay();
  pixels.begin();
  pixels2.begin();
  pixels2.show();
  pixels3.begin();
  blockint();

}

void save() {
  int i = 0;
  for (i = 0; menu[i].mi_name != NULL; i++) {
    EEPROM.write(i, controls[i]);
  }

}


void restore() {
  int i;
  for (i = 0; menu[i].mi_name != NULL; i++) {
    byte d;
    d = EEPROM.read(i);
    if (d <= menu[i].mi_max) {
      controls[i] = d;
    }
  }
}


#define MENUITEMS (sizeof(menu)/sizeof(MenuItem)-1)
void menu_render(int selected) {

  int top;
  int items = sizeof(menu) / sizeof(MenuItem) - 1;

  selected = selected % items;

  if (selected < 4) {
    top = 0;
  } else if (selected > items - 4) {
    top = items - 4;
  } else {
    top = selected - 1;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  for (int i = top, n = 0; i < top + 4; i++, n++) {
    if (i == selected) {
      display.setTextColor(BLACK, WHITE);
    } else {
      display.setTextColor(WHITE);
    }

    display.setCursor(0, n * 8);
    display.print(menu[i].mi_name);
    display.setCursor(88, n * 8);
    display.print(controls[i]);
   // display.print(*(menu[i].mi_data));
  }
  display.display();

}

byte last_button;
byte  did_action;
byte  display_selected;

int button_check() {
  int s = analogRead(1) / 4;
  int button = 0;
  if (s < 20) {
    button = 1;
  } else if (s > 35 && s < 60) {
    button = 2;
  } else if (s > 65 && s < 90) {
    button = 4;
  } else if ( s > 95 && s < 110) {
    button = 3;
  }

  if (button == 0) {
    did_action = 0;
    last_button = 0;
    return 0;
  }

  if (button != last_button) {
    last_button = button;
    return 0;
  }

  if (did_action == 1) {
    return 0;
  }

  did_action = 1;

  switch (button) {
    case (1): 
      display_selected++; 
      if (display_selected >= MENUITEMS) display_selected = MENUITEMS-1;
      return 1;
    case (2): 
      if (display_selected > 0) display_selected--;     
      return 1;
    case (3):
    case (4):
      if (menu[display_selected].mi_max > 0) {
        int nv;
        nv = controls[display_selected];
  //      nv = *(menu[display_selected].mi_data);
        if (button == 3) {
          nv++;
        } else {
          nv--;
        }
        if (nv < 0) {
          nv = 0;
        }
        if (nv > menu[display_selected].mi_max) {
          nv = menu[display_selected].mi_max;
        }
        controls[display_selected] = nv;
      //  *menu[display_selected].mi_data = nv;


      }
  }
  return 1;

}

#define main_audio_threshold 0
byte  sel = 0;
byte pround = 0;
byte smoothed_audio = 0;
byte move_holdoff = 0;
int last = 0;
short mag_holdoff;
byte mag_close = 0;
void loop() {
  /*
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("MAGL ");
    int x = analogRead(0);
    display.println(x);
    display.setCursor(8,8);
    display.println("Hello1!");
    display.setCursor(16,16);
    display.println("Hello2!");
    display.setCursor(24,24);
    display.println("Hello3!");
    display.display();
  */
//  boringcolor2();
  int didmove = 0;
  main_motion = digitalRead(9);
  if (main_motion != last) {
    last = main_motion;
    didmove = 1;
  }

  main_hall_sensor = analogRead(0) / 4;
  main_knob_sensor = analogRead(1) / 4;
  
  //main_audio_adc = analogRead(2) /4;

  short  raw_audio_adc = analogRead(2);
  
  main_audio_adc = raw_audio_adc / 4;

  
  if (raw_audio_adc < main_audio_threshold) {
    raw_audio_adc = 0;
  }
 // raw_audio_adc = raw_audio_adc / (1 + 5 * main_audio_amp);


  raw_audio_adc/=(1+ 8 * main_audio_amp);
  
  if (raw_audio_adc > smoothed_audio) {
    smoothed_audio++;
  } else if (raw_audio_adc < smoothed_audio){
    smoothed_audio--;
  }
  main_audio_adc = smoothed_audio;
  //main_audio_adc = raw_audio_adc / (1 + 5*main_brightness));
  if (main_hall_sensor > 100 || main_hall_sensor < 60) {
    if (mag_close ==0) {
     mag_holdoff=500;
     mag_close = 1;
    }
  } else {
    mag_close = 0;
  }
 
  sel++;
  delay(25);
  if ( button_check() || sel > 10) {
    menu_render(display_selected);
    sel = 0;
    if (main_save == 1) {
      save();
      main_save = 0;
      display.clearDisplay();
      display.setCursor(00,0);
      display.println("save");
      display.display();
      delay(250);
    }
  }

  byte sparkle_use_brightness = main_sparkle;
  byte use_brightness = main_brightness;
  
  if (main_brightness_mode == 1) {
    use_brightness = 1 + smoothed_audio;
    if (use_brightness > 10) {
      use_brightness = 10;
    }   
  }else if (main_brightness_mode == 2) {
    if (mag_holdoff > 0) {
      mag_holdoff--;
      use_brightness = main_brightness;
    } else {
      chase();
      pixels.show();
      return;        
    }
  } else if (main_brightness_mode == 3) {
    sparkle_use_brightness = smoothed_audio;
  } else {
    use_brightness = main_brightness;
  } 

  switch (main_mode) {
    case(0):
      rainbows(use_brightness, 0, sparkle_use_brightness);
      break;
    case(1):
      allsolid(use_brightness);
      break;
    case(2):
      pan1(use_brightness);
      break;
    case(3):
       chase();
       break;
    case(4):
      twinkle(use_brightness,didmove, sparkle_use_brightness);
      break;
   case(5):
    rainbows(use_brightness, 1, sparkle_use_brightness);
     break;
     case(6):
     arrow2(use_brightness, sparkle_use_brightness);
     break;
     case(7):
     circles(use_brightness, sparkle_use_brightness);
     break;
     case(8):
     wipe(use_brightness, sparkle_use_brightness);
     break;
     case(9):
     wipe2(use_brightness, sparkle_use_brightness);
     break;
  }

}

#define mwrite(x,y,c) pixels.setPixelColor((x) + ((y) * 17) , c)

typedef struct {
  int8_t x;
  int8_t y;
  byte ci;
} block;

block blocks[6];
#define LENBLOCKS 6

void blockint(void) {
  blocks[0].ci = 0;
  blocks[1].ci = 25;
  blocks[2].ci = 50;
  blocks[3].ci = 75;
  blocks[4].ci = 100;
  blocks[5].ci = 125;
}

void circles(byte use_brightness, byte sparkle_brightness) {
  static byte centerx;
  static byte centery;
  static byte color;
  static byte state;
  static byte wait = 0;

  if (wait >1) {
    wait--;
    return;
  }
  wait = main_speed;
 
  if (main_brightness_mode == 1) {
    use_brightness--;
  }

  
  fade();
  if (state > 2) {
    state = 0;
    color = RND()%144;
    centerx = RND()%18;
    centery = RND()%6+2;
    mwrite(centerx, centery, nextrainbow(color, use_brightness));
  } else {
    state++;
    byte y;
    int8_t i;
    uint32_t c = nextrainbow(color, use_brightness);
    for (i = -state; i <=state; i++) {
      
      mwrite((centerx +i)%18, centery+state, c);
      mwrite((centerx+i)%18, centery-state, c);
      mwrite((centerx+state)%18, centery+i, c);
      mwrite((centerx-state)%18, centery+i, c);
    }
  }

  pixels.show();

  

}
void wipe2(byte use_brightness, byte sparkle_brightness) {
  static byte mode;
  static byte state;
  static byte color;
  static byte holdoff;

  if (holdoff <=main_speed) {
    holdoff++;
    return;
  }

  holdoff = 0;

  fade();
  if (state == 0) {
    color = RND()%144;
    mode = RND()&3;
  }
  //color++;
  if (color > 143) {
    color = 0;
  }
  state++;
  
  if (mode&1) {
    byte rowcol;
    byte i;
    if (mode&2) {
      rowcol = 17-state;  
    } else {
      rowcol = state;
    }
    for (i = 0; i < 8; i++) {
      //color++;
      //if (color > 143) {
      //  color = 0;
     // }
      mwrite(rowcol, i, solid(color, use_brightness));
    }
    if (state >=17) {
      state = 0;
    }
  } else {
    byte rowcol;
    byte i;
    if (mode &2) {
      rowcol = 8 - state;
    } else {
      rowcol = state;
    }
    for (i = 0; i < 17; i++) {
     // color++;
     // if (color > 143) {
     //   color = 0;
     // }
      mwrite(i, rowcol, solid(color, use_brightness));
    }
    if (state >8) {
      state = 0;
    }
    
  }
  pixels.show();
}

void wipe(byte use_brightness, byte sparkle_brightness) {
  static byte mode;
  static byte state;
  static byte color;

  fade();
  if (state == 0) {
    color = RND()%144;
    mode = RND()&3;
  }
  //color++;
  if (color > 143) {
    color = 0;
  }
  state++;
  
  if (mode&1) {
    byte rowcol;
    byte i;
    if (mode&2) {
      rowcol = 17-state;  
    } else {
      rowcol = state;
    }
    for (i = 0; i < 8; i++) {
      //color++;
      //if (color > 143) {
      //  color = 0;
     // }
      mwrite(rowcol, i, nextrainbow(color, use_brightness));
    }
    if (state >=17) {
      state = 0;
    }
  } else {
    byte rowcol;
    byte i;
    if (mode &2) {
      rowcol = 8 - state;
    } else {
      rowcol = state;
    }
    for (i = 0; i < 17; i++) {
     // color++;
     // if (color > 143) {
     //   color = 0;
     // }
      mwrite(i, rowcol, nextrainbow(color, use_brightness));
    }
    if (state >8) {
      state = 0;
    }
    
  }
  pixels.show();
}

void fade(void) {
  byte i;
for (i = 0; i < NUMPIXELS; i++) {
    uint32_t p = pixels.getPixelColor(i);
    byte r,g,b;
    r = p&0xFF;
    b = (p&0xFF00) >>8;
    g = (p&0xFF0000) >> 16;
    if (r > 0) r--;
    if (b > 0) b--;
    if (g > 0) g--;
    uint32_t c2;
    c2 = (uint32_t)r | (uint32_t)b<<8 | (uint32_t)g<<16;
    pixels.setPixelColor(i, c2);
  }
  
  
}
void arrow2(byte use_brightness, byte sparkle_brightness) {
  static byte st;
  byte i;  
  byte u = use_brightness*15;
  uint32_t c = Color(u,u,u,u);

  if (st < main_speed) {
    st++;
    return;
  }

  st = 0;
  fade();

  for (i = 0; i < LENBLOCKS; i++) {
    byte dir = RND()&3;
    block * b = blocks+i;
    if (b->ci != 255)   mwrite(b->x,b->y, nextrainbow(b->ci, use_brightness));
     
    switch (dir){
      case(0):
        b->x++;
      break;
      case(1):
        b->x--;
        break;
      case(2):
        b->y--;
        break;
      case(3):
        b->y++;
        break;
    }
    if (b->x > 16) b->x = 16;
    if (b->x < 0) b->x = 0;
    if (b->y > 8) b->y = 8;
    if (b->y < 0) b->y = 0;
    if (b->ci == 255) {
      mwrite(b->x, b->y, 0); 
    } else {
     b->ci++;
     if (b->ci > 143) {
       b->ci = 0;
     }
     mwrite(b->x,b->y, Color(c,c,c,c));
    }
  }
  pixels.show();
}

byte snow[10];
void arrow(byte use_brightness, byte sparke_brightness) {
  static byte pos;

  pos++;
  if (pos > 32) {
    pos = 0;
  }
  byte p = pos>>1;
  byte u = use_brightness>>1;
  byte i;
  for (i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, Color(u,0,0,0));
  }
  
  for (i = 0; i < 10; i++) {
    snow[i] += 17 +(RND()&1);
    if (snow[i] >= NUMPIXELS) {
      snow[i]=RND();
    }

    pixels.setPixelColor(snow[i], Color(use_brightness, use_brightness, use_brightness, 0));
    /*
    //mwrite(i+p, i, Color(use_brightness,0,0,0));
    mwrite(8-i+p, i, Color(0,0,use_brightness,0));
    mwrite(8-i+p+1, i, Color(use_brightness, u, use_brightness,0));
    mwrite(8-i+p+2, i, Color(use_brightness, use_brightness, use_brightness, 0));
    mwrite(8-i+p+3, i, Color(use_brightness, use_brightness, use_brightness, 0));
    mwrite(8-i+p+4, i, Color(use_brightness, u, use_brightness, 0));
    mwrite(8-i+p+5, i, Color(use_brightness, u, u, 0));
    mwrite(8-i+p+6, i, Color(use_brightness, 0,0, 0));
    mwrite(8-i+p+7, i, Color(u,0,0, 0));
    mwrite(8+i-p, i, Color(0,0,use_brightness,0));
    mwrite(8+i-p+1, i, Color(u, use_brightness, u,0));
    mwrite(8+i-p+2, i, Color(u, use_brightness, u, 0));
    mwrite(8+i-p+3, i, Color(use_brightness, use_brightness, use_brightness, 0));
    mwrite(8+i-p+4, i, Color(use_brightness, use_brightness, u, 0));
    mwrite(8+i-p+5, i, Color(use_brightness, use_brightness, 0, 0));
    mwrite(8+i-p+6, i, Color(use_brightness, u,0, 0));
    mwrite(8+i-p+7, i, Color(use_brightness,u,0, 0));
   // mwrite(p, i, Color(0,use_brightness,0,0));
  //  mwrite(8-i-p+4, i, Color(use_brightness,  use_brightness,u,0));
  //  mwrite(8-i-p+5, i, Color(use_brightness, use_brightness, use_brightness, 0));
    */
  }
  
  pixels.show();

  
}
void rainbows(byte use_brightness, byte scrambles, byte sparkle_brightness) {
  static byte rainbow_offset;
  static byte rainbow_offset2;
  byte i;

  for (i = 0; i < NUMPIXELS; i++) {
    int p = i +rainbow_offset;
    if (p >= NUMPIXELS) p -=NUMPIXELS;

    if (scrambles) {
      uint32_t c = nextrainbow(i, use_brightness);
      byte s;
      s = i % 4;
      c = (c>>(s*8)) | (c<<(32-s*8));
      pixels.setPixelColor(NUMPIXELS-p-1, c);
    } else {
      pixels.setPixelColor(NUMPIXELS-p-1, nextrainbow(i, use_brightness));
    }
  }

  twinkp1(sparkle_brightness, rainbow_offset);
  pixels.show();

  for (i = 0; i < NUMPIXELS2; i++) {
    int p = i + rainbow_offset2;
    if (p >= NUMPIXELS2) p-=NUMPIXELS2;
    pixels2.setPixelColor(NUMPIXELS2-p-1, nextrainbow2(i, use_brightness));
  }
  twinkp2(sparkle_brightness, rainbow_offset2);
  pixels2.show();
  pixels3.show();

  rainbow_offset+=main_speed;
  if (rainbow_offset >= NUMPIXELS) {
    rainbow_offset=0;
  }

  rainbow_offset2+=main_speed;
  if (rainbow_offset2 >= NUMPIXELS2) {
    rainbow_offset2=0;
  }
}

void allsolid(byte use_brightness) {
  byte i;
  for (i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, solid(i, use_brightness));
  }
  pixels.show();

  for ( i = 0; i < NUMPIXELS2; i++) {
    pixels2.setPixelColor(i, solid(i, use_brightness));
  }
  pixels2.show();
  pixels3.show();
  
}

void twinkp1(byte brightness, byte offset) {
  byte b = (brightness) * 15;
  if (b) {
  for (byte k = 1; k < 3; k++) {
      byte p;
      p = (17* offset * RND() *5 *k) % NUMPIXELS;
      pixels.setPixelColor(p, Color(b,b,b,b));
  
  }
  }

}

void twinkp2(byte brightness, byte offset) {
    byte b = (brightness) * 15;

    if (b) {
  for (byte k = 1; k < 2; k++) {
      byte p;
      p = (23* offset * RND()*5 *k) % NUMPIXELS2;
      pixels3.setPixelColor(p, Color2(b,b,b,b));
  }
  }
}


void twinkle(byte use_brightness, byte didmove, byte sparkle_brightness) {

  static short pos;
  static byte skip;
  static byte r1, g1, b1=15, r2, g2=15, b2;
  byte bc = (use_brightness);
  byte i,k;

  skip+=main_speed;
  if (skip > 15) {
    skip = 0;
  } else {
    return;
  }


  if (r1 < r2)  r1++;
  if (r1 > r2) r1--;
  if (g1 < g2)  g1++;
  if (g1 > g2) g1--;
  if (b1 < b2)  b1++;
  if (b1 > b2) b1--;

  
  uint32_t c = Color(r1 * bc, g1 * bc, b1  *bc, 0);

  for (i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, c);  
  }

  byte b = (sparkle_brightness) * 15;
  if (b) {
  for (k = 1; k < 3; k++) {
      byte p;
      p = (17* pos * RND()*5 *k) % NUMPIXELS;
      pixels.setPixelColor(p, Color(b,b,b,b));
  
  }
  }
  
  pixels.show();
  c = Color2(g1 * bc, b1 * bc, r1 * bc, 0);

  for (i = 0; i < NUMPIXELS2; i++) {
    pixels2.setPixelColor(i, c);
  }

  if (b) {
  for (k = 1; k < 2; k++) {
      byte p;
      p = (11* pos * RND()*5 *k) % NUMPIXELS2;
      pixels2.setPixelColor(p, Color2(b,b,b,b));
  }
  }
  
  pixels2.show();

    c = Color2(b1 * bc, r1 * bc, g1 * bc, 0);

  for (i = 0; i < NUMPIXELS2; i++) {
    pixels3.setPixelColor(i, c);
  }

  if (b) {
  for (k = 1; k < 2; k++) {
      byte p;
      p = (23* pos * RND()*5 *k) % NUMPIXELS2;
      pixels3.setPixelColor(p, Color2(b,b,b,b));
  }
  }
  pixels3.show();
  pos++;
  if (pos > 64) {
    r1=r2;
    g1=g2;
    b1=b2;
    r2=(RND()&1)*15;
    g2=(RND()&1)*15;
    b2=(RND()&1)*15;
    pos = 0;
  }
}
void pan1(byte use_brightness) {

  static byte panmode;
  static byte panprogress;
  static byte r1=15,g1=15,b1=0,r2=15,g2=0,b2=15;
  static byte skip;

  uint32_t c1,c2;
  byte i;

  skip +=main_speed;
  if (skip > 15) {
    skip = 0;
  } else {
    return;
  }
  c1 = Color(r1 * use_brightness, g1 * use_brightness, b1 * use_brightness, 0);
  c2 = Color(r2 * use_brightness, g2 * use_brightness, b2 * use_brightness,0 );
  for (i = 0; i < NUMPIXELS; i++) {
     if (panmode > 0) {
      pixels.setPixelColor(i, c2);
     } else {
       if ( i < panprogress) {
        pixels.setPixelColor(i, c2);
       } else {
        pixels.setPixelColor(i, c1);
       }
     }
  }
  pixels.show();
  c1 = Color2(r1 * use_brightness, g1 * use_brightness, b1 * use_brightness,0);
  c2 = Color2(r2 * use_brightness, g2 * use_brightness, b2 * use_brightness,0);
  for (i = 0; i < NUMPIXELS2; i++) {
    if (panmode == 0) {
      pixels2.setPixelColor(i, c1);
      
    } else if (panmode == 1) {
       if (i < panprogress) {
         pixels2.setPixelColor(i, c2);
       } else {
          pixels2.setPixelColor(i, c1);
       }
    } else  {
      pixels2.setPixelColor(i, c2);
    }
  }
  pixels2.show();
  
  for (i = 0; i < NUMPIXELS2; i++) {
    if (panmode == 2 && i < panprogress) {
      pixels3.setPixelColor(i, c2);
    } else {
      pixels3.setPixelColor(i, c1);
    }
  }
  pixels3.show();

  panprogress++;

  if (panmode == 0 && (panprogress >= NUMPIXELS)) {
    panmode++;
    panprogress = 0;
  } else if (panmode >0 && panprogress >= NUMPIXELS2) {
    panprogress = 0;
    panmode++;
  }
  if (panmode > 2) {
    panmode = 0;
    panprogress = 0;
    r1=r2;
    g1=g2;
    b1=b2;
    r2=(RND()&1)*15;
    g2=(RND()&1)*15;
    b2=(RND()&1)*15;
  }
  
}

byte RND(void) {
  static short r1=101, r2=257;
  r1=(r1*r2);
  r2++;
  r1++;
  return (r1%7917)&255;
}

void reactive(void){
}


  /*
  if (main_mode == 3) {
    chase();
  } else {

   
  for (int i = 0; i < NUMPIXELS; i++) {

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    //pixels.setPixelColor(i, pixels.Color(0,0,0,150)); // Moderately bright green color.
    int p = i + pround;
    if (p >= NUMPIXELS) {
      p -= NUMPIXELS;
    }
    if (main_mode == 0) {
      pixels.setPixelColor(p, nextrainbow(i, use_brightness));
    }

    if (main_mode == 1) {
      pixels.setPixelColor(p, solid(i, use_brightness));
    } 
    
    if (main_mode ==2 ) {
      if (didmove) {

        move_holdoff = 16;
      }
      if (move_holdoff >0 ) {
        move_holdoff--;
        
      }
      pixels.setPixelColor(p, solid(i, move_holdoff)); 
    }
  }
  }
  pixels.show(); // This sends the updated pixel color to the hardware.

  pround += main_speed;
  if (pround > NUMPIXELS) {
    pround = 0;
  }
  */

void boringcolor2(void) {
  #ifndef BIKE
    int i;
    for (i = 0; i < 25; i++) {
      pixels2.setPixelColor(i, main_color_r,main_color_g,main_color_b);
    }
    pixels2.show();
    pixels3.show();
    #endif
}

void chase(void) {
  
  static int chaseround = 0;
  static int dochase = 0;

  if (dochase == 0) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i,0);
  }

  chaseround++;
  if (chaseround >= NUMPIXELS) {
    chaseround = 0;
  }
  pixels.setPixelColor(chaseround, nextrainbow(chaseround, 8));
  dochase=4;
  } else {
    dochase--;
  }
  pixels.show();
  
}
void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }
  display.display();
  delay(1);
}


void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
  delay(1);

  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}



uint32_t solid(int i, unsigned int scale) {

  int r, b, g, w;
  r = main_color_r;
  b = main_color_b;
  g = main_color_g;
  w = main_color_w;
  r*=scale;
  g*=scale;
  b*=scale;
  w*=scale;
  if (r > 255) r = 255;
  if (g>255) g = 255;
  if (b>255) b=255;
  if (w>255) w=255;

  return Color(r,g,b,w);
}


uint32_t nextrainbow(int i, unsigned int scale) {
  int mode = i / (NUMPIXELS/6);

  if (scale > 16) {
    scale = 16;
  }

  unsigned int level = ((i % 24) * 2) / 3 + 1;
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
uint32_t nextrainbow2(int i, unsigned int scale) {
  int mode = i / (NUMPIXELS2/6);

  if (scale > 16) {
    scale = 16;
  }

  unsigned int level = (i % (5)) * 4;
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

    case (0): return Color2(big, levelH, 0, 0);
    case (1): return Color2(levelL, big, 0, 0);
    case (2): return Color2(0, big, levelH, 0);
    case (3): return Color2(0, levelL, big, 0);
    case (4): return Color2(levelH, 0, big, 0);
    case (5): return Color2(big, 0, levelL, 0);
    default: return Color2(big, levelH,0,0);
  }

}
