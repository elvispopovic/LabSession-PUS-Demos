#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <balls.h>
#include <simulation.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 13 //if available

#define BUTTON_PIN 18

#define N_BALLS 6

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //OLED
extern Freenove_ESP32_WS2812 stripRGB;

void IRAM_ATTR ISR_Button();

Simulation *simulation;
ITask *stripTask;

bool buttonToggle;
TickType_t debounceTime;

void setup()
{
  Serial.begin(115200);
  if(!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Screen not present.");
    while(1)
      delay(1);
  }
  screen.clearDisplay();
  screen.setTextColor(WHITE);
  screen.setTextSize(0);
  screen.display();

  pinMode(BUTTON_PIN, INPUT_PULLUP); //obligate to use pullup resistor for buttons
  buttonToggle = true;
  debounceTime = xTaskGetTickCount();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISR_Button, RISING);

  stripTask = new LEDStrip_Task(&stripRGB, 20,40);
  simulation = new Simulation(N_BALLS, stripTask);
}

long t, deltaT;
int tCount = 0;

void loop()
{
  Ball *balls;
  short nBalls;
  float startMomentum, currentMomentum;
  int i;
  screen.clearDisplay();
  simulation->updatePositions(SCREEN_WIDTH, SCREEN_HEIGHT);
  startMomentum = simulation->getStartTotalMomentum();
  currentMomentum = simulation->getCurrentTotalMomentum();
  nBalls = simulation->getBalls(balls);
  
  screen.drawRect(0,0,128,64,WHITE);
  for(i=0; i<nBalls; i++)
  {
    float r = balls[i].radius;
    int bmp = (int)(balls[i].radius-5.0);
    if(buttonToggle)
      screen.drawBitmap(balls[i].position[0]-r, balls[i].position[1]-r,bitmapPointers[bmp], r*2,r*2,WHITE);
    else
      screen.drawCircle(balls[i].position[0], balls[i].position[1], balls[i].radius, WHITE);
  }
  if(!buttonToggle)
  {
    screen.setCursor(0, 1);
    screen.printf("m:%.3f/%.3f", currentMomentum, startMomentum);
    screen.setCursor(74,54);
    screen.printf("%.1f fps", 100000.0/(float)deltaT);
  }
  screen.display();
  tCount++;
  if(tCount == 100)
  {
    deltaT = millis()-t;
    t = millis();
    tCount = 0;
  }
  stripTask->Tick();
}


void IRAM_ATTR ISR_Button()
{
  if(xTaskGetTickCount()-debounceTime > 125)
    buttonToggle = !buttonToggle;
  debounceTime = xTaskGetTickCount();
}

