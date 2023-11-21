#include <oled.h>
#include <balls.h>
#include <simulation.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN 13 //if available
#define COLORS 19

/* RGB LED WS 2812 */
#define LEDS_COUNT  1 //for LED strip, we have only one
#define LEDS_PIN	32 //dasduino RGB LED pin
#define CHANNEL		0 //RGB channel 0

#define BUTTON_PIN 18

void IRAM_ATTR ISR_Button();

Simulation *simulation;
ITask *LED_task;

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //OLED
Freenove_ESP32_WS2812 ledRGB = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_RGB);
bool buttonToggle;
TickType_t debounceTime;

u8 BGRcolors[][3]={{255,0,0},{0,255,0},{0,0,255},{255,255,0},{255,0,255},{0,255,255},{255,255,255},
{255,128,128},{128,255,128},{128,128,255},{255,255,128},{255,128,255},{128,255,255},
{64,128,255},{128,64,255},{128,255,64},{64,255,128},{255,64,128},{255,128,64}};

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
  buttonToggle = false;
  debounceTime = xTaskGetTickCount();
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISR_Button, RISING);

  LED_task = new LED_Task(&ledRGB, 5, 40);
  simulation = new Simulation(7,LED_task);

  delay(2000);
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
  LED_task->Tick();
}

/* LED Task methods */
LED_Task::LED_Task(Freenove_ESP32_WS2812* led, u8 brightness, int delay)
{
  this->ledRGB = led;
  this->delay = delay;
  ledRGB->begin();
  this->brightness = brightness;
  colorCycle = 0;
}

void LED_Task::On()
{
  ledRGB->setLedColorData(0,BGRcolors[colorCycle][0],BGRcolors[colorCycle][1],BGRcolors[colorCycle][2]);
  ledRGB->setBrightness(brightness);
  ledRGB->show();
  t = millis();
}

void LED_Task::Off()
{
  ledRGB->setLedColorData(0,BGRcolors[colorCycle][0],BGRcolors[colorCycle][1],BGRcolors[colorCycle][2]);
  ledRGB->setBrightness(0);
  ledRGB->show();
  colorCycle++;
  if(colorCycle>=COLORS)
    colorCycle=0;
}

void LED_Task::Tick()
{
  long t1 = millis();
  if((t1-t)>delay)
    Off();
}

void IRAM_ATTR ISR_Button()
{
  if(xTaskGetTickCount()-debounceTime > 125)
    buttonToggle = !buttonToggle;
  debounceTime = xTaskGetTickCount();
}
