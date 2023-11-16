#include <Arduino.h>
#include <Wire.h>
#include <mutex>

// Regular ESP 32
// #define SDA0_Pin 21
// #define SCL0_Pin 22

// ESP32 CAM
#define SDA0_Pin 14
#define SCL0_Pin 15

#define I2C_Freq 100000
#define I2C_DEV_ADDR 0x10
//#define LED_PIN 5 //ESP32
#define FLASH_PIN 4 //ESP32 CAM Flash
#define LED_PIN 33 //ESP32 CAM LED

typedef struct
{
  int reqCount;
  int reg1;
  byte reg2;
  char reg3;
  double reg4;
  char reg5[5];
} __attribute__((packed)) reg;

typedef struct
{
  int requestCount;
  byte command;
} __attribute__((packed)) req;

void updateRegisters();
void onReceive(int size);
void onRequest();

reg registers;
req requestCommand;

int loopCount;
int ledCount;
int flashCount;

void setup() 
{
  Serial.begin(115200);
  while(!Serial)
    delay(1);

  if(!Wire.begin((uint8_t)I2C_DEV_ADDR, SDA0_Pin, SCL0_Pin, I2C_Freq))
  {
    Serial.println("I2C Wire Error. Going idle.");
    while(true)
      delay(1);
  }
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest); 
  randomSeed(0);
  updateRegisters();
  pinMode(LED_PIN, OUTPUT);
  pinMode(FLASH_PIN, OUTPUT);
  Serial.println("Slave engaged.");

  loopCount = 0;
  ledCount = 25;
  flashCount =25;
  delay(500);
}

void loop() 
{
  static int command = -1;
  if(loopCount == 480)
    loopCount = 0;

  if(ledCount == 0)
     digitalWrite(LED_PIN, HIGH);
  if(ledCount == 25)
    digitalWrite(LED_PIN, LOW);

  if(flashCount == 0)
    digitalWrite(FLASH_PIN, HIGH);
  if(flashCount == 25)
    digitalWrite(FLASH_PIN, LOW);

  if(loopCount == 0)
    updateRegisters();


  loopCount++;
  ledCount++;
  flashCount++;
  if(requestCommand.command != 255 && requestCommand.command != command)
  {
    Serial.print("Command: "); Serial.print(requestCommand.command);
    Serial.print(" request count: "); Serial.println(requestCommand.requestCount);
    command = requestCommand.command;
    ledCount = 0;
  }
  delay(1);
}

void updateRegisters()
{
  int i;
  registers.reg1 = 55500+random(0,99);
  registers.reg2 = 250+random(0,5);
  registers.reg3 = random(97,122);
  registers.reg4 = 22.0+(double)random(0,1000)/1000.0;
  for(i=0; i<4; i++)
    registers.reg5[i] = random(97,122);
  registers.reg5[i] = '\0';
}

void onReceive(int size)
{
  if(size==sizeof(req))
    Wire.readBytes((char *)&requestCommand, sizeof(req));
    registers.reqCount = requestCommand.requestCount;
}
void onRequest()
{
  switch(requestCommand.command)
  {
    case 0: Wire.write((uint8_t *)&registers.reqCount, sizeof(int)); flashCount = 0; break;
    case 1: Wire.write((uint8_t *)&registers.reg1, sizeof(int)); break;
    case 2: Wire.write(registers.reg2); break;
    case 3: Wire.write(registers.reg3); break;
    case 4: Wire.write((uint8_t *)&registers.reg4, sizeof(double)); break;
    case 5: Wire.write((uint8_t *)registers.reg5,5); break;
    default: Wire.write(0); //single byte for dummy
  }
}
