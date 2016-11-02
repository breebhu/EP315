#include<openGLCD.h>
//#include<LiquidCrystal.h>
#include<math.h>

//LiquidCrystal lcd(38,39,40,42,44,46);

int joyPin1 = A5;                 // slider variable connecetd to analog pin 0
int joyPin2 = A4;
int PenLiftPin = 2;
int ErasePin = 3;
int ClearPin = 18;
int BrushSizeChangePin = 19;
int RectPin = 20;
int CircPin = 21;
// slider variable connecetd to analog pin 1
int value1 = 0;                  // variable to read the value from the analog pin 0
int value2 = 0;
volatile bool penLift = false;
volatile bool eraseMode = false;
volatile bool clearScreen = false;
volatile bool rectMode = false;
volatile bool circMode = false;
volatile int brushType = 0;

// variable to read the value from the analog pin 1
int x, y;
int real_image[5][5] = {0};

void setup() {
  InitLCDs();
}

void loop() {
}

void InitLCDs() {
  GLCD.Init();
//  lcd.begin(16,2);
  
  InitScreen(500000);
  
  x = GLCD.CenterX; y = GLCD.CenterY;
  real_image[1][1] = 1;
  GLCD.ClearScreen();
//  lcd.clear();

//  lcd.print("BrushSize:  , Mode: ");
  GLCD.CursorTo(x, y);
}

void InitScreen(long delaytime) {
  GLCD.DefineArea(textAreaTOP, TimesNewRoman16_bold);
  GLCD.DrawString("LCD Painter", gTextfmt_center, gTextfmt_center);
  GLCD.DefineArea(textAreaBOTTOM, Callibri10);
  GLCD.DrawString("Created by\n Reebhu Bhattacharyya\n Archit Bhatnagar", gTextfmt_left, gTextfmt_center);
//  lcd.setCursor(0,0);
//  lcd.print("Welcome, Let's Paint");
  delay(delaytime);
}

//void InitPin() {
//  pinMode(PenLiftPin, INPUT);
//  digitalWrite(PenLiftPin, HIGH);
//
//    attachInterrupt(digitalPinToInterrupt(PenLiftPin), penLiftISR, FALLING);
//    attachInterrupt(digitalPinToInterrupt(ErasePin), eraseModeISR, RISING);
//    attachInterrupt(digitalPinToInterrupt(ClearPin), clearScreenISR, RISING);
//    attachInterrupt(digitalPinToInterrupt(BrushSizeChangePin), brushSizeChangeISR, RISING);
//}
