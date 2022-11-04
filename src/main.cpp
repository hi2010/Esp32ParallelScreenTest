#include <Arduino.h>
#include "COMMANDS.h"
#include "SerialCom.h"

/*
  Ellipse drawing example

  This sketch does not use any fonts.
*/

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// These are the pins for the shield!
#define YP TFT_CS  // must be an analog pin, use "An" notation!
#define XM TFT_DC  // must be an analog pin, use "An" notation!
#define YM TFT_D1   // can be a digital pin
#define XP TFT_D0   // can be a digital pin

#define MINPRESSURE 10
#define MAXPRESSURE 2000

// 36, 39, 34, 35, 32, 33 -> ADC1 (2 used for wifi)

void setup(void) {
  Serial.begin(9600);
  #define RXD2 16
  #define TXD2 17
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  tft.init();

  tft.setRotation(1);
  tft.fillScreen(TFT_OLIVE);
  tft.drawRect(45, 45, 10, 10, TFT_RED);
  tft.drawRect(295, 45, 10, 10, TFT_RED);
  tft.drawRect(45, 205, 10, 10, TFT_RED);
  tft.drawRect(295, 205, 10, 10, TFT_RED);
  Param p;
  p.data2param(32.f, Parameters::Co22BeerTime);
}


/***************************************************************************************
** Function name:           begin_touch_read_write - was spi_begin_touch
** Description:             Start transaction and select touch controller
***************************************************************************************/
// The touch controller has a low SPI clock rate
inline void begin_touch_read_write(void){
  DMA_BUSY_CHECK;
  CS_L; // Just in case it has been left low
  SET_BUS_READ_MODE;
  T_CS_L;
}

/***************************************************************************************
** Function name:           end_touch_read_write - was spi_end_touch
** Description:             End transaction and deselect touch controller
***************************************************************************************/
inline void end_touch_read_write(void){
  T_CS_H;
  //SET_BUS_WRITE_MODE;
}

struct Point2d {
  float x, y;
  bool valid = false;
};

Point2d getPositionFromAnalog(uint16_t ax, uint16_t ay) {
  auto x = -0.437336815*ax + 0.097911227*ay + 448.694517;
  auto y = 0.146214099*ax + -0.27154047*ay + 800.2872063;
  Point2d pos;
  pos.x = x;
  pos.y = y;
  return pos;
}

Point2d getTouched() {
  begin_touch_read_write();
  pinMode(YM, OUTPUT);
  pinMode(XP, OUTPUT);
  pinMode(YP, INPUT);
  pinMode(XM, INPUT);
  digitalWrite(YM, LOW);
  digitalWrite(XP, HIGH);
  delayMicroseconds(40);
  uint16_t lowb;
  uint16_t highb;
  uint8_t nSamples = 14;
  for (auto i = 0; i < nSamples; i++) {
    lowb += analogRead(34);
    highb += analogRead(35);
  }
  lowb = lowb / nSamples;
  highb = highb / nSamples;
  int a = (int) lowb;
  int b = (int) highb;
  //Serial.print(a);
  //Serial.print("\t");
  //Serial.println(b);
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(XP, LOW);
  end_touch_read_write();
  Point2d pos;
  if (lowb > 20 || highb < 4068 ) {
    //pos = getPositionFromAnalog(lowb, highb);
    pos.x = a;
    pos.y = b;
    pos.valid = true;
    return pos;
  }
  return pos;
}

long lastUpdate = millis();
uint32_t clrs[] = {TFT_ORANGE, TFT_OLIVE, TFT_GOLD};
uint8_t clrIdx = 0;
Point2d lastPos;
void loop() {
  if (tft.spiBusyCheck == 0 && millis() - lastUpdate > 5) {
    //
    Point2d pos = getTouched();
    if (pos.valid) {
      //tft.fillScreen(clrs[clrIdx]);
      //clrIdx ++;
      //clrIdx = clrIdx % 3;
      lastUpdate = millis();
      auto x = pos.x * tft.width() - 10;
      auto y = pos.y * tft.height() - 10;
      /*Serial.print(pos.x);
      Serial.print("\t");
      //Serial.print(x);
      //Serial.print("\t");
      Serial.print(pos.y);
      Serial.print("\t");*/
      auto calcPos = getPositionFromAnalog(pos.x, pos.y);
      /*Serial.print(calcPos.x);
      Serial.print("\t");
      Serial.println(calcPos.y);*/
      tft.fillRect(calcPos.x-5, calcPos.y-5, 10, 10, TFT_BROWN);
      if (lastPos.valid || true) {
        tft.drawLine(lastPos.x, lastPos.y, calcPos.x, calcPos.y, TFT_BLUE);
      }
      lastPos = calcPos;
      //Serial.println(y);
      //tft.fillRect(x, y, 20, 20, TFT_CASET);
      Serial2.println("hihi from esp32");
    } else {
      lastPos.valid = false;
    }
    //tft.startWrite();
  }

  //delay(2000);
}



