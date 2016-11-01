#include<openGLCD.h>
#include<math.h>

// int buttonPin = ;
int joyPin1 = A5;                 // slider variable connecetd to analog pin 0
int joyPin2 = A4;
int SWPin = 19; // interchanged with BSCP=2
int ErasePin = 3;
int ClearPin = 18;
int BrushSizeChangePin = 2;
// slider variable connecetd to analog pin 1
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;
volatile bool penLift = false;
volatile bool eraseMode = false;
volatile bool clearScreen = false;
volatile int brushType = 0;
// variable to read the value from the analog pin 1
int x, y;
int real_image[5][5] = {0};

void setup() {
  GLCD.Init();
  pinMode(joyPin1, INPUT);
  pinMode(joyPin2, INPUT);
  pinMode(SWPin, INPUT);
  digitalWrite(SWPin, HIGH);
  pinMode(ErasePin, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(SWPin), penLiftISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ErasePin), eraseModeISR, RISING);
  attachInterrupt(digitalPinToInterrupt(ClearPin), clearScreenISR, RISING);
  attachInterrupt(digitalPinToInterrupt(BrushSizeChangePin), brushSizeChangeISR, RISING);

  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
  GLCD.CursorTo(x, y);
  Serial.begin(9600);
}

void loop() {
  delay(150);
  value1 = analogRead(joyPin1);
  delay(10);
  value2 = analogRead(joyPin2);

  for (int i = -2; i < 3; i++) {
    for (int j = -2; j < 3; j++) {
      GLCD.SetDot(x + j, y + i, real_image[2 + j][2 + i]);
    }
  }

  if (value2 > 900)
  {
    x = x + 1;
    if (x > 127)x = 0;
  }
  else if (value2 < 100)
  {
    x = x - 1;
    if (x < 0)x = 127;
  }
  if (value1 < 100)
  {
    y = y + 1;
    if (y > 63)y = 0;
  }
  else if (value1 > 900)
  {
    y = y - 1;
    if (y < 0)y = 63;
  }

  //if (!penLift) {
    //if (!eraseMode) {
      GLCD.SetDot(x, y, BLACK);
   // }
    //else {
    //  GLCD.SetDot(x, y, WHITE);
   // }
  //}

  for (int i = -2; i < 3; i++) {
    for (int j = -2; j < 3; j++) {
      real_image[2 + j][2 + i] = ActualReadData((x + j) % 128, (y + i) % 64);
    }
  }
  Serial.println(brushType);
  setBrush(2);
  if (clearScreen) clearScreenFunc();
}

void penLiftISR() {
  penLift = !(penLift);
}

void eraseModeISR() {
  eraseMode = !(eraseMode);
}

void clearScreenISR() {
  clearScreen = true;
}

void brushSizeChangeISR() {
  if (brushType < 2) {
    brushType++;
  }
  else {
    brushType = 0;
  }
}

int setBrush(int BrushType) {
  if (BrushType == 0) {
    GLCD.SetDot(x, y, BLACK);
    GLCD.SetDot(x + 1, y, BLACK);
    GLCD.SetDot(x - 1, y, BLACK);
    GLCD.SetDot(x, y + 1, BLACK);
    GLCD.SetDot(x, y - 1, BLACK);
    return 0;
  }
  if (BrushType == 1) {
    GLCD.SetDot(x, y, BLACK);
    GLCD.SetDot(x + 1, y, BLACK);
    GLCD.SetDot(x, y - 1, BLACK);
    GLCD.SetDot(x+1, y - 1, BLACK);
    return 0;
  }
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      GLCD.SetDot(x + i, y + j, BLACK);
    }
  }
  GLCD.SetDot(x + 2, y, BLACK);
  GLCD.SetDot(x - 2, y, BLACK);
  GLCD.SetDot(x, y + 2, BLACK);
  GLCD.SetDot(x, y - 2, BLACK);
  return 0;
}

int ActualReadData(int x, int y) {
  uint8_t bitarray[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
  GLCD.GotoXY(x, y);
  uint8_t data = GLCD.ReadData();
  if (data & bitarray[y % 8]) return BLACK;
  else return WHITE;
}

void clearScreenFunc() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      real_image[i][j] = 0;
    }
  }
  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
  GLCD.CursorTo(x, y);
  clearScreen = false;
}



