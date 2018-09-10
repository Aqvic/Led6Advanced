
#include <Arduino.h>
#include <avr/wdt.h>
#include "Settings.h"


int pwms[CHANNELS_NUM] = {0, 0, 0, 0, 0, 0}; //unused values, but initialized )))
int blyncInterval = BLYNC_INTERVAL_2S;

void setup() {

  wdt_enable(WDTO_8S);

  for(int i=0; i<CHANNELS_NUM; i++){
    pinMode(OUT_PINS[i], OUTPUT);
    analogWrite(OUT_PINS[i], 0);
  }

  fillPwms(); //for analog initialization and inputs stabilization
  //resetWDT(); //uncoment it if using less then 1S WDT timeout

  pinMode(BLYNC_PIN, OUTPUT);
  digitalWrite(BLYNC_PIN, LOW);

  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

void loop() {

  sunrise();
  day();
  sunset(); 

  // --- waiting to poweroff --- FOREVER!!! 
  for(int i=0; i<CHANNELS_NUM; i++){
    analogOut(OUT_PINS[i], 0);
  }
  digitalWrite(BLYNC_PIN, HIGH);

  while(true){
    resetWDT();
    delay(5000); //TODO wdt-compatible delay, 5 sec - ok
  }
}

byte analogToPWM(int analogValue){
    // map 10-bit ADC to 8-bit PWM, with trimming
    return map(constrain(analogValue, DOWN_TRIM_LEVEL, UP_TRIM_LEVEL), DOWN_TRIM_LEVEL, UP_TRIM_LEVEL, 0, 255); // 0..255 analogAut band
}

int readAnalog(byte pin){

  int buf[20], temp;
  int avgValue = 0;

  //Get 20 sample value from the sensor for smooth the value
  for(int i=0; i<20; i++){ 
    buf[i] = analogRead(pin);
    delay(1);
  }

  //sort the analog from small to large
  for(int i=0; i<19; i++){
    for(int j=i+1; j<20; j++){
      if(buf[i] > buf[j]){
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  
  //take the average value of 6 center sample
  for(int i=5; i<15; i++){
    avgValue += buf[i];
  }

  return avgValue/10;
}

void resetWDT(){
  wdt_reset();
}


long millisToPWM(long x, long in_min, long in_max, long out_min, long out_max){ 
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void fillPwms(){
  for(int i=0; i<CHANNELS_NUM; i++){
    pwms[i] = analogToPWM(readAnalog(IN_PINS[i]));
  }    
}


void sunrise(){

  #ifdef DEBUG
    Serial.println(F("sunrise started"));
  #endif

  blyncInterval = BLYNC_INTERVAL_200MS;
  fillPwms();

  volatile long curenttime = millis();
  long starttime = curenttime;
  long endtime = SUNRISE_DURATION + starttime;

  #ifdef DEBUG
    String debug = (String)starttime + " " + (String)curenttime + " " + (String)endtime;
    Serial.println(debug);
    debug = "";
    for(int i=0; i<CHANNELS_NUM; i++){
        debug = debug + (String)pwms[i] + " ";
    }
    Serial.println(debug);
  #endif
  
  while(curenttime <= endtime){
    for(int i=0; i<CHANNELS_NUM; i++){
        analogOut(OUT_PINS[i], millisToPWM(curenttime, starttime, endtime, 0L, (long)pwms[i]));
    }
    curenttime = millis();
    routine();
  }

  #ifdef DEBUG
    debug = (String)starttime + " " + (String)curenttime + " " + (String)endtime;
    Serial.println(debug);
    Serial.println(F("sunrise ended"));
  #endif
}


void sunset(){

  #ifdef DEBUG
    Serial.println(F("sunsed started"));
  #endif

  blyncInterval = BLYNC_INTERVAL_200MS;
  fillPwms();

  volatile long curenttime = millis();
  long starttime = curenttime;
  long endtime = SUNSET_DURATION + starttime;

  #ifdef DEBUG
    String debug = (String)starttime + " " + (String)curenttime + " " + (String)endtime;
    Serial.println(debug);
    debug = "";
    for(int i=0; i<CHANNELS_NUM; i++){
        debug = debug + (String)pwms[i] + " ";
    }
    Serial.println(debug);
  #endif
  
  while(curenttime <= endtime){
    for(int i=0; i<CHANNELS_NUM; i++){
        analogOut(OUT_PINS[i], millisToPWM(curenttime, starttime, endtime, (long)pwms[i], 0L));
    }
    curenttime = millis();
    routine();
  }

  #ifdef DEBUG
    debug = (String)starttime + " " + (String)curenttime + " " + (String)endtime;
    Serial.println(debug);
    Serial.println(F("sunset ended"));
  #endif
}

void day(){

  #ifdef DEBUG
    Serial.println(F("day started"));
  #endif

  blyncInterval = BLYNC_INTERVAL_2S;

  volatile long curenttime = millis();
  long starttime = curenttime;
  long endtime = DAYLIGHT_DURATION + starttime;

  #ifdef DEBUG
    String debug = (String)starttime + " " + (String)curenttime + " " + (String)endtime;
    Serial.println(debug);
  #endif
  
  while(curenttime <= endtime){
    for(int i=0; i<CHANNELS_NUM; i++){
        analogOut(OUT_PINS[i], analogToPWM(readAnalog(IN_PINS[i])));
    }
    curenttime = millis();
    routine();
  }

  #ifdef DEBUG
    debug = (String)starttime + " " + (String)curenttime + " " + (String)endtime;
    Serial.println(debug);
    Serial.println(F("day ended"));
  #endif
}


void routine(){
  resetWDT();
  blync();  
}

void blync(){
  static boolean state;
  static unsigned long last;

  if(millis() > (last+blyncInterval)){
    state = !state;
    digitalWrite(BLYNC_PIN, (state ? HIGH : LOW));
    last = millis();
  }
}

void analogOut(byte pin, byte value){
  analogWrite(pin, value);
}



//
