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
//#define HAT 1

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET 3
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
#define main_hall_sensor controls[10]
#define main_knob_sensor controls[11]
#define main_audio_adc controls[12]
#define main_motion controls[13]
#define main_save controls[14]

#define main_color_w controls[9]



#include <Adafruit_NeoPixel.h>
//byte rawpixeldata[144*4];
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(144, 2, rawpixeldata, NEO_GRBW + NEO_KHZ800);

//Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(25, 4, rawpixeldata, NEO_GRB + NEO_KHZ800);


#define NUMPIXELS_SQUARE (300)
#define NUMPIXELS_CIRCLE 230
#define NUMPIXELS_CROSS1 150
#define NUMPIXELS_CROSS2 150

#define OFFSET_SQUARE 0
#define OFFSET_CIRCLE 20
#define OFFSET_CROSS1 0
#define OFFSET_CROSS2 0

Adafruit_NeoPixel pixels_cross2 = Adafruit_NeoPixel(NUMPIXELS_CROSS2 + OFFSET_CROSS2, 12, NEO_GRBW + NEO_KHZ800);

Adafruit_NeoPixel pixels_square = Adafruit_NeoPixel(NUMPIXELS_SQUARE + OFFSET_SQUARE, 8, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel pixels_circle = Adafruit_NeoPixel(NUMPIXELS_CIRCLE + OFFSET_CIRCLE, 10, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel pixels_cross1 = Adafruit_NeoPixel(NUMPIXELS_CROSS1 + OFFSET_CROSS1, 11, NEO_GRBW + NEO_KHZ800);


uint32_t Color(byte r, byte g, byte b, byte w) {
  if (main_super_dim) {
    return pixels_square.Color(r>>2, g>>2, b>>2,w>>2);
  } else {
    return pixels_square.Color(r,g,b,w);
  }
}


class NullPixels {
  public:
  void setPixelColor(byte i, uint32_t c) {};
  void setPixelColor(byte i, byte r, byte g, byte b, byte w) {};
  void show() {}
  void begin(){}
};



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
    3,
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
    "white"
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
 // analogReference(EXTERNAL);
 pinMode(4, INPUT_PULLUP);
 pinMode(5, INPUT_PULLUP);
 pinMode(6, INPUT_PULLUP);
 pinMode(7, INPUT_PULLUP);
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
  display.print("0x"); display.println(0xDEBF, HEX);
  display.display();
  display.clearDisplay();

  pixels_square.begin();
  pixels_circle.begin();
  pixels_cross1.begin();
  pixels_cross2.begin();
  pixels_square.clear();

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

  

  int button = 0;

  if (digitalRead(4) == 0) {
    button =1;
  } else if (digitalRead(5) ==0) {
    button =2;
  } else if (digitalRead(6) == 0) {
    button = 3;
  } else if (digitalRead(7) == 0) {
    button = 4;
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

byte sel;
void loop() {
  sel++;
  delay(1);
//  rainbows(2,0,0);
//  display.clearDisplay();
//  display.setCursor(0,0);
//  display.println(sel);
//  display.display();
  
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
   byte use_mode;
   main_brightness = (analogRead(0)+63)/64;
   main_speed =(analogRead(2)+63)/64;
   if (main_mode == 0) {
    use_mode = 1+(analogRead(1)+127)/128;
   } else {
    use_mode = main_mode;
   }
  switch (use_mode) {
    case(2):
       chase(main_brightness);
       break;
    case(3):
      rainbows(main_brightness, 0, 0);
      break;
    case(4):
       allsolid(main_brightness);
      break;
    case(1):
      manychase(main_brightness);
      break;
   
  }

}

void rainbows(byte use_brightness, byte scrambles, byte sparkle_brightness) {
  static short rainbow_offset;
  static byte rainbowcirc_offset;
  static byte rainbowcross_offset;
  
  short i;

  for (i = 0; i < NUMPIXELS_SQUARE; i++) {
    int p = i +rainbow_offset;
    if (p >= NUMPIXELS_SQUARE) p -=NUMPIXELS_SQUARE;
    pixels_square.setPixelColor(NUMPIXELS_SQUARE-p-1, nextrainbow(NUMPIXELS_SQUARE/6, i, use_brightness));
    
  }
  pixels_square.show();
  rainbow_offset+=main_speed;
  if (rainbow_offset >= NUMPIXELS_SQUARE) {
    rainbow_offset=0;
  }




  for (i = 0; i < NUMPIXELS_CIRCLE; i++) {
    int p = i +rainbowcirc_offset;
    if (p >= NUMPIXELS_CIRCLE) p -=NUMPIXELS_CIRCLE;
    pixels_circle.setPixelColor(NUMPIXELS_CIRCLE-p-1, nextrainbow(NUMPIXELS_CIRCLE/6, i, use_brightness));
    
  }
  pixels_circle.show();
  rainbowcirc_offset+=main_speed;
  if (rainbowcirc_offset >= NUMPIXELS_CIRCLE) {
    rainbowcirc_offset=0;
  }




  for (i = 0; i < NUMPIXELS_CROSS1; i++) {
    int p = i +rainbowcross_offset;
    if (p >= NUMPIXELS_CROSS1) p -=NUMPIXELS_CROSS1;
    pixels_cross1.setPixelColor(NUMPIXELS_CROSS1-p-1, nextrainbow(NUMPIXELS_CROSS1/6, i, use_brightness));
    
  }

    
  for (i = 0; i < NUMPIXELS_CROSS2; i++) {
    int p = i +rainbowcross_offset;
    if (p >= NUMPIXELS_CROSS1) p -=NUMPIXELS_CROSS2;
    pixels_cross2.setPixelColor(NUMPIXELS_CROSS1-p-1, nextrainbow(NUMPIXELS_CROSS1/6, i, use_brightness));
    
  }
  pixels_cross2.show();
  pixels_cross1.show();
  rainbowcross_offset+=main_speed;
  if (rainbowcross_offset >= NUMPIXELS_CROSS1) {
    rainbowcross_offset=0;
  }


}

uint32_t segment_getpixel(byte seg, short off) {
  switch (seg) {
    case(1):
      return pixels_square.getPixelColor(off);
      break;
    case(2): 
      return pixels_square.getPixelColor(off+38);
      break;
    case(3):
      return  pixels_square.getPixelColor(off+75);
       break;
    case(4):
      return  pixels_square.getPixelColor(off+113);
       break;
    case(5):
     return  pixels_square.getPixelColor(off+150);
      break;
      case(6):
      return pixels_square.getPixelColor(off+188);
      break;
    case(7):
      return pixels_square.getPixelColor(off+225);
      break;
    case(8):
      return  pixels_square.getPixelColor(off+263);
       break;
    case(9):
     return pixels_circle.getPixelColor(off + OFFSET_CIRCLE);
      break;
    case(10):
      return pixels_circle.getPixelColor(off+57 + OFFSET_CIRCLE);
      break;
    case(11): 
      return pixels_circle.getPixelColor(off+115 + OFFSET_CIRCLE);
      break;
    case(12):
      return pixels_circle.getPixelColor(off+173 + OFFSET_CIRCLE);
      break;
    case(13):
      return pixels_cross1.getPixelColor(off);
      break;
    case(14):
      return pixels_cross1.getPixelColor(off+75);
      break;
    case(15):
      return pixels_cross2.getPixelColor(off);
      break;
    case(16):
      return pixels_cross2.getPixelColor(off+75);
      break;
  }
}
void segment_setpixel(byte seg, short off, uint32_t col) {
  switch (seg) {
    case(1):
      pixels_square.setPixelColor(off, col);
      break;
    case(2): 
      pixels_square.setPixelColor(off+38, col);
      break;
    case(3):
       pixels_square.setPixelColor(off+75, col);
       break;
    case(4):
       pixels_square.setPixelColor(off+113, col);
       break;
    case(5):
      pixels_square.setPixelColor(off+150, col);
      break;
      case(6):
      pixels_square.setPixelColor(off+188, col);
      break;
    case(7):
      pixels_square.setPixelColor(off+225, col);
      break;
    case(8):
       pixels_square.setPixelColor(off+263, col);
       break;
    case(9):
      pixels_circle.setPixelColor(off + OFFSET_CIRCLE, col);
      break;
    case(10):
      pixels_circle.setPixelColor(off+57 + OFFSET_CIRCLE, col);
      break;
    case(11): 
      pixels_circle.setPixelColor(off+115 + OFFSET_CIRCLE, col);
      break;
    case(12):
      pixels_circle.setPixelColor(off+173 + OFFSET_CIRCLE, col);
      break;
    case(13):
      pixels_cross1.setPixelColor(off, col);
      break;
    case(14):
      pixels_cross1.setPixelColor(off+75, col);
      break;
    case(15):
      pixels_cross2.setPixelColor(off, col);
      break;
    case(16):
      pixels_cross2.setPixelColor(off+75, col);
      break;
  }
}
/* 4 [4] 3 [3] 2
 *[5]         [2]
 * 5     9     1  
 *[6]         [1]
 * 6 [7] 7 [8] 8 
 * 
 * 
 */  
byte segment_lengths[] = {0, 38,37,38,37,38,37,38,37,57,57,57,57,74,74,74,74};
int8_t vertex_to_edges[][4] = {
  {0,0,0,0},
  {-9, 10, -1, 2},
  {-2,3,-14,0},
  {-3,4,-10,11},
  {-4,5,-16,0},
  {-5,6,-11,12},
  {-6,7,13, 0},
  {-7,8,-12,9},
  {-8, 1, 15, 0},
  {16, -13,14,-15} 
  
};


byte edges_to_vertex[][2] = {
  {0,0},
  {8, 1},
  {1, 2},
  {2, 3},
  {3, 4},
  {4, 5},
  {5, 6},
  {6, 7},
  {7, 8},
  {7, 1},
  {1, 3},
  {3, 5},
  {5, 7},
  {6, 9},
  {9, 2},
  {8, 9},
  {9, 4} 
  
};

byte tried[10];

typedef struct chase_state {
  byte dir;
  byte section;
  byte off;
  byte idx;
  uint32_t fixcolor;
  
} chase_state;

chase_state CHASESTATE[5];
#define N_CHASE 5

void manychase(byte brightness) {
  byte i;

  byte activecount = 0;
  for (i = 0; i < N_CHASE; i++) {
    if (CHASESTATE[i].section) {
      activecount++;
    }
  }
  for (i = 0; i < N_CHASE; i++) {
    chase_state * cp = CHASESTATE+i;
    if(random(1000) == 1 || activecount == 0) {
        cp->section=(random(12)+1);
        cp->dir=random(2);
        cp->idx = random(144);
        cp->off = 0;
        if (random(10)==1) {
          cp->fixcolor = 0;
        } else {
          cp->fixcolor=nextrainbow(24,random(144), brightness); 
          
        }
        activecount++;

      
    } else {
      chase1(cp);

    }

  }
  decimate(pixels_square.getPixels(), NUMPIXELS_SQUARE);
  decimate(pixels_circle.getPixels(), NUMPIXELS_CIRCLE);
  decimate(pixels_cross1.getPixels(), NUMPIXELS_CROSS1);
  decimate(pixels_cross2.getPixels(), NUMPIXELS_CROSS2);


  
  pixels_square.show();
  pixels_circle.show();
  pixels_cross1.show();
  pixels_cross2.show();
  
};

void dddd(void){}


void chase1(struct chase_state  * cp) {

int i;
  uint32_t c;
  for (i = 0; i < main_speed; i++) {

// r++;
 cp->idx ++;
 if (cp->idx >=144) {
 cp->idx = 0;
}
if (cp->fixcolor ==0 ) {
 c= nextrainbow(24, cp->idx, main_brightness);
} else {
  c = cp->fixcolor;
}
 if (cp->dir) {
  cp->off+=1; 
 } else {
  cp->off-=1;
 }

 if ( cp->off < 0 || cp->off >= segment_lengths[cp->section]) {
  byte newv;
  newv = edges_to_vertex[cp->section][cp->dir];

  short news;
  news = vertex_to_edges[newv][random(4)];
  if (news == 0) {
    news=vertex_to_edges[newv][0];
  }
  
  if (news < 0) {
    cp->dir = 0;
    cp->section = -news;
    cp->off = segment_lengths[cp->section];
  } else {
    cp->dir = 1;
    cp->off = 0;
    cp->section = news;
  }
 }
uint32_t oldpix = segment_getpixel(cp->section, cp->off);

 segment_setpixel(cp->section, cp->off, addcolor(oldpix,c));
  }



  
}


void chase(byte brightness ) {
 static byte dir=1;
 static byte section = 1;
 static short off=0;
 static uint32_t c;
 static byte r = 0;
 static byte r2=1;
 static byte idx;
int i;
  
  for (i = 0; i < main_speed; i++) {

// r++;
 idx ++;
 if (idx >=144) {
 idx = 0;
}
 c= nextrainbow(24, idx, brightness);
 
  r2++;
 if (dir) {
  off+=1; 
 } else {
  off-=1;
 }

 if ( off < 0 || off >= segment_lengths[section]) {
  byte newv;
  newv = edges_to_vertex[section][dir];
  r+=r2+1;
  short news;
  news = vertex_to_edges[newv][random(4)];
  if (news == 0) {
    news=vertex_to_edges[newv][0];
  }
  
  if (news < 0) {
    dir = 0;
    section = -news;
    off = segment_lengths[section];
  } else {
    dir = 1;
    off = 0;
    section = news;
  }
 }

 segment_setpixel(section, off, c);
  }
  decimate(pixels_square.getPixels(), NUMPIXELS_SQUARE);
  decimate(pixels_circle.getPixels(), NUMPIXELS_CIRCLE);
  decimate(pixels_cross1.getPixels(), NUMPIXELS_CROSS1);
  decimate(pixels_cross2.getPixels(), NUMPIXELS_CROSS2);


  
  pixels_square.show();
  pixels_circle.show();
  pixels_cross1.show();
  pixels_cross2.show();


  
}

void decimate(byte * t, short n) {
  short rn = n*4;
  for (short i = 0; i < rn; i++) {
    if (t[i] > 0) {
      if (t[i] < 4) {
        t[i] = 0;
      } else {
       t[i]-=4;
      }
    }
  }
}

void allsolid(unsigned int scale) {
  uint32_t c = solid(0, scale);
  int i;
  for (i = 0; i < NUMPIXELS_SQUARE; i++) {
    pixels_square.setPixelColor(i, c);
  }

  for (i = 0; i < NUMPIXELS_CIRCLE; i++) {
    pixels_circle.setPixelColor(i, c);
  }

  for (i = 0; i < NUMPIXELS_CROSS1; i++) {
    pixels_cross1.setPixelColor(i, c);
    pixels_cross2.setPixelColor(i, c);
  }

  pixels_square.show();
  pixels_circle.show();
  pixels_cross1.show();
  pixels_cross2.show();
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


uint32_t nextrainbow(int segment, int i, unsigned int scale) {
  int mode = i / (segment);

  if (scale > 16) {
    scale = 16;
  }

  unsigned int level = ((i % (segment)) * 16) / segment + 1;
  //level/=2;
  unsigned int big = scale*16;

  if (big==256) {
    big--;
  }
  level = level * level;
  if (level > 255) {
    level=255;
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


byte satadd(byte a, byte b) {
  short r = (short)a+(short)b;
  if ( r > 255) {
    return 255;
  } else {
    return (byte)r;
  }
}


uint32_t addcolor(uint32_t a, uint32_t b) {

 byte * ap = (byte*)&a;
 byte * bp = (byte*)&b;

 uint32_t r;

 byte * rp = (byte*)&r;

 byte i;

 for (i = 0; i < 4; i++) {
  rp[i] = satadd(ap[i], bp[i]);
 }

 return r;
  
}
