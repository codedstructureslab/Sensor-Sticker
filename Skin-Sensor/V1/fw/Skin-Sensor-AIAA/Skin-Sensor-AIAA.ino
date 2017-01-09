#include "APA.h"
#include <lp5521.h>
#include <LPS.h>


Lp5521 ledChip(0x32);
LPS ps1;
LPS ps2;

uint32_t curtime;
uint32_t pressure_sensor_update_time, led_update_time,verbose_time;
uint32_t calibration_time = 30000;

uint32_t pressure_sensor_update_interval = 50;
uint32_t led_update_interval = 50;
uint32_t verbose_interval = 50;

uint8_t rainbowtime = 0;

float raw_pa_factor = 1.0/4096.0;

uint8_t num_samples = 50;

float num_sample_factor = 1.0/num_samples;
float num_sample_factor_var = 1.0/(num_samples-1);

float var_threshold_factor = 30.0;

float ps1_val_cur, ps2_val_cur;
float ps1_val_avg, ps2_val_avg;
float ps1_val_var, ps2_val_var;

float ps1_baseline_pressure, ps2_baseline_pressure;
uint8_t baseline_counter = 0;

bool verbose_mode = true;
bool calibrating = true;

void setup() {
  SerialUSB.begin(9600);
  APA0.begin(9600);
  APA1.begin(9600);
  APA2.begin(9600);

  if(verbose_mode){
    delay(3000);
    Serial.println("Starting LED");
  }

  ledChip.Begin();
  
  if(verbose_mode){
    Serial.println("Enabling LED");
  }
  ledChip.Enable();

  if(verbose_mode){
    SerialUSB.println("Initializing Pressure Sensor 1");
  }
  
  ps1.init(LPS::deviceType::device_25H,LPS::sa0State::sa0_low);

  if(verbose_mode){
    SerialUSB.println("Initializing Pressure Sensor 2");
  }
  ps2.init(LPS::deviceType::device_25H,LPS::sa0State::sa0_high);

  if(verbose_mode){
    SerialUSB.println("Enabling Pressure Sensor 1");
  }
  ps1.enableDefault();
  
  if(verbose_mode){
    SerialUSB.println("Enabling Pressure Sensor 2");
  }
  ps2.enableDefault();

  curtime = 0;
  pressure_sensor_update_time = 0;
  led_update_time = 0;
  verbose_time = 0;
  
  ps1_val_avg = raw_pa_factor*ps1.readPressureRaw();
  ps2_val_avg = raw_pa_factor*ps2.readPressureRaw();

  ps1_val_var = 0;
  ps2_val_var = 0;

  delay(500);
}

void loop() {
  curtime = millis();
  
  if(curtime - pressure_sensor_update_time > pressure_sensor_update_interval){
    pressure_sensor_update_time = curtime;
    updatePressure();
  }

  if(curtime - led_update_time > led_update_interval){
    led_update_time = curtime;
    if(calibrating)
      rainbow();
    else
      updateLED();
  }
  if(calibrating && curtime > calibration_time){
    calibrating = false;
  }
  if(verbose_mode && curtime - verbose_time > verbose_interval){
    verbose_time = curtime; 
    SerialUSB.print(ps1_val_cur);
    SerialUSB.print("\t");
    ///*
    SerialUSB.print(ps1_val_avg + ps1_val_var*num_sample_factor_var*var_threshold_factor);
    SerialUSB.print("\t");
    SerialUSB.print(ps1_val_avg - ps1_val_var*num_sample_factor_var*var_threshold_factor);
    SerialUSB.print("\t"); 
    //*/
    SerialUSB.print(ps2_val_cur);
    ///*
    SerialUSB.print("\t");
    SerialUSB.print(ps2_val_avg + ps2_val_var*num_sample_factor_var*var_threshold_factor);
    SerialUSB.print("\t");
    SerialUSB.println(ps2_val_avg - ps2_val_var*num_sample_factor_var*var_threshold_factor);
    //*/
  }
}

void updatePressure(){
  ps1_val_cur = raw_pa_factor*ps1.readPressureRaw();
  ps2_val_cur = raw_pa_factor*ps2.readPressureRaw();
  
  if(calibrating){
    ps1_val_var = ps1_val_var + (ps1_val_cur-ps1_val_avg)*(ps1_val_cur-(ps1_val_avg+num_sample_factor*(ps1_val_cur-ps1_val_avg)));
    ps1_val_avg = ps1_val_avg+num_sample_factor*(ps1_val_cur-ps1_val_avg);
    ps2_val_var = ps2_val_var + (ps2_val_cur-ps2_val_avg)*(ps2_val_cur-(ps2_val_avg+num_sample_factor*(ps2_val_cur-ps2_val_avg)));
    ps2_val_avg = ps2_val_avg+num_sample_factor*(ps2_val_cur-ps2_val_avg);
    if(ps1_val_cur > 1260)
      ps1_val_cur = 1260;
    if(ps2_val_cur > 1260)
      ps2_val_cur = 1260;
    if(ps1_val_avg > 1260 || ps1_val_avg < 960)
      ps1_val_avg = ps1_val_cur;
    if(ps2_val_avg > 1260 || ps2_val_avg < 960)
      ps2_val_avg = ps2_val_cur;
    if(ps2_val_var > 100)
      ps2_val_var = 0;  
    if(ps1_val_var > 100)
      ps1_val_var = 0;
  }
  else{
    if(abs(ps1_val_cur-ps1_val_avg) > 3.0*ps1_val_var*num_sample_factor_var){
      ps1_val_avg = ps1_val_avg + (ps1_val_cur-ps1_val_avg)*num_sample_factor*0.01;
    }
    if(abs(ps2_val_cur-ps2_val_avg) > 3.0*ps2_val_var*num_sample_factor_var){
      ps2_val_avg = ps2_val_avg + (ps2_val_cur-ps2_val_avg)*num_sample_factor*0.01;
    }
  }
}

void updateLED(){
  uint8_t rv = 0;
  uint8_t gv = 0;
  uint8_t bv = 0;
  
  if(abs(ps1_val_cur-ps1_val_avg) > var_threshold_factor*ps1_val_var*num_sample_factor_var){
    rv = 127;
  }

  if(abs(ps2_val_cur-ps2_val_avg) > var_threshold_factor*ps2_val_var*num_sample_factor_var){
    bv = 127;
  }

  ledChip.SetChannelPWM(0,rv); 
  ledChip.SetChannelPWM(1,bv);
  ledChip.SetChannelPWM(2,gv);
}

char* readMessage(APA_Node *node){
  if(node->getId() == 0){
    
  }
}

void rainbow() {
  rainbowtime = (rainbowtime + 1)%255;
  uint32_t color = Wheel(rainbowtime&255);
  ledChip.SetChannelPWM(0, (color >> 16) & 0xFF);
  ledChip.SetChannelPWM(1, (color >> 8) & 0xFF);
  ledChip.SetChannelPWM(2, color & 0xFF);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return (uint32_t)(((255 - WheelPos * 3)<<16) | (WheelPos * 3));
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return (uint32_t)(((WheelPos * 3)<<8) | (255 - WheelPos * 3));
  }
  WheelPos -= 170;
  return (uint32_t)(((WheelPos * 3)<<16) | ((255 - WheelPos * 3)<<8));
}

