/**
 * Digital clock
 * Components:
 *    Arduino uno
 *    Real time clock module DS1307
 */

#include "LedControl.h" 
#include "fonts.h"
#include <Wire.h>
#include "RTClib.h"

/**
 * Wire:
 * Arduino A4 - RTC-SDA
 * Arduino A5 - RTC-SCL
 */
RTC_DS1307 RTC;

/**
 * 4 8x8 led matrix wiring:
 */
#define PIN_DATA 12
#define PIN_CLK  11
#define PIN_CS   10
#define NUM_OF_DEVICES 4

LedControl lc = LedControl(PIN_DATA, PIN_CLK, PIN_CS, NUM_OF_DEVICES);

byte * getDigit(int n, bool right){
  return right?digitsRight[n]:digitsLeft[n];
}

int colonLeds[4] = {1,2,5,6};
/**
 * Show/Hide colon between hours and minutes
 */
void colon(bool on) {
   for(int i=0;i<4;i++) {
      // lc.setLed(1,colonLeds[i], 0, on); // Uncomment this for wider colon
      lc.setLed(2,colonLeds[i], 7, on);
   }
}

void initDisplay(){
  for(int index=0; index < lc.getDeviceCount(); index++) {
      lc.shutdown(index, false);
  }
  setBrightness(0);
  clearDisplay();
}

void clearDisplay() {
  for(int index=0; index < lc.getDeviceCount(); index++) {    
    lc.clearDisplay(index);
  }
}

void setBrightness(int br) {
  int br1 = br<0?0:br>15?15:br;
  for(int index=0; index < lc.getDeviceCount(); index++) {  
      lc.setIntensity(index, br1);
  }
}

void initClock(){
  Wire.begin();
  RTC.begin();
  //  if (! RTC.isrunning()) {
  //    // This will reflect the time that your sketch was compiled
  //    RTC.adjust(DateTime(__DATE__, __TIME__));
  //  }
}

void digit(int dev, int digit, boolean right) {
  byte *rows = getDigit(digit, right);
  for(int i=0;i<8;i++) {
     lc.setRow(dev, i, rows[i]);
  }
}

void getTime(int *t) {
  DateTime now = RTC.now(); 
  int h = now.hour();
  int m = now.minute();
  
  t[3] = m % 10;
  t[2] = m / 10 % 10;

  t[0] = h % 10;
  t[1] = h / 10 % 10;
}

int tm[4] = {0,0,0,0};
int newTm[4] = {0,0,0,0};

void logTime() {
  digit(3, tm[1], false);
  digit(2, tm[0], false);
  digit(1, tm[2], true);
  digit(0, tm[3], true);
}

void setup() {
  initClock();
  initDisplay();  
}

void loop() {
  getTime(newTm);
  // Update display only if someting changed (prevents flikering of colon)
  if(tm[3] != newTm[3] || tm[2] != newTm[2] || tm[1] != newTm[1] || tm[0] != newTm[0]) {
    getTime(tm);
    logTime();    
  }
  colon((millis() % 2000L)>1000L); // 1 - sec on | 1 sec off
}
