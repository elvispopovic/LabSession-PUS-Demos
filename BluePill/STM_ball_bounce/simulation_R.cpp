#include <experiment.h>

Simulation::Simulation(int nBalls, int radius)
{
    int i;
    this->nBalls = nBalls;
    balls = new Ball[nBalls];
    startTotalMomentum = currentTotalMomentum = 0.0;
    delay(30);
    srand(millis());
    for(i=0; i<nBalls; i++)
    {
        float r = radius/2.0+radius*(100+rand()%200)/100.0;
        balls[i].radius = r;
        if(i==0)
        {
            balls[i].position.x = r;
            balls[i].position.y = r;
        }
        else
        {
            float r = balls[i-1].radius+balls[i].radius;
            balls[i].position.x=balls[i-1].position.x+r;
            balls[i].position.y=balls[i-1].position.y+r;
        }
        balls[i].speed.x = 0.51;
        balls[i].speed.y = 0.73;
        balls[i].mass = r*r*0.001;
        currentTotalMomentum+=
            balls[i].speed.x*balls[i].mass*balls[i].speed.x*balls[i].mass+
            balls[i].speed.y*balls[i].mass*balls[i].speed.y*balls[i].mass;
    }
    calculateTotalMomentum();
    startTotalMomentum = currentTotalMomentum;
}

short Simulation::getBalls(Ball *&balls)
{
  int i;
  balls = this->balls;
  return nBalls;
}

void Simulation::updatePositions(int w, int h)
{
    int i, j;
    float d;
    wallBounce(w, h);
    for(i=0; i<nBalls; i++)
    {
        balls[i].position.x+=balls[i].speed.x;
        balls[i].position.y+=balls[i].speed.y;
    }
    for(j=0; j<nBalls-1; j++)
      for(i=j+1; i<nBalls; i++)
            if(isInterractionCandidate(balls+j, balls+i))
            {
                float r = balls[j].radius + balls[i].radius;
                float dx = balls[i].position.x - balls[j].position.x;
                float dy = balls[i].position.y - balls[j].position.y;
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
        if(balls[i].position.x<balls[i].radius && balls[i].speed.x<0.0)
            balls[i].speed.x=-balls[i].speed.x;
        if(balls[i].position.y<balls[i].radius && balls[i].speed.y<0.0)
            balls[i].speed.y=-balls[i].speed.y;
        if(balls[i].position.x>w-balls[i].radius && balls[i].speed.x>0.0)
            balls[i].speed.x=-balls[i].speed.x;
        if(balls[i].position.y>h-balls[i].radius && balls[i].speed.y>0.0)
            balls[i].speed.y=-balls[i].speed.y;
    }
}

void Simulation::ballBounce(Ball *b1, Ball *b2, double r)
{
    int i;
    Point deltaPos, deltaSpeed;
    float speedr;
    float vr = 0.0;
    
    deltaPos.x = b2->position.x-b1->position.x;
    deltaSpeed.x = b2->speed.x-b1->speed.x;
    vr+=deltaPos.x * deltaSpeed.x;
    deltaPos.y = b2->position.y-b1->position.y;
    deltaSpeed.y = b2->speed.y-b1->speed.y;
    vr+=deltaPos.y*deltaSpeed.y;

    vr /= (r*r); //divide again to be able to multiply with position vector to get vector
    if(vr<0) //bounce
    {
        float M = b1->mass+b2->mass; //mass of both balls
        speedr=deltaPos.x*vr; //speed change along distance vector by components
        deltaSpeed.x+=speedr*((b2->mass-b1->mass)/M-1);
        //same as deltaSpeed.x-=speedr and deltaSpeed.x+=speedr*(b2->mass-b1->mass)/M
        b2->speed.x=b1->speed.x+deltaSpeed.x; //return to lab system
        b1->speed.x=b1->speed.x+speedr*2.0*b2->mass/M; //*2.0*b2->mass/M; //momentum ball 1

        speedr=deltaPos.y*vr; //speed change along distance vector by components
        deltaSpeed.y+=speedr*((b2->mass-b1->mass)/M-1);
        //same as deltaSpeed.y-=speedr and deltaSpeed.y+=speedr*(b2->mass-b1->mass)/M
        b2->speed.y=b1->speed.y+deltaSpeed.y; //return to lab system
        b1->speed.y=b1->speed.y+speedr*2.0*b2->mass/M; //*2.0*b2->mass/M; //momentum ball 1
    }
}

void Simulation::positionCorrection(Ball *b1, Ball *b2)
{
    int i;
    float corr = 0.0;
    Point deltaPos;
    float r = b1->radius + b2->radius;
    deltaPos.x = b2->position.x-b1->position.x;
    corr+=deltaPos.x*deltaPos.x;
    deltaPos.y = b2->position.y-b1->position.y;
    corr+=deltaPos.y*deltaPos.y;
    corr = r/sqrt(corr); //ratio distance/real distance

    b2->position.x = b1->position.x+deltaPos.x*corr;
    b2->position.y = b1->position.y+deltaPos.y*corr;
}

bool Simulation::isInterractionCandidate(Ball *b1, Ball *b2)
{
    bool br[2];
    int i;
    float r = b1->radius + b2->radius;
    if(abs(b2->position.x-b1->position.x)<=r)
          br[0] = true;
      else
          br[0] = false;
    if(abs(b2->position.y-b1->position.y)<=r)
          br[1] = true;
      else
          br[1] = false;
    return br[0]&&br[1];
}

void Simulation::calculateTotalMomentum()
{
    int i;
    currentTotalMomentum = 0.0;
    for(i=0; i<nBalls; i++)
    {
        Point m;
        m.x = balls[i].speed.x * balls[i].mass;
        m.y = balls[i].speed.y * balls[i].mass;
        currentTotalMomentum += m.x * m.x + m.y * m.y;
    }
    currentTotalMomentum = sqrt(currentTotalMomentum);
}

void Simulation::keepTotalMomentum()
{
    int i,j;
    float ratio = startTotalMomentum/currentTotalMomentum;
    for(j=0; j<nBalls; j++)
    {
        balls[j].speed.x*=ratio;
        balls[j].speed.y*=ratio;
    }
}


float Simulation::getStartTotalMomentum()
{
    return startTotalMomentum;
}

float Simulation::getCurrentTotalMomentum()
{
    return currentTotalMomentum;
}