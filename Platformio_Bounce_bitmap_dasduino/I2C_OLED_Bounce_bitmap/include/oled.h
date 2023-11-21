#ifndef OLED_H
#define OLED_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Freenove_WS2812_Lib_for_ESP32.h>

/* task interface */
class ITask
{
  public:
    virtual void On()=0;
    virtual void Off()=0;
    virtual void Tick()=0;
};

/* LED task derived from ITask pure abstract class */
class LED_Task : public ITask
{
  public:
    LED_Task(Freenove_ESP32_WS2812* led, u8 brightness, int delay);
    void On();
    void Off();
    void Tick();
  private:
    Freenove_ESP32_WS2812 *ledRGB;
    short colorCycle;
    long t;
    int brightness;
    int delay;
};

#endif