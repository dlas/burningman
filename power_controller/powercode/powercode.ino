

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define OLED_RESET 19
Adafruit_SSD1306 display(OLED_RESET);

byte current_sense_mapping[]={7,6,5,4,3,2,1,0};
byte switch_down_mapping[]={30, 23,25, 27, 29, 36, 34, 32};
byte switch_up_mapping[]={31, 22, 24, 26, 28, 37, 35, 33};
byte enable_pwm = 1;
volatile byte tod_second;
volatile byte tod_minute;
volatile byte tod_hour;
volatile short tod_ms;
volatile byte tod_overflow;
volatile byte need_update;

typedef struct  {

  byte values[8];
} offon;

 offon tod_off_on_mappings[4];

#define MODE_SWITCH_BUTTON 42
#define PWM_CONTROLLER_ADDRESS 66


void save_data_to_eeprom() {
  byte * bp = (byte*)tod_off_on_mappings;
  short l = sizeof(tod_off_on_mappings);
  short i;

  for (i = 0; i < l; i++) {
    EEPROM.write(i, bp[i]); 
  }

}

void restore_data_from_eeprom() {
  byte * bp = (byte*)tod_off_on_mappings;
  short l = sizeof(tod_off_on_mappings);
  short i;

  for (i = 0; i < l; i++) {
    bp[i] = EEPROM.read(i);
    
  }
}
void tod_inc() {
  tod_second++;
  if (tod_second >=60) {
    tod_second = 0;
    tod_minute++;
  }
 //tod_minute++;
  if (tod_minute>=60) {
    tod_minute = 0;
    tod_hour ++;

    if ((tod_hour+4)%6 == 0) {
      update_universe();
    }
  }

  if (tod_hour >=24) {
    tod_hour = 0;
  }
  
}

void update_universe() {
  need_update = 1;
  
}

ISR(TIMER1_COMPA_vect) {
 // tod_inc();
  tod_ms+=500;
  if (tod_ms >=1000) {
    tod_inc();
    tod_ms=0;
  }

/*
  tod_overflow+=3;
  if (tod_overflow > 125) {
    tod_overflow-=125;
    tod_ms++;
    if (tod_ms >= 1000) {
      tod_inc();
      tod_ms = 0;
    }
  }
  
  */
}

void setup()   {   


               
  TCCR1A = 0;

  TCCR1B = 0;

  TCNT1  = 0;


  OCR1A = 31250;            // compare match register 16MHz/256/2Hz

  TCCR1B |= (1 << WGM12);   // CTC mode

  TCCR1B |= (1 << CS12);    // 256 prescaler 

  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  
  //OCR0A = 0xAF;
  //TIMSK0 |= _BV(OCIE0A);
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  //display.display();
  byte i;
  pca_setup();
  for (i = 0; i < 8; i++) {
    pinMode(switch_up_mapping[i], INPUT_PULLUP);
    pinMode(switch_down_mapping[i], INPUT_PULLUP);
  }

  pinMode(15, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(14, OUTPUT);
  digitalWrite(14,0);
  delay(1);
  enable_pwm = digitalRead(15);
  restore_data_from_eeprom();

}


float currents[8];
byte outputs[8];
byte last_outputs[8];
byte last_switch_state[8];

void read_switches() {
  byte up, down;

  byte i;
  for (i = 0; i < 8; i++) {
    up=digitalRead(switch_up_mapping[i]);
    down=digitalRead(switch_down_mapping[i]);
    do_switch(i, !up, !down);
  }

    
}


byte kind_of_square(byte x) {
  if (x < 16) {
    return x*x;  
  } else {
    return 255;
  }
}
void inc(byte *p) {
  if (*p<17) (*p)++;
}

void dec(byte *p) {
  if (*p>0) (*p)--;
}

void do_switch(byte sw, byte up, byte down) {
  if (enable_pwm) {
   if (up) inc(outputs+sw);
   if (down) dec(outputs+sw);
  } else {
   if (up) outputs[sw] = 16;
   if (down) outputs[sw] = 0; 
  }
  /*
  if (last_switch_state[sw]== 0 && up) {
    last_switch_state[sw] = 1;
    inc(outputs+sw);
  } else if (last_switch_state[sw] == 0 && down) {
    last_switch_state[sw] = 2;
    dec(outputs+sw);
  }
  if (last_switch_state > 0 && !up &&!down) {
    last_switch_state[sw] = 0;
  }
*/
  
}

#define CURRENT_FACTOR_SN1 0.012
#define CURRENT_FACTOR_SN2 0.0093
void read_currents() {
  int i;
  for (i = 0; i < 8; i++) {
    int t = analogRead(current_sense_mapping[i]);
    currents[i] = ((float)(t))*(CURRENT_FACTOR_SN2);
  }

}

void do_display() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  byte i;

  for (i = 0; i < 4; i++) {
    display.setCursor(i*32, 0);
    display.print(currents[i]);
    display.setCursor(i*32, 24);
    display.print(currents[i+4]);
    display.setCursor(i*32, 8);
    display.print(outputs[i]);
    display.setCursor(i*32, 16);
    display.print(outputs[i+4]);
  }
  display.display();
}

void switches_update_tod() {
  if (!digitalRead(switch_up_mapping[0])) {
    tod_hour++;
  }

  if (!digitalRead(switch_down_mapping[0])) {
    tod_hour--;
  }

  if (!digitalRead(switch_up_mapping[1])) {
    tod_minute++;
  }

  if (!digitalRead(switch_down_mapping[1])) {
    tod_minute--;
  }

  //tod_second = 0;
  //tod_inc();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(24, 4);
  display.print(tod_hour);
  display.print(":");
  display.print(tod_minute);
  display.print(":");
  display.print(tod_second);
  display.setTextSize(1);
  display.setCursor(0,24);
  display.print(tod_ms);
  display.display();  
}

char * times[] = {"2", "8", "14", "20"};
 
void do_config(void) {
  static byte config_row;
   byte i;
   
  if(!digitalRead(switch_down_mapping[0])) config_row--;
  if(!digitalRead(switch_up_mapping[0])) config_row++;

  if (config_row >7) {
    config_row = 0;
  }

  for (i = 0; i < 4; i++) {
    if (!digitalRead(switch_down_mapping[i+1])) {
      tod_off_on_mappings[i].values[config_row]++;
    }
    if (!digitalRead(switch_up_mapping[i+1])) {
      tod_off_on_mappings[i].values[config_row]--;
    }
  }

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("output: ");
  display.print(config_row);
  display.setCursor(0, 12);
  for (i = 0; i < 4; i++) {
    display.setCursor(i*32, 12);
    if (tod_off_on_mappings[i].values[config_row] > 18) {
      display.print("keep");
    } else {
      display.print(tod_off_on_mappings[i].values[config_row]);
    }
    
    display.setCursor(i*32, 20);
    display.print(times[i]);
  }
  
  display.display();

  if (!digitalRead(switch_down_mapping[5]) || ! digitalRead(switch_up_mapping[5])) {
    save_data_to_eeprom();
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("save!");
    display.display();
    delay(100);
  }
 
}
byte display_mode = 0;

#define DM_NORMAL 0
#define DM_TOD 1
#define DM_CONFIG 2

void really_update_universe() {
  // Stops at 2:00PM; 8:00PM; 2:00AM; 8:00AM;
  byte t = (tod_hour+2)/6;
  byte i;
   offon * tmap = tod_off_on_mappings+t;

  for (i = 0; i < 8; i++) {
    byte v;
    v = tmap->values[i];
    if (v<18) {
      outputs[i] = v;
    }
  }
  
}
void loop() {
  int i;

  if (!digitalRead(MODE_SWITCH_BUTTON)) {
    display_mode ++;
    if (display_mode >2) {
      display_mode = 0;
    }
  }

  switch (display_mode){
    case(DM_NORMAL):
      read_switches();
      do_display();
      break;
    case(DM_TOD):
       switches_update_tod();
     break;
    case(DM_CONFIG):
       do_config();
     break;
  }
  read_currents();
  update_pca();
  for (i = 0; i < 8; i++) {
    last_outputs[i] = outputs[i];
  }
  if (need_update) {
    need_update = 0;
    really_update_universe();
  }
  delay(100);

/*
  static int i = 0;
  pca_writereg(7, i);
  pca_writereg(9,10);
  i++;
  if (i > 15) i = 0;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(i);
  display.display();
  delay(400);
  */
}

void update_pca() {
  byte i;
  for (i = 0; i < 8; i++){
    if (outputs[i] != last_outputs[i]) {
      if (enable_pwm) {
        update_output_port(i, kind_of_square(outputs[i]));
      } else {
        if (outputs[i]) {
         // update_output_port(i, 128);
         // delay(50);
          update_output_port(i, 255);
        } else {
          update_output_port(i, 0);
        }
      }
    }
  }
}
void update_output_port(byte port, byte power) {
  byte base = 6 + 4 * port;
  
  if (power == 0) {
    pca_writereg(base+1, 0);
    pca_writereg(base+3, 16);
  }else   if (power == 255) {
    pca_writereg(base+1, 16);
    pca_writereg(base+3, 0);
  } else {
    byte low = (power&0xF) << 4;
    byte high = power >> 4;
    pca_writereg(base, 0);
    pca_writereg(base+1, 0);
    pca_writereg(base+2, low);
    pca_writereg(base+3, high);
  }
}

void pca_writereg(byte reg, byte data) {
  Wire.beginTransmission(66);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}
void pca_setup(void) {
  Wire.begin();
  Wire.beginTransmission(3);
  Wire.write(0);
  Wire.endTransmission();
  delay(100);
  pca_writereg(0,0);
  pca_writereg(1,4);
  pca_writereg(7,15);
}
