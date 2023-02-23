#include <experiment.h>

void screenInit();
void screenClear(bool display = true);


HardwareSerial Serial1(SERIAL1_RX, SERIAL1_TX);
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //ESP32

ITask *task;
Simulation *simulation;

long t, deltaT;
int tCount;

void setup()
{
  Serial1.begin(9600); //connect to second MCU (ESP32) via UART1
  delay(30);
  screenInit();
  screenClear(true);

  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  delay(25);
  digitalWrite(LED_PIN, HIGH);

  task = new LED_Task(LED_PIN, 25, true);
  simulation = new Simulation(5,3);

  if(Serial1)
  {
    Serial1.println("Program started.");
    delay(30);
    Serial1.printf("Screen size %dx%d",screen.width(), screen.height());
  }
  tCount = 0;
}



void loop() 
{
  Ball *balls;
  short nBalls;
  int i;
  float startMomentum, currentMomentum;

  screenClear(false);
  simulation->updatePositions(SCREEN_WIDTH, SCREEN_HEIGHT, task);
  startMomentum = simulation->getStartTotalMomentum();
  currentMomentum = simulation->getCurrentTotalMomentum();
  nBalls = simulation->getBalls(balls);

  for(i=0; i<nBalls; i++)
    screen.fillCircle(balls[i].position.x, balls[i].position.y, balls[i].radius, WHITE);

  if(deltaT > 0)
  {
    screen.setCursor(3, 2);
    screen.print("m:");
    screen.print(currentMomentum);
    screen.print("/");
    screen.print(startMomentum);

    screen.setCursor(70,54);
    screen.print(100000.0/(float)deltaT);
    screen.print(" fps");
  }
  screen.display();
  tCount++;
  if(tCount == 100)
  {
    deltaT = millis()-t;
    t = millis();
    tCount = 0;
  }
  task->Tick();
  delay(1);
}

void screenInit()
{
  screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  screen.clearDisplay();
  screen.drawRect(0,0,screen.width()-1,screen.height()-1, WHITE);
  screen.setTextSize(0);
  screen.setTextWrap(false);
  screen.setTextColor(WHITE);
}

void screenClear(bool display)
{
  screen.clearDisplay();
  screen.drawRect(0,0,screen.width()-1,screen.height()-1, WHITE);
  if(display)
    screen.display();
}

/* LED Task methods */
LED_Task::LED_Task(int pin, int delay, bool swap)
{
  this->pin = pin;
  this->delay = delay;
  this->swap = swap;
  pinMode(pin,OUTPUT);
}

void LED_Task::On()
{
  if(!swap)
    digitalWrite(pin,HIGH);
  else
    digitalWrite(pin,LOW);
  t = millis();
}

void LED_Task::Off()
{
    if(!swap)
    digitalWrite(pin,LOW);
  else
    digitalWrite(pin,HIGH);
}

void LED_Task::Tick()
{
  long t1 = millis();
  if((t1-t)>delay)
    Off();
}