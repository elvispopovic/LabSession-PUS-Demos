#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define __HAL_TIM_GET_COUNTER(__HANDLE__) ((__HANDLE__)->Instance->CNT)

#define SERIAL1_RX PA10
#define SERIAL1_TX PA9

#define LED_PIN PC13

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

struct Point
{
    float x;
    float y;
};

struct Ball
{
    Point position;
    Point speed;
    float radius;
    float mass;
};

/* task interface */
class ITask
{
  public:
    virtual void On()=0;
    virtual void Off()=0;
    virtual void Tick()=0;
};

class Simulation
{
    public:
    Simulation(int nBalls, int radius);
    ~Simulation();
    short getBalls(Ball *&balls);
    void updatePositions(int w, int h, ITask *task);
    float getStartTotalMomentum();
    float getCurrentTotalMomentum();

    private:
    short nBalls;
    Ball *balls;
    float startTotalMomentum, currentTotalMomentum;

    bool wallBounce(int w, int h);
    bool ballBounce(Ball *b1, Ball *b2, double r);
    bool isInterractionCandidate(Ball *b1, Ball *b2);
    void positionCorrection(Ball *b1, Ball *b2);
    void calculateTotalMomentum();
    void keepTotalMomentum();
};


/* LED task derived from ITask pure abstract class */
class LED_Task : public ITask
{
  public:
    LED_Task(int pin, int delay, bool swap);
    void On();
    void Off();
    void Tick();
  private:
    bool swap;
    long t;
    int pin;
    int delay;
};