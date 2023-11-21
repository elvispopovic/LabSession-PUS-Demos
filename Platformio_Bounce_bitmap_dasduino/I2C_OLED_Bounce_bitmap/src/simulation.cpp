/* Simulation class methods */
#include <simulation.h>

Simulation::Simulation(int nBalls, ITask *task)
{
    int i;
    this->nBalls = nBalls;
    this->task = task;
    balls = new Ball[nBalls];
    startTotalMomentum = 0.0;
    currentTotalMomentum = 0.0;
    srand(time(0));
    // start condition
    for(i=0; i<nBalls; i++)
    {   
        int r = rand()%10;
        balls[i].radius = r+5.0;

        if(i==0)
        {
            balls[i].position[0]=balls[i].radius;
            balls[i].position[1]=balls[i].radius;
        }
        else
        {
            balls[i].position[0]=balls[i-1].position[0]+balls[i-1].radius+balls[i].radius;
            balls[i].position[1]=balls[i-1].position[1]+balls[i-1].radius+balls[i].radius;
        }
        balls[i].speed[0]=0.5;
        balls[i].speed[1]=0.73;
        balls[i].mass = balls[i].radius*balls[i].radius*0.001;
        currentTotalMomentum+=
            balls[i].speed[0]*balls[i].mass*balls[i].speed[0]*balls[i].mass+
            balls[i].speed[1]*balls[i].mass*balls[i].speed[1]*balls[i].mass;
    }
    calculateTotalMomentum();
    startTotalMomentum = currentTotalMomentum;
}

Simulation::~Simulation()
{
  delete[] balls;
}

void Simulation::updatePositions(int w, int h)
 {
  int i, j;
  float d;
  wallBounce(w, h);
  for(j=0; j<nBalls; j++)
      for(i=0; i<2; i++)
      {
          float r = balls[j].radius + balls[i].radius;
          balls[j].position[i]+=balls[j].speed[i];
      }
  for(j=0; j<nBalls-1; j++)
      for(i=j+1; i<nBalls; i++)
          if(isInterractionCandidate(balls+j, balls+i))
          {
              float r = balls[j].radius + balls[i].radius;
              float dx = balls[i].position[0] - balls[j].position[0];
              float dy = balls[i].position[1] - balls[j].position[1];
              d = sqrt(dx*dx+dy*dy);
              if(d<=r)
              {
                  positionCorrection(balls+j,balls+i);
                  ballBounce(balls+j,balls+i,r);
                  keepTotalMomentum();
              }
          }
  calculateTotalMomentum();
 }

void Simulation::wallBounce(int w, int h)
{
    int i;
    for(i=0; i<nBalls; i++)
    {
        if(balls[i].position[0]<balls[i].radius && balls[i].speed[0]<0.0)
            balls[i].speed[0]=-balls[i].speed[0];
        if(balls[i].position[1]<balls[i].radius && balls[i].speed[1]<0.0)
            balls[i].speed[1]=-balls[i].speed[1];
        if(balls[i].position[0]>w-balls[i].radius && balls[i].speed[0]>0.0)
            balls[i].speed[0]=-balls[i].speed[0];
        if(balls[i].position[1]>h-balls[i].radius && balls[i].speed[1]>0.0)
            balls[i].speed[1]=-balls[i].speed[1];
    }
}

bool Simulation::isInterractionCandidate(Ball *b1, Ball *b2)
{
  bool br[2];
  int i;
  float r = b1->radius + b2->radius;
  for(i=0; i<2; i++)
      if(abs(b2->position[i]-b1->position[i])<=r)
          br[i] = true;
      else
          br[i] = false;
  return br[0]&&br[1];
}

void Simulation::positionCorrection(Ball *b1, Ball *b2)
{
  int i;
  float corr = 0.0;
  float deltaPos[2];
  float r = b1->radius + b2->radius;
  for(i=0; i<2; i++)
  {
      deltaPos[i] = b2->position[i]-b1->position[i];
      corr+=deltaPos[i]*deltaPos[i];
  }
  corr = r/sqrt(corr); //ratio distance/real distance
  for(i=0; i<2; i++)
      b2->position[i]=b1->position[i]+deltaPos[i]*corr;
}

void Simulation::ballBounce(Ball *b1, Ball *b2, double r)
{
  int i;
  float deltaPos[2], deltaSpeed[2], speedr;
  float vr = 0.0;
  task->On();
  for(i=0; i<2; i++)
  {
      deltaPos[i] = b2->position[i]-b1->position[i];
      deltaSpeed[i] = b2->speed[i]-b1->speed[i];
      vr+=deltaPos[i]*deltaSpeed[i]; //scalar projection to distance vector
  }
  // vr /=r; scalar speed, divide with length od distance vector speed * cos (theta)
  vr /= (r*r); //divide again to be able to multiply with position vector to get vector
  if(vr<0) //bounce
  {
      float M = b1->mass+b2->mass; //mass of both balls
      //double x = vr/r; //speed to distance vector length
      for(i=0; i<2; i++)
      {
          speedr=deltaPos[i]*vr; //speed change along distance vector by components
          deltaSpeed[i]+=speedr*((b2->mass-b1->mass)/M-1);
          //same as deltaSpeed[i]-=speedr and deltaSpeed[i]+=speedr*(b2->mass-b1->mass)/M
          b2->speed[i]=b1->speed[i]+deltaSpeed[i]; //return to lab system
          b1->speed[i]=b1->speed[i]+speedr*2.0*b2->mass/M; //*2.0*b2->mass/M; //momentum ball 1
      }  
  }
}

void Simulation::calculateTotalMomentum()
{
    int i;
    currentTotalMomentum = 0.0;
    for(i=0; i<nBalls; i++)
        currentTotalMomentum+=
            balls[i].speed[0]*balls[i].mass*balls[i].speed[0]*balls[i].mass+
            balls[i].speed[1]*balls[i].mass*balls[i].speed[1]*balls[i].mass;
    currentTotalMomentum = sqrt(currentTotalMomentum);
}

void Simulation::keepTotalMomentum()
{
    int i,j;
    float ratio = startTotalMomentum/currentTotalMomentum;
    for(j=0; j<nBalls; j++)
        for(i=0; i<2; i++)
            balls[j].speed[i]*=ratio;
}

short Simulation::getBalls(Ball *&balls)
{
  int i;
  balls = this->balls;
  return nBalls;
}

float Simulation::getStartTotalMomentum()
{
    return startTotalMomentum;
}

float Simulation::getCurrentTotalMomentum()
{
    return currentTotalMomentum;
}