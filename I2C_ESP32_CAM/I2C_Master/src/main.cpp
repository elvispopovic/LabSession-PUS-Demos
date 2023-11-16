
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SDA0_Pin 21
#define SCL0_Pin 22
#define I2C_Freq 100000
#define I2C_DEV_ADDR 0x10
#define LED_PIN 5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define REQUEST_DELAY 10
#define DISPLAY_DELAY 1000

typedef struct
{
  int requestCount;
  byte command;
} __attribute__((packed)) req;

void bufferPrefiil(int requestNumber, byte registerNumber);
void requestCommand(int requestNumber, byte registerNumber, int *response); 
void requestCommand(int requestNumber, byte registerNumber, byte *response);
void requestCommand(int requestNumber, byte registerNumber, double *response); //double
void requestCommand(int requestNumber, byte registerNumber, char *response, int length); //string
void I2cTransmit(int requestNumber, byte registerNumber);
void I2cRead(int    *response); //int
void I2cRead(byte    *response); //byte
void I2cRead(double *response); //double
void I2cRead(char *response, int length); //string

bool initScreen();
void WriteToScreen(int req, int reg, String value);

const char regTypes[][11]={"req. count","int","byte","char","double","string"};
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //OLED
bool screenAvailable;

void setup() 
{
  int c;
  Serial.begin(115200);
  if(!Wire.begin(SDA0_Pin, SCL0_Pin, I2C_Freq)) //starting I2C Wire
  {
    Serial.println("I2C Wire Error. Going idle.");
    while(true)
      delay(1);
  }
  initScreen();
  pinMode(LED_PIN,OUTPUT);
  Serial.println("Master engaged.");
}

void loop() 
{
  static int requestCount = 0;
  int responseReqCount;
  int r1;
  byte r2;
  char r3;
  double r4;
  char r5[5];

  digitalWrite(LED_PIN, HIGH);
  requestCommand(requestCount, 0, &responseReqCount);
  Serial.printf("Request %d, reqCount (response): %d\n",requestCount,responseReqCount);
  WriteToScreen(requestCount,0,String(responseReqCount));
  requestCount++;
  digitalWrite(LED_PIN, LOW);

  delay(DISPLAY_DELAY);

  digitalWrite(LED_PIN, HIGH);
  requestCommand(requestCount, 1, &r1);
  printf("Request %d, register 1 (int): %d\n",requestCount,r1);
  WriteToScreen(requestCount,1,String(r1));
  requestCount++;
  digitalWrite(LED_PIN, LOW);

  delay(DISPLAY_DELAY);
  
  digitalWrite(LED_PIN, HIGH);
  requestCommand(requestCount, 2, &r2);
  Serial.printf("Request %d, register 2 (byte): %d\n",requestCount,r2);
  WriteToScreen(requestCount,2,String(r2));
  requestCount++;
  digitalWrite(LED_PIN, LOW);

    delay(DISPLAY_DELAY);
  
  digitalWrite(LED_PIN, HIGH);
  requestCommand(requestCount, 3, (byte *)&r3);
  Serial.printf("Request %d, register 3 (char): %c\n",requestCount,r3);
  WriteToScreen(requestCount,3,String(r3));
  requestCount++;
  digitalWrite(LED_PIN, LOW);

  delay(DISPLAY_DELAY);
  
  digitalWrite(LED_PIN, HIGH);
  requestCommand(requestCount, 4, &r4);
  Serial.printf("Request %d, register 4 (double): %.2f\n",requestCount,r4);
  WriteToScreen(requestCount,4,String(r4));
  requestCount++;
  digitalWrite(LED_PIN, LOW);

  delay(DISPLAY_DELAY);
  
  digitalWrite(LED_PIN, HIGH);
  requestCommand(requestCount, 5, r5, 5);
  Serial.printf("Request %d, register 5 (string): %s\n",requestCount,r5);
  WriteToScreen(requestCount,5,String(r5));
  requestCount++;
  digitalWrite(LED_PIN, LOW);
  delay(DISPLAY_DELAY);
}

void bufferPrefiil(int requestNumber, byte registerNumber)
{
  I2cTransmit(requestNumber, registerNumber); //send register command
  delay(REQUEST_DELAY);
  if(Wire.requestFrom(I2C_DEV_ADDR,1) == 1)
    Wire.read(); //read old dummy
}

void requestCommand(int requestNumber, byte registerNumber, int *response)
{
  bufferPrefiil(requestNumber, registerNumber);
  //again - after ESP32 buffer prefill
  I2cTransmit(requestNumber, 255); //send dummy command
  delay(REQUEST_DELAY);
  I2cRead(response); //read register
}

void requestCommand(int requestNumber, byte registerNumber, byte *response)
{
  bufferPrefiil(requestNumber, registerNumber);
  //again - after ESP32 buffer prefill
  I2cTransmit(requestNumber, 255); //send dummy command
  delay(REQUEST_DELAY);
  I2cRead(response); //read register
}

void requestCommand(int requestNumber, byte registerNumber, double *response)
{
  bufferPrefiil(requestNumber, registerNumber);
  //again - after ESP32 buffer prefill
  I2cTransmit(requestNumber, 255); //send dummy command
  delay(REQUEST_DELAY);
  I2cRead(response); //read register
}

void requestCommand(int requestNumber, byte registerNumber, char *response, int length)
{
  bufferPrefiil(requestNumber, registerNumber);
  //again - after ESP32 buffer prefill
  I2cTransmit(requestNumber, 255); //send dummy command
  delay(REQUEST_DELAY);
  I2cRead(response, length); //read register
}

void I2cTransmit(int requestNumber, byte registerNumber)
{
  req command;
  command.command = registerNumber;
  command.requestCount = requestNumber;
  Wire.beginTransmission(I2C_DEV_ADDR); //start condition
  Wire.write((uint8_t*)&command, sizeof(req)); //write to slave register
  Wire.endTransmission(); // end condition
}

void I2cRead(int *response)
{
  if(Wire.requestFrom(I2C_DEV_ADDR,sizeof(int)) == sizeof(int)) //read from device register
    Wire.readBytes((char *)response, sizeof(int)); //read register
}

void I2cRead(byte *response)
{
  if(Wire.requestFrom(I2C_DEV_ADDR,1) == 1) //read from device register
    Wire.readBytes((char *)response, 1); //read register
}

void I2cRead(double *response)
{
  if(Wire.requestFrom(I2C_DEV_ADDR,sizeof(double)) == sizeof(double)) //read from device register
    Wire.readBytes((char *)response, sizeof(double)); //read register
}

void I2cRead(char *response, int length)
{
  if(Wire.requestFrom(I2C_DEV_ADDR,length) == length) //read from device register
    Wire.readBytes((char *)response, length); //read register
}


bool initScreen()
{
  bool result;
  screenAvailable = false;
  result = screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if(result)
  {
    screen.setTextColor(WHITE);
    screen.setTextSize(2);
    screen.clearDisplay();
    screen.display();
    screenAvailable = true;
  }
  return result;
}

void WriteToScreen(int req, int reg, String value)
{
  if(!screenAvailable)
    return;
  screen.clearDisplay(); 
  screen.setTextSize(2);
  screen.drawRect(0,0,127,63,WHITE); 
  screen.setCursor(4,4); 
  screen.printf("Req %d\n", req);
  screen.setCursor(4,22); 
  if(reg==0)
    screen.printf("C: %s",value.c_str());
  else
    screen.printf("r%d: %s",reg,value.c_str());
  screen.setCursor(4,40);
  screen.setTextSize(1);
  screen.printf("Type: %s",regTypes[reg]);
 
  screen.display();  
}

