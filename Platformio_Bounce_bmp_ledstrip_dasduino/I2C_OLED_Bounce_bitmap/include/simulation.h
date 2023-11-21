#ifndef SIMULATION_H
#define SIMULATION_H
#include <ledTask.h>

struct Ball
{
  float position[2];
  float speed[2];
  float radius;
  float mass;
};

class Simulation
{
  public:
    Simulation(int nBalls, ITask *task);
    ~Simulation();
    void updatePositions(int w, int h);
    short getBalls(Ball *&balls);
    float getStartTotalMomentum();
    float getCurrentTotalMomentum();

  private:
    ITask *task;
    short nBalls;
    float startTotalMomentum, currentTotalMomentum;
    Ball *balls;
    bool isInterractionCandidate(Ball *b1, Ball *b2);
    void wallBounce(int w, int h);
    void positionCorrection(Ball *b1, Ball *b2);
    void ballBounce(Ball *b1, Ball *b2, double r);
    void calculateTotalMomentum();
    void keepTotalMomentum();
};

#endif