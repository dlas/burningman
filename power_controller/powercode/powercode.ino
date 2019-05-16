

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 19
Adafruit_SSD1306 display(OLED_RESET);

byte current_sense_mapping[]={7,6,5,4,3,2,1,0};
byte switch_down_mapping[]={30, 23,25, 27, 29, 36, 34, 32};
byte switch_up_mapping[]={31, 22, 24, 26, 28, 37, 35, 33};
byte enable_pwm = 1;


#define MODE_SWITCH_BUTTON 20
#define PWM_CONTROLLER_ADDRESS 66





void setup()   {                
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
  pinMode(14, OUTPUT);
  digitalWrite(14,0);
  delay(1);
  enable_pwm = digitalRead(15);

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

void loop() {
  int i;
  read_currents();
  read_switches();
  do_display();
  update_pca();
  for (i = 0; i < 8; i++) {
    last_outputs[i] = outputs[i];
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
