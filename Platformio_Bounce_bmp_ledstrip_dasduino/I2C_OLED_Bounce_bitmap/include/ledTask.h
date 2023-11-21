#include <Arduino.h>
#ifndef LEDTASK_H
#define LEDTASK_H
#define WALLS_TRIGGER 0
#define COLLISION_TRIGGER 1

#include <Freenove_WS2812_Lib_for_ESP32.h>
#define COLORS 25 
#define CHANNEL 0 //RGB channel 0

/* LED RGB pin */
#define STRIP_COUNT 24 //24 diodes in strip, set 1 for onboard LED only
#define STRIP_PIN 32


/* task interface */
class ITask
{
  public:
    virtual void On()=0;
    virtual void Off()=0;
    virtual void Tick()=0;
};

/* LED task derived from ITask pure abstract class */
class LED_Task : virtual public ITask
{
  public:
    LED_Task(Freenove_ESP32_WS2812* led, u8 brightness, int taskDelay);
  private:
    void On();
    void Off();
    void Tick();
  protected:
    short colorCycle;
    long t;
    int taskDelay;
    Freenove_ESP32_WS2812 *ledRGB;
};

class LEDStrip_Task : public LED_Task, virtual public ITask
{
  public:
    LEDStrip_Task(Freenove_ESP32_WS2812* led, u8 brightness, int taskDelay);
    void On();
    void Off();
    void Tick();
  private:
    u8 ledColors[STRIP_COUNT][3];
};

#endif