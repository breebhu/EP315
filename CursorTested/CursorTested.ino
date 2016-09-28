#include<openGLCD.h>
#include<math.h>

// int buttonPin = ;
int joyPin1 = A5;                 // slider variable connecetd to analog pin 0
int joyPin2 = A4;
int SWPin = 3;
// slider variable connecetd to analog pin 1
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;
int value3 = -1;
bool penLift = false;
// variable to read the value from the analog pin 1
int x, y;
int real_image[3][3] = {0};

void setup() {
  GLCD.Init();
  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
  GLCD.CursorTo(x, y);
  pinMode(joyPin1, INPUT);
  pinMode(joyPin2, INPUT);
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
  Serial.begin(9600);
}

void loop() {

  delay(150);
  value1 = analogRead(joyPin1);
  delay(10);
  value2 = analogRead(joyPin2);
  value3 = digitalRead(SWPin);

  GLCD.SetDot((x - 1) % 128, (y - 1) % 64, real_image[0][0]);
  GLCD.SetDot((x - 1) % 128, y, real_image[0][1]);
  GLCD.SetDot((x - 1) % 128, (y + 1) % 64, real_image[0][2]);
  GLCD.SetDot(x, (y - 1) % 64, real_image[1][0]);
  GLCD.SetDot(x, y, real_image[1][1]); //May not need this
  GLCD.SetDot(x, (y + 1) % 64, real_image[1][2]);
  GLCD.SetDot((x + 1) % 128, (y - 1) % 64, real_image[2][0]);
  GLCD.SetDot((x + 1) % 128, y, real_image[2][1]);
  GLCD.SetDot((x + 1) % 128, (y + 1) % 64, real_image[2][2]);

  if (value2 > 985)
  {
    x = x + 1;
    if (x > 127)x = 0;
  }
  else if (value2 < 10)
  {
    x = x - 1;
    if (x < 0)x = 127;
  }
  if (value1 < 10)
  {
    y = y + 1;
    if (y > 63)y = 0;
  }
  else if (value1 > 985)
  {
    y = y - 1;
    if (y < 0)y = 63;
  }
  if (value3 == 0)
  {
    penLift = !(penLift);
  }

  GLCD.SetDot(x, y, BLACK);

  real_image[0][0] = ActualReadData((x - 1) % 128, (y - 1) % 64);
  real_image[1][0] = ActualReadData(x, (y - 1) % 64);
  real_image[2][0] = ActualReadData((x + 1) % 128, (y - 1) % 64);
  real_image[0][1] = ActualReadData((x - 1) % 128, y);
  real_image[1][1] = ActualReadData(x, y);
  real_image[2][1] = ActualReadData((x + 1) % 128, y);
  real_image[0][2] = ActualReadData((x - 1) % 128, (y + 1) % 64);
  real_image[1][2] = ActualReadData(x, (y + 1) % 64);
  real_image[2][2] = ActualReadData((x + 1) % 128, (y + 1) % 64);

  GLCD.SetDot(x, y, BLACK);
  GLCD.SetDot(x + 1, y, BLACK);
  GLCD.SetDot(x - 1, y, BLACK);
  GLCD.SetDot(x, (y + 1) % 64, BLACK);
  GLCD.SetDot(x, y - 1, BLACK);

}

int ActualReadData(int x, int y) {
  uint8_t bitarray[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  GLCD.GotoXY(x, y);
  uint8_t data = GLCD.ReadData();
  if (data & bitarray[y % 8]) return BLACK;
  else return WHITE;
}
